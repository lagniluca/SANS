/*
 *
 *  Copyright (C) 2019  Cugini Lorenzo <lorenzo.cugini@mail.polimi.it>
 *                      Lagni Luca <luca.lagni@mail.polimi.it>
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
 */

#ifndef SUPPORT_H
#define SUPPORT_H

//Application libraries
#include "node_library.h"
#include "stdvector_support.h"
#include "cellrepo.h"

USING_YOSYS_NAMESPACE

namespace SupportGraph
{
	/*
	 *	The graph represented by this class is the one containing all infos needed.
	 */
	class SupportGraph
	{
		private:
			//Tech repository to distinguish between data and selection signals.
			CellRepository::CellRepository *cr = new CellRepository::CellRepository();
			//Node list.
			std::vector<Graph_Node::Node *> nodes;
			//Count of the nodes. Incremented each time a node is added.
			int numOfNodes = 0;		

		public:
			//Getters
			CellRepository::CellRepository* getCellRepository();
			std::vector<Graph_Node::Node *> getNodes();
			int getNumOfNodes();

			/*
			 *	Get node from its id
			 */
			Graph_Node::Node* getNode(int id);

			/*
			 *	Check if the passed name is the one of an existing node.
			 */
			bool isNodeName( std::string name );

			/*
			 *	Add a node to the node list. Also increment the node count.
			 */
			int addNode(Graph_Node::Node *newNode);

			/*
			 *	Given a node and a port, find out what's reachable forwards.
			 */
			std::vector<Graph_Node::Node *> nextNodes(Graph_Node::Node *node, Graph_Port::Port *port, bool discriminating, bool verbose=false);

			/*
			 *	Extracts all the next nodes of a given node.
			 */
			std::vector<Graph_Node::Node *> allNextNodes(Graph_Node::Node *node, bool discriminating, bool verbose=false);

			/*
			 *	<srcNode, srcPort> ---> <dstNode, ?>
			 *	Returns each port together with the number of bits that enters there from my port.
			 */
			std::vector< std::pair<Graph_Port::Port *, int> > reachedPorts( Graph_Node::Node *sourceNode, Graph_Port::Port *sourcePort, Graph_Node::Node *dstNode );

			/*
			 *	toString() interface
			 */
			std::string toString();
	};
}

#endif
