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

#ifndef PORT_LIBRARY_H
#define PORT_LIBRARY_H

#include "signal_library.h"
#include "stdvector_support.h"

USING_YOSYS_NAMESPACE

namespace Graph_Port
{
	/*
	 *	Enumeration that represents the direction of a port.
	 */
	enum PortDirection
	{
		INVALID = -1,
		INPUT = 0,
		OUTPUT = 1,
		INOUT = 2
	};

	/*
	 *	Structure representing one port of a component.
	 */
	class Port
	{
		private:
			//Name of the port. Unique wrt the node it's contained in.
			std::string portName;
			//Unique id of the port wrt the node it's contained in.
			int portId;
			//Direction of the port.
			PortDirection direction;
			//Edges connected to this port.
			pool<int> edges;
			//Signals connected as inputs, empty if output port.
			std::vector<Signal::Signal> inputSignals;
			//Signals connected as outputs, empty if input port.
			std::vector<Signal::Signal> outputSignals;
			//Connected to extern
			bool connectedToExtern;	//MUST BE FALSE FOR INPUT PORTS! THIS HAS MEANING ONLY FOR OUTPUTS OR INOUTS
			
		public:
			//Constructors
			Port();
			Port(std::string portName);
			Port(std::string portName, int portId);
			Port(std::string portName, PortDirection portDirection);
			Port(std::string portName, PortDirection portDirection, int portId);

			//Getters
			std::string getPortName();
			int getPortId();
			PortDirection getPortDirection();
			pool<int> getEdges();
			std::vector<Signal::Signal> getInputSignals();
			std::vector<Signal::Signal> getOutputSignals();
			bool isConnectedToExtern();

			/*
			 *	Getter for the whole bunch of connected signals.
			 */
			std::vector<Signal::Signal> getSignals();

			//Setters
			void setConnectionToExtern(bool conn);

			/*
			 *	Set the direction of the port.
			 */
			void setDirection(PortDirection dir);

			/*
			 *	Check whether a direction is valid.
			 */
			bool isValidPortDirection(PortDirection dir);

			/*
			 *	Method to know if said port is an input port.
			 */
			bool isInputPort();

			/*
			 *	Method to know if said port is an output port.
			 */
			bool isOutputPort();

			/*
			 *	Method to know if said port is an inout port.
			 */
			bool isInOutPort();

			/*
			 *	Method to know if said port is an invalid port.
			 */
			bool isInvalidPort();

			/*
			 *	Safely push a signal in the signal vector
			 */
			void pushInputSignal(Signal::Signal sig);

			/*
			 *	Safely push a signal in the signal vector
			 */
			void pushOutputSignal(Signal::Signal sig);

			/*
			 *	Safely push a signal in the signal vector
			 */
			void pushInOutSignal(Signal::Signal sig);

			/*
			 *	Push an edge in the pool.
			 */
			void addEdge(int edgeId);
			
			/*
			 *	toString() interface.
			 */
			std::string toString();
	};
}

#endif