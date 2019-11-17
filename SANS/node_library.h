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

#ifndef NODE_LIBRARY_H
#define NODE_LIBRARY_H

#include "port_library.h"
#include "stdvector_support.h"

USING_YOSYS_NAMESPACE

namespace Graph_Node
{
	/*
	 *	Structure representing one component (one node in the graph)
	 */
	class Node
	{
		private:
			//Unique ID of the node
			int nodeId;
			//Unique name of the node
			std::string name;
			//Type of the node, needed for node recognition
			std::string type;

			//Boolean flag used to decree if a node is a placeholder (i.e. the real node , with its internal structure, is defined elsewhere)
			bool modulePlaceholder = false ;

			//Port lists
			std::vector<Graph_Port::Port *> inputPorts;
			std::vector<Graph_Port::Port *> outputPorts;
			std::vector<Graph_Port::Port *> inoutPorts;
            std::vector<Graph_Port::Port *> moduleDataPorts;
            std::vector<Graph_Port::Port *> moduleSelectionPorts;

		public:
			//Constructor
			Node(int nodeId, std::string nodeName, std::string nodeType, bool modulePlaceholder = false);
			
			//Getters
			int getId();
			std::string getName();
			std::string getType();
			/*
			 *	The node is a signal emitter if at least one of its outputs (output/inout ports) is connected to extern
			 */
			bool isEmitter();

			/*
			 * Method used to state if a node is a module placeholder (i.e. the cell is not an atomic cell but must be considered as a blackbox)
			 */
			bool isModulePlaceholder();
			
			/*
			 *	This method is the one suggested when searching for input ports since it considers the inout possibility too.
			 */
			std::vector<Graph_Port::Port *> getInputs();

			/*
			 *	This method is the one suggsted when searching for output ports since it considers the inout possibility too.
			 */
			std::vector<Graph_Port::Port *> getOutputs();

			/*
			 *	This method returns all the ports. Used solely for debug purpose.
			 */
			std::vector<Graph_Port::Port *> getPorts();

			/*
			 *	Get port form its id.
			 */
			Graph_Port::Port* getPort(int id);

			/*
			 *	Adds a port to the ports' vector. The return value is used for debugging purposes.
			 */
			int addPort(Graph_Port::Port *newPort, Graph_Port::PortDirection direction);
            
            /*
             * Module used for setting a module port.
             * It is easy to see that we have not included the direction since this is applied to anly input or inout
             * ports
             */
            bool setModulePort(Graph_Port::Port *newPort,bool data,bool selection);

			/*
			 *	Get all the input signals of the port.
			 */
			std::vector<Signal::Signal> getInputSignals();

			/*
			 *	Get all the output signals of the port.
			 */
			std::vector<Signal::Signal> getOutputSignals();
            
            /**
             * In case of a module_placeholder, we used this method for 
             * getting the selection ports
             */
            std::vector<Graph_Port::Port *> getModuleSelectionPorts();
            
             /**
             * In case of a module_placeholder, we used this method for 
             * getting the data ports
             */
            std::vector<Graph_Port::Port *> getModuleDataPorts();
            
             /**
             * In case of a module_placeholder, we used this method for 
             * getting the selection and data ports
             */
            std::vector<Graph_Port::Port *> getModuleDataSelectionPorts();

			/*
			 *	toString interface.
			 */
			std::string toString();
	};
}

#endif
