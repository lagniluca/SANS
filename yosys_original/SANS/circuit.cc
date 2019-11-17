
/*
 *  SubCircuit -- An implementation of the Ullmann Subgraph Isomorphism
 *                algorithm for coarse grain logic networks
 *
 *  Copyright (C) 2013  Clifford Wolf <clifford@clifford.at>
 *
 *  Permission to use, copy, modify, and/or distribute this software for any
 *  purpose with or without fee is hereby granted, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *	This library is an extension of a small part of the forementioned library.
 *	We named it _v2 to differentiate it from the original one, which is still
 *	used in Yosys, to avoid conflicts.
 *
 */

#include "circuit.h"

#include <algorithm>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#ifndef VECSUPPORT_H
#define VECSUPPORT_H
#include "stdvector_support.h"
#endif

#ifdef _YOSYS_
#  include "kernel/yosys.h"
#  define my_printf YOSYS_NAMESPACE_PREFIX log
#else
#  define my_printf printf
#endif

using namespace SubCircuit_v2;

#ifndef _YOSYS_
static std::string my_stringf(const char *fmt, ...)
{
	std::string string;
	char *str = NULL;
	va_list ap;

	va_start(ap, fmt);
	if (vasprintf(&str, fmt, ap) < 0)
		str = NULL;
	va_end(ap);

	if (str != NULL) {
		string = str;
		free(str);
	}

	return string;
}
#else
#  define my_stringf YOSYS_NAMESPACE_PREFIX stringf
#endif

SubCircuit_v2::Graph::Graph(const Graph &other, const std::vector<std::string> &otherNodes)
{
	allExtern = other.allExtern;

	std::map<int, int> other2this;
	for (int i = 0; i < int(otherNodes.size()); i++)
	{
		assert(other.nodeMap.count(otherNodes[i]) > 0);
		other2this[other.nodeMap.at(otherNodes[i])] = i;
		nodeMap[otherNodes[i]] = i;
	}

	std::map<int, int> edges2this;
	for (auto &i1 : other2this)
	for (auto &i2 : other.nodes[i1.first].ports)
	for (auto &i3 : i2.bits)
		if (edges2this.count(i3.edgeIdx) == 0) {
			int next_idx = edges2this.size();
			edges2this[i3.edgeIdx] = next_idx;
		}

	edges.resize(edges2this.size());
	for (auto &it : edges2this) {
		for (auto &bit : other.edges[it.first].portBits)
			if (other2this.count(bit.nodeIdx) > 0)
				edges[it.second].portBits.insert(BitRef(other2this[bit.nodeIdx], bit.portIdx, bit.bitIdx));
		edges[it.second].constValue = other.edges[it.first].constValue;
		edges[it.second].isExtern = other.edges[it.first].isExtern;
	}

	nodes.resize(other2this.size());
	for (auto &it : other2this) {
		nodes[it.second] = other.nodes[it.first];
		for (auto &i2 : nodes[it.second].ports)
		for (auto &i3 : i2.bits)
			i3.edgeIdx = edges2this.at(i3.edgeIdx);
	}
}

bool SubCircuit_v2::Graph::BitRef::operator < (const BitRef &other) const
{
	if (nodeIdx != other.nodeIdx)
		return nodeIdx < other.nodeIdx;
	if (portIdx != other.portIdx)
		return portIdx < other.portIdx;
	return bitIdx < other.bitIdx;
}

void  SubCircuit_v2::Graph::createNode(std::string nodeId, std::string typeId, void *userData, bool shared)
{
	assert(nodeMap.count(nodeId) == 0);
	nodeMap[nodeId] = nodes.size();
	nodes.push_back(Node());

	Node &newNode = nodes.back();
	newNode.nodeId = nodeId;
	newNode.typeId = typeId;
	newNode.userData = userData;
	newNode.shared = shared;
}

void SubCircuit_v2::Graph::createPort(std::string nodeId, std::string portId, int width, int minWidth)
{
	assert(nodeMap.count(nodeId) != 0);
	int nodeIdx = nodeMap[nodeId];
	Node &node = nodes[nodeIdx];

	assert(node.portMap.count(portId) == 0);

	int portIdx = node.ports.size();
	node.portMap[portId] = portIdx;
	node.ports.push_back(Port());
	Port &port = node.ports.back();

	port.portId = portId;
	port.minWidth = minWidth < 0 ? width : minWidth;
	port.bits.insert(port.bits.end(), width, PortBit());

	for (int i = 0; i < width; i++)
	{
		//Increasing edge number assigned
		port.bits[i].edgeIdx = edges.size();
		edges.push_back(Edge());
		edges.back().portBits.insert(BitRef(nodeIdx, portIdx, i));
	}
}

void SubCircuit_v2::Graph::createConnection(std::string fromNodeId, std::string fromPortId, int fromBit, std::string toNodeId, std::string toPortId, int toBit, int width)
{
	assert(nodeMap.count(fromNodeId) != 0);
	assert(nodeMap.count(toNodeId) != 0);

	int fromNodeIdx = nodeMap[fromNodeId];
	Node &fromNode = nodes[fromNodeIdx];

	int toNodeIdx = nodeMap[toNodeId];
	Node &toNode = nodes[toNodeIdx];

	assert(fromNode.portMap.count(fromPortId) != 0);
	assert(toNode.portMap.count(toPortId) != 0);

	int fromPortIdx = fromNode.portMap[fromPortId];
	Port &fromPort = fromNode.ports[fromPortIdx];

	int toPortIdx = toNode.portMap[toPortId];
	Port &toPort = toNode.ports[toPortIdx];

	if (width < 0) {
		assert(fromBit == 0 && toBit == 0);
		assert(fromPort.bits.size() == toPort.bits.size());
		width = fromPort.bits.size();
	}

	assert(fromBit >= 0 && toBit >= 0);
	for (int i = 0; i < width; i++)
	{
		assert(fromBit + i < int(fromPort.bits.size()));
		assert(toBit + i < int(toPort.bits.size()));

		int fromEdgeIdx = fromPort.bits[fromBit + i].edgeIdx;

		int toEdgeIdx = toPort.bits[toBit + i].edgeIdx;


		if (fromEdgeIdx == toEdgeIdx)
			continue;

		// merge toEdge into fromEdge
		if (edges[toEdgeIdx].isExtern)	
			{
				edges[fromEdgeIdx].isExtern = true;
			}
		if (edges[toEdgeIdx].constValue) {
			assert(edges[fromEdgeIdx].constValue == 0);
			edges[fromEdgeIdx].constValue = edges[toEdgeIdx].constValue;
		}
		for (const auto &ref : edges[toEdgeIdx].portBits) {
			edges[fromEdgeIdx].portBits.insert(ref);
			nodes[ref.nodeIdx].ports[ref.portIdx].bits[ref.bitIdx].edgeIdx = fromEdgeIdx;
		}

		//Remove toEdge (move last edge over toEdge if needed)
		if (toEdgeIdx+1 != int(edges.size())) {
			edges[toEdgeIdx] = edges.back();
			for (const auto &ref : edges[toEdgeIdx].portBits)
				nodes[ref.nodeIdx].ports[ref.portIdx].bits[ref.bitIdx].edgeIdx = toEdgeIdx;
		}
		edges.pop_back();
	}
}

void SubCircuit_v2::Graph::createConnection(std::string fromNodeId, std::string fromPortId, std::string toNodeId, std::string toPortId)
{
	createConnection(fromNodeId, fromPortId, 0, toNodeId, toPortId, 0, -1);
}

void SubCircuit_v2::Graph::createConstant(std::string toNodeId, std::string toPortId, int toBit, int constValue)
{
	assert(nodeMap.count(toNodeId) != 0);
	int toNodeIdx = nodeMap[toNodeId];
	Node &toNode = nodes[toNodeIdx];

	assert(toNode.portMap.count(toPortId) != 0);
	int toPortIdx = toNode.portMap[toPortId];
	Port &toPort = toNode.ports[toPortIdx];

	assert(toBit >= 0 && toBit < int(toPort.bits.size()));
	int toEdgeIdx = toPort.bits[toBit].edgeIdx;

	assert(edges[toEdgeIdx].constValue == 0);
	edges[toEdgeIdx].constValue = constValue;
}

void SubCircuit_v2::Graph::createConstant(std::string toNodeId, std::string toPortId, int constValue)
{
	assert(nodeMap.count(toNodeId) != 0);
	int toNodeIdx = nodeMap[toNodeId];
	Node &toNode = nodes[toNodeIdx];

	assert(toNode.portMap.count(toPortId) != 0);
	int toPortIdx = toNode.portMap[toPortId];
	Port &toPort = toNode.ports[toPortIdx];

	for (int i = 0; i < int(toPort.bits.size()); i++) {
		int toEdgeIdx = toPort.bits[i].edgeIdx;
		assert(edges[toEdgeIdx].constValue == 0);
		edges[toEdgeIdx].constValue = constValue % 2 ? '1' : '0';
		constValue = constValue >> 1;
	}
}

void SubCircuit_v2::Graph::markExtern(std::string nodeId, std::string portId, int bit)
{
	assert(nodeMap.count(nodeId) != 0);
	Node &node = nodes[nodeMap[nodeId]];

	assert(node.portMap.count(portId) != 0);
	Port &port = node.ports[node.portMap[portId]];

	if (bit < 0) {
		for (const auto portBit : port.bits)
			edges[portBit.edgeIdx].isExtern = true;
	} else {
		assert(bit < int(port.bits.size()));
		edges[port.bits[bit].edgeIdx].isExtern = true;
	}
}

void SubCircuit_v2::Graph::markAllExtern()
{
	allExtern = true;
}

/*
 *	Returns a list of <nodeIdx, portIdx> which have been marked as extern
 */
std::vector<std::pair<int, int>> SubCircuit_v2::Graph::getAllExtern()
{
	std::vector<std::pair<int, int>> temp;

	for (int i = 0; i < int(nodes.size()); i++)
	{
		const Node &node = nodes[i];
		for (int j = 0; j < int(node.ports.size()); j++)
		{
			const Port &port = node.ports[j];
			for (int k = 0; k < int(port.bits.size()); k++)
			{
				int edgeIdx = port.bits[k].edgeIdx;
				
				for (const auto &ref : edges[edgeIdx].portBits)
				{
					if (edges[edgeIdx].isExtern)
					{
						bool exists = false;
						//Check if the pair already exists
						for( auto extistingPair : temp )
						{
							if( (extistingPair.first == ref.nodeIdx) && (extistingPair.second == ref.portIdx) )
							{
								exists = true;
							}
						}
						//If not, push it
						if(!exists)
						{
							temp.push_back( std::make_pair(ref.nodeIdx, ref.portIdx) );
						}
					}
				}
			}
		}
	}

	return temp;
}

void SubCircuit_v2::Graph::print()
{
	for (int i = 0; i < int(nodes.size()); i++)
	{
		const Node &node = nodes[i];
		my_printf("NODE %d: %s (%s)\n", i, node.nodeId.c_str(), node.typeId.c_str());
		for (int j = 0; j < int(node.ports.size()); j++)
		{
			const Port &port = node.ports[j];
			my_printf("  PORT %d: %s (%d/%d)\n", j, port.portId.c_str(), port.minWidth, int(port.bits.size()));
			for (int k = 0; k < int(port.bits.size()); k++)
			{
				int edgeIdx = port.bits[k].edgeIdx;
				my_printf("    BIT %d (%d):", k, edgeIdx);

				for (const auto &ref : edges[edgeIdx].portBits)
					my_printf(" %d.%d.%d", ref.nodeIdx, ref.portIdx, ref.bitIdx);

				if (edges[edgeIdx].isExtern)
					my_printf(" [extern]");
				
				my_printf("\n");
			}
		}
	}
}
