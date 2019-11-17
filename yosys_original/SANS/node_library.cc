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

#include "node_library.h"

Graph_Node::Node::Node(int nodeId, std::string nodeName, std::string nodeType, bool modulePlaceholder) : nodeId(nodeId), name(nodeName), type(nodeType), modulePlaceholder(modulePlaceholder) {}

int Graph_Node::Node::getId() { return this->nodeId; }
std::string Graph_Node::Node::getName() { return this->name; }
std::string Graph_Node::Node::getType() { return this->type; }

bool Graph_Node::Node::isEmitter()
{
	for( auto outputPort : getOutputs() )
	{
		if( outputPort->isConnectedToExtern() )
		{
			return true;
		}
	}
	
	return false;
}

bool Graph_Node::Node::isModulePlaceholder()
{
	return this->modulePlaceholder;
}

std::vector<Graph_Port::Port *> Graph_Node::Node::getInputs()
{
	std::vector<Graph_Port::Port *> temp;
	temp = VectorSupport::merge( temp, inputPorts );
	temp = VectorSupport::merge( temp, inoutPorts );
	return temp;
}

std::vector<Graph_Port::Port *> Graph_Node::Node::getOutputs()
{
	std::vector<Graph_Port::Port *> temp;
	temp = VectorSupport::merge( temp, outputPorts );
	temp = VectorSupport::merge( temp, inoutPorts );
	return temp;
}

std::vector<Graph_Port::Port *> Graph_Node::Node::getPorts()
{
	std::vector<Graph_Port::Port *> temp;
	temp = VectorSupport::merge( temp, inputPorts );
	temp = VectorSupport::merge( temp, outputPorts );
	temp = VectorSupport::merge( temp, inoutPorts );
	return temp;
}

Graph_Port::Port* Graph_Node::Node::getPort(int id)
{
	for( auto port : this->getPorts() )
	{
		if( port->getPortId() == id )
		{
			return port;
		}
	}

	Graph_Port::Port* errorPort = new Graph_Port::Port("Error_Port");
	return errorPort;
}

int Graph_Node::Node::addPort(Graph_Port::Port *newPort, Graph_Port::PortDirection direction)
{
	for( auto port: this->inputPorts )
	{
		if( port->getPortName() == newPort->getPortName() )
			return -1;
	}

	for( auto port: this->outputPorts )
	{
		if( port->getPortName() == newPort->getPortName() )
			return -2;
	}

	for( auto port: this->inoutPorts )
	{
		if( port->getPortName() == newPort->getPortName() )
			return -3;
	}

	switch(direction)
	{
		case Graph_Port::PortDirection::INPUT:
			this->inputPorts.push_back(newPort);
		break;

		case Graph_Port::PortDirection::OUTPUT:
			this->outputPorts.push_back(newPort);
		break;

		case Graph_Port::PortDirection::INOUT:
			this->inoutPorts.push_back(newPort);
		break;

		default:
			std::cout << "Geeettttttt dunked on!!!" << std::endl;
			return -4;
	}
	return 1;
}

std::vector<Signal::Signal> Graph_Node::Node::getInputSignals()
{
	std::vector<Signal::Signal> temp;

	for( auto port : this->getInputs() )
	{
		temp = VectorSupport::merge( temp, port->getInputSignals() );
	}

	return temp;
}

std::vector<Signal::Signal> Graph_Node::Node::getOutputSignals()
{
	std::vector<Signal::Signal> temp;

	for( auto port : this->getOutputs() )
	{
		temp = VectorSupport::merge( temp, port->getOutputSignals() );
	}

	return temp;
}

bool Graph_Node::Node::setModulePort(Graph_Port::Port *newPort,bool data,bool selection)
{
    bool found = false;
    for(auto ip : this->getInputs())
    {
        if(ip == newPort)
        {
            found = true;
        }
    }
    
    if(found)
    {
        if(data)
        {
            this->moduleDataPorts = VectorSupport::conditionalInsert(this->moduleDataPorts, newPort);
        }
        
        if(selection)
        {
           this->moduleSelectionPorts = VectorSupport::conditionalInsert(this->moduleSelectionPorts, newPort); 
        }
    }
    
    return found;
}

/**
  * In case of a module_placeholder, we used this method for 
  * getting the selection ports
*/
std::vector<Graph_Port::Port *> Graph_Node::Node::getModuleSelectionPorts()
{
    return this->moduleSelectionPorts;
}
            
/**
 * In case of a module_placeholder, we used this method for 
 * getting the data ports
*/
std::vector<Graph_Port::Port *> Graph_Node::Node::getModuleDataPorts()
{
    return this->moduleDataPorts;
}
            
/**
 * In case of a module_placeholder, we used this method for 
 * getting the selection and data ports
 */
std::vector<Graph_Port::Port *>  Graph_Node::Node::getModuleDataSelectionPorts()
{
    std::vector<Graph_Port::Port *> res = this->moduleDataPorts;
    
    res = VectorSupport::merge(res, this->moduleSelectionPorts);
    
    return res;
}

std::string Graph_Node::Node::toString()
{
	std::stringstream ss;
	ss << "Node #" << this->nodeId << "\t" << this->name << "   of type:\t" << this->type << "\t__________________________________________________\n";
	
	ss << "IsEmitter : " << this->isEmitter() << "\n";
	ss << "IsModulePlaceholder: " << this->isModulePlaceholder() << "\n";

	ss << "\nInput ports:";
	for(auto port: this->inputPorts)
	{
		ss << port->toString();
	}

	ss << "\nOutput ports:";
	for(auto port: this->outputPorts)
	{
		ss << port->toString();
	}

	ss << "\nInout ports:";
	for(auto port: this->inoutPorts)
	{
		ss << port->toString();
	}

	ss  << "\n\n__________________________________________________________________________________________________________________________________________________\n";

	return ss.str();
}
