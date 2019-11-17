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

#include "port_library.h"

USING_YOSYS_NAMESPACE

Graph_Port::Port::Port() : portName(""), portId(-1), direction(INVALID), connectedToExtern(false) {}
Graph_Port::Port::Port(std::string portName) : portName(portName), portId(-1), direction(INVALID), connectedToExtern(false) {}
Graph_Port::Port::Port(std::string portName, int portId) : portName(portName), portId(portId), direction(INVALID), connectedToExtern(false) {}
Graph_Port::Port::Port(std::string portName, PortDirection portDirection) : portName(portName), portId(-1), direction(portDirection), connectedToExtern(false) {}
Graph_Port::Port::Port(std::string portName, PortDirection portDirection, int portId) : portName(portName), portId(portId), direction(portDirection), connectedToExtern(false) {}

//Getters
std::string Graph_Port::Port::getPortName() { return this->portName; }
int Graph_Port::Port::getPortId() { return this->portId; }
Graph_Port::PortDirection Graph_Port::Port::getPortDirection() { return this->direction; }
pool<int> Graph_Port::Port::getEdges() { return this->edges; }
std::vector<Signal::Signal> Graph_Port::Port::getInputSignals() { return this->inputSignals; }
std::vector<Signal::Signal> Graph_Port::Port::getOutputSignals() { return this->outputSignals; }
bool Graph_Port::Port::isConnectedToExtern() { return this->connectedToExtern; }

std::vector<Signal::Signal> Graph_Port::Port::getSignals()
{
	std::vector<Signal::Signal> temp;

	temp = VectorSupport::merge( temp, this->inputSignals );
	temp = VectorSupport::merge( temp, this->outputSignals );

	return temp;
}

void Graph_Port::Port::setConnectionToExtern(bool conn) { this->connectedToExtern = conn; }

void Graph_Port::Port::setDirection(PortDirection dir)
{
	if( isValidPortDirection(dir) )
	{
		this->direction = dir;
	}
}

bool Graph_Port::Port::isValidPortDirection(PortDirection dir)
{
	if( dir<-1 || dir > 2)
	{
		return false;
	}
	return true;
}

bool Graph_Port::Port::isInputPort()
{
	if( this->direction == INPUT )
	{
		return true;
	}
	return false;
}

bool Graph_Port::Port::isOutputPort()
{
	if( this->direction == OUTPUT )
		return true;
	return false;
}

bool Graph_Port::Port::isInOutPort()
{
	if( this->direction == INOUT )
	{
		return true;
	}
	return false;
}

bool Graph_Port::Port::isInvalidPort()
{
	if( this->direction == INVALID )
	{
		return true;
	}
	return false;
}

void Graph_Port::Port::pushInputSignal(Signal::Signal sig)
{
	if( !Signal::isIn(this->inputSignals, sig) )
	{
		this->inputSignals.push_back(sig);
	}
}

void Graph_Port::Port::pushOutputSignal(Signal::Signal sig)
{
	if( !Signal::isIn(this->outputSignals, sig) )
	{
		this->outputSignals.push_back(sig);
	}
}

void Graph_Port::Port::pushInOutSignal(Signal::Signal sig)
{
	pushInputSignal(sig);
	pushOutputSignal(sig);
}

void Graph_Port::Port::addEdge(int edgeId)
{
	if( edgeId >= 0 )
	{
		this->edges.insert(edgeId);
	}
}

std::string Graph_Port::Port::toString()
{
	std::stringstream ss;

	ss << "\n---------------------------------\n";

	ss << "<" << this->portName << " - ID#" << this->portId;


	switch( this->direction )
	{
		case INPUT:
			ss << " input";
		break;
		case OUTPUT:
			ss << " output";
		break;
		case INOUT:
			ss << " inout";
		break;
		case INVALID:
			ss << " invalid";
		break;
		default:
			log("Strange behaviour detected in toString() for a Port.\n");
	}

	if(this->connectedToExtern)
	{
		ss << " [extern]";
	}

	ss << ">\n\tEdges: ";
	for( auto edge : this->edges )
	{
		ss << edge << " ";
	}

	ss << "\n\tSignals: ";
	switch( this->direction )
	{
		case INPUT:
			ss << "<inputs>\n";
			if( inputSignals.empty() )
			{
				ss << "\t\t< >";
			}

			for( auto sig : this->inputSignals )
			{
				ss << "\t\t" << sig.toString() << "  ";
			}
		break;
		case OUTPUT:
			ss << "<outputs>\n";
			if( outputSignals.empty() )
			{
				ss << "\t\t< >";
			}
			for( auto sig : this->outputSignals )
			{
				ss << "\t\t" << sig.toString() << "  ";
			}
		break;
		case INOUT:
			ss << "<inputs>\n";
			if( inputSignals.empty() )
			{
				ss << "\t\t< >";
			}
			for( auto sig : this->inputSignals )
			{
				ss << "\t\t" << sig.toString() << "  ";
			}
			
			ss << "\n";
			ss << "<outputs>\n";
			if( outputSignals.empty() )
			{
				ss << "\t\t< >";
			}
			for( auto sig : this->outputSignals )
			{
				ss << sig.toString() << "  ";
			}
		break;
		case INVALID:
			ss << " invalid";
		break;
		default:
			log("Strange behaviour detected in toString() for a Port.\n");
	}

	ss << "\n---------------------------------\n";

	return ss.str();
}