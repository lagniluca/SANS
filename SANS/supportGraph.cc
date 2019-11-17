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

#include "supportGraph.h"

CellRepository::CellRepository* SupportGraph::SupportGraph::getCellRepository() { return this->cr; }
std::vector<Graph_Node::Node *> SupportGraph::SupportGraph::getNodes() { return this->nodes; }
int SupportGraph::SupportGraph::getNumOfNodes() { return this->numOfNodes; }

Graph_Node::Node* SupportGraph::SupportGraph::getNode(int id)
{
	for( auto node : this->nodes )
	{
		if( node->getId() == id )
		{
			return node;
		}
	}

	Graph_Node::Node* errorNode = new Graph_Node::Node(-1, "Error_Node", "Error_Node");
	return errorNode;
}

bool SupportGraph::SupportGraph::isNodeName( std::string name )
{
	for( auto node : this->nodes )
	{
		if( node->getName() == name )
		{
			return true;
		}
	}
	return false;
}

int SupportGraph::SupportGraph::addNode(Graph_Node::Node *newNode)
{
	for( auto node: this->nodes )
	{
		if( node->getName() == newNode->getName() )
		{
			//Node name is not unique.
			return -1;
		}
	}

	this->nodes.push_back(newNode);
	this->numOfNodes++;

	return 1;
}

std::vector<Graph_Node::Node *> SupportGraph::SupportGraph::nextNodes(Graph_Node::Node *node, Graph_Port::Port *port, bool discriminating, bool verbose)
{
	std::vector<Graph_Node::Node *> next;
	std::stringstream ss;

	if(verbose)
	{
		ss << "\tExecution of nextNodes for " << node->getName() << " : " << port->getPortName() << "\n";
	}

	for( auto outputPort : node->getOutputs() )
	{
		/*
		 *	Portname is unique wrt the component. Since I pass a port supposedly correctly and I search in the component outputs, I can presume it's unique enough.
		 *	It's a basic optimization of the cycle to improve temporal complexity.
		 */
		//We match the right port of the node
		if( outputPort->getPortName() == port->getPortName() )
		{
			//For each one of the edges connected to this port, I've to retrieve the other end of the edge.
			for( auto outputEdge : outputPort->getEdges() )
			{
				//Check each node
				for( auto otherNode : getNodes() )
				{
					//We need this, otherwise we find ourselves while searching for other nodes.
					if( node->getName() != otherNode->getName() )
					{
						//And each input port
						for( auto inputPort : otherNode->getInputs() )
						{
							for( auto inputEdge : inputPort->getEdges() )
							{
								//We've found a matching edge in another node
								if(outputEdge == inputEdge)
								{
									//If we're discriminating between selection and communication, then execute this block.
									if(discriminating)
									{
										/*  
										 *	If the input port corresponds to a selection signal, we do not need it since we're explicitly
										 *	discriminating between selection and communication signals.
										 */		
										bool selectionPort = false;
                                        
                                        if(!otherNode->isModulePlaceholder())
                                        {
                                            if( VectorSupport::isIn( (cr->getCell(otherNode->getType()))->getSelectionInputs(), inputPort->getPortName() ) )
                                            {
                                                selectionPort = true;
                                            }
                                        }
                                        else
                                        {
                                            for(auto selectionInput : node->getModuleSelectionPorts())
                                            {
                                                if(selectionInput->getPortName() == inputPort->getPortName())
                                                {
                                                    selectionPort = true;
                                                    
                                                    for(auto dataInput : node->getModuleDataPorts())
                                                    {
                                                        if(dataInput->getPortName() == selectionInput->getPortName())
                                                        {
                                                            selectionPort = false;
                                                        }
                                                    }
                                                    
                                                }
                                            }
                                        }

										if(!selectionPort)
										{
											next = VectorSupport::conditionalInsert(next, otherNode);

											if(verbose)
											{
												ss << "\tMatched edge: " << outputEdge << " of node " << node->getName() << " port: " << inputPort->getPortName() << "\n";
											}
										}
									}
									else
									{
										next = VectorSupport::conditionalInsert(next, otherNode);

										if(verbose)
										{
											ss << "\tMatched edge: " << outputEdge << " of node " << node->getName() << " port: " << inputPort->getPortName() << "\n";
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if(verbose)
	{
		ss << "\n";

		ss << "Resulting std::vector<Node *>\n";

		for( auto node: next )
		{
			ss << node->getName() << "\n";
		}

		ss << "\n\n";
		std::cout << ss.str();
	}

	return next;
}

std::vector<Graph_Node::Node *> SupportGraph::SupportGraph::allNextNodes(Graph_Node::Node *node, bool discriminating, bool verbose)
{
	std::stringstream ss;
	std::vector<Graph_Node::Node *> reachable;

	for( auto outputPort : node->getOutputs() )
	{
		std::vector<Graph_Node::Node *> appendable = nextNodes(node, outputPort, discriminating);
		reachable = VectorSupport::append(reachable, appendable);
	}

	if(verbose)
	{
		std::cout << ss.str() << std::endl;
	}

	return reachable;
}

std::vector< std::pair<Graph_Port::Port *, int> > SupportGraph::SupportGraph::reachedPorts( Graph_Node::Node *sourceNode, Graph_Port::Port *sourcePort, Graph_Node::Node *dstNode )
{
	std::vector< std::pair<Graph_Port::Port *, int> > temp;

	//If the srcport is not from srcnode, this analysis is meaningless.
	if( !VectorSupport::isIn(sourceNode->getPorts(), sourcePort ) )
	{
		return temp;
	}

	//Take destination node's input ports
	for( auto dstPort : dstNode->getInputs() )
	{
		temp.push_back( std::make_pair(dstPort, 0) );
		//And all the edges connected to each port
		for( auto inputEdge : dstPort->getEdges() )
		{
			//Foreach edge that exits from my port
			for( auto outputEdge : sourcePort->getEdges() )
			{
				//If one of my edges matches one of its edges
				if( outputEdge == inputEdge )
				{
					//Pairs are intended as values, the auto& forces the reference
					for( auto& pair : temp )
					{
						if( pair.first->getPortName() == dstPort->getPortName() )
						{
							pair.second++;
						}
					}
				}
			}
		}
	}

	std::vector< std::pair<Graph_Port::Port *, int> > final;

	for( auto pair : temp )
	{
		if( pair.second > 0 )
		{
			final.push_back(pair);
		}
	}

	return final;
}

std::string SupportGraph::SupportGraph::toString()
{
	std::stringstream ss;

	ss << "\n______________________________ Graph toString ______________________________\n\n";

	ss << "#Nodes: " << this->numOfNodes << "\n";

	for( auto node : this->nodes )
	{
		ss << "\n\n\n" << node->toString();
	}

	return ss.str();
}
