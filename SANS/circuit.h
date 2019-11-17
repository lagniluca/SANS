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

#ifndef SUBCIRCUIT_H
#define SUBCIRCUIT_H

#include <string>
#include <vector>
#include <set>
#include <map>

namespace SubCircuit_v2
{
	class Graph
	{
		protected:
			struct BitRef
			{
				int nodeIdx, portIdx, bitIdx;
				BitRef(int nodeIdx = -1, int portIdx = -1, int bitIdx = -1) : nodeIdx(nodeIdx), portIdx(portIdx), bitIdx(bitIdx) { };
				bool operator < (const BitRef &other) const;
			};

			struct Edge
			{
				std::set<BitRef> portBits;
				int constValue;
				bool isExtern;
				Edge() : constValue(0), isExtern(false) { };
			};

			struct PortBit
			{
				int edgeIdx;
				PortBit() : edgeIdx(-1) { };
			};

			struct Port
			{
				std::string portId;
				int minWidth;
				std::vector<PortBit> bits;
				Port() : minWidth(-1) { };
			};

			struct Node
			{
				std::string nodeId, typeId;
				std::map<std::string, int> portMap;
				std::vector<Port> ports;
				void *userData;
				bool shared;
				Node() : userData(NULL), shared(false) { };
			};

			bool allExtern;
			std::map<std::string, int> nodeMap;
			std::vector<Node> nodes;
			std::vector<Edge> edges;

		public:
			Graph() : allExtern(false) { };
			Graph(const Graph &other, const std::vector<std::string> &otherNodes);

			void createNode(std::string nodeId, std::string typeId, void *userData = NULL, bool shared = false);
			void createPort(std::string nodeId, std::string portId, int width = 1, int minWidth = -1);
			void createConnection(std::string fromNodeId, std::string fromPortId, int fromBit, std::string toNodeId, std::string toPortId, int toBit, int width = 1);
			void createConnection(std::string fromNodeId, std::string fromPortId, std::string toNodeId, std::string toPortId);
			void createConstant(std::string toNodeId, std::string toPortId, int toBit, int constValue);
			void createConstant(std::string toNodeId, std::string toPortId, int constValue);
			std::vector<std::pair<int, int>> getAllExtern();
			void markExtern(std::string nodeId, std::string portId, int bit = -1);
			void markAllExtern();
			void print();

			friend class SolverWorker;
	};
}

#endif /* SUBCIRCUIT_H */
