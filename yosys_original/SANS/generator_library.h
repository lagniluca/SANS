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

#ifndef GENERATOR_H
#define GENERATOR_H

#include "supportGraph.h"
#include "circuit.h"

//To elaborate RTLIL::Design
#include "kernel/yosys.h"
#include "backends/ilang/ilang_backend.h"

#define CHECK_NUMBER 7

#define NONE 'N'
#define SELECT 'S'
#define DATA 'D'
#define BOTH 'B'
#define ERROR_CODE 'E'

namespace Generator
{

	/*
	 *	Decides what letter the support structure shall contain for each bit of each signal.
	 *	NB: downgrades are not feasible. A signal which has been recognized as SELECT somewhere will forever remain at least a SELECT signal, that's
	 *	why there are some 'return currentCode'.
	 */
	char nextLetter( char currentCode, std::pair<bool, bool> data_select );

	/*	
	 *	Now this is tricky. What we'll output is a ConfiguratorGraph (cg), but why we need
	 *	another class and why the hell is inheriting from Graph? Since SubCircuit_v2::Graph has
	 *	a custom made list of nodes and edges, from which we build the graph, we exploit
	 *	such variables to do that. For the inherinting part, those parameters are protected so
	 *	we can't access them unless we recompile the whole Yosys project by modifying that.
	 *	Since such action is condamned by who's writing, I've performed a bypass to access
	 *	such parameters even if they're protected.
	 */
	struct Generator : SubCircuit_v2::Graph
	{
		private:
            std::string moduleName ;
            //Internal structure of a single module (the one we are looking for)
			SupportGraph::SupportGraph sg;
            //vector of external signals of the module
			std::vector<Signal::Signal> externalSignals;
			//The two above are carefully constructed subsets of the one above.
			std::vector<Signal::Signal> enteringSignals; //Vector of signals entering the module
			std::vector<Signal::Signal> exitingSignals;  //Vector of signals exiting the module
			std::vector<Signal::Signal> inoutSignals; //Vector of inout signals for the module
			//Support structure to fill the exitingSignals even in case of signal renaming. Renaming follows a direction <newName <- oldName> in the std::pair
			std::vector< std::pair<std::string, std::string> > coupledSignals;
            //Vector that contains all the infos related to coupled signals
            std::vector< std::pair<Signal::Signal,Signal::Signal>> pairedSignals;
            //Set of modules that are instantiated inside the current module (used to state if a module is atomic or not)
            std::vector<std::pair<std::string, std::string>> innerModules;

			/*
			 *	Support for bit condensing. NB: to ease the procedures involving this structure, here signals are contained in a [LSB:MSB] fashion.
			 *	This is done to avoid strange measures for accessing the vector of the signal
			 */
			std::vector< std::pair< Signal::Signal, std::vector<char> > > sliceStructure;

			//=================================================================================================================================== GRAPH_BEGIN

			/*
			 *	Removes unneded parts of a particular string (used in cleanseConnection)
			 */
			bool isConnection(std::string str);

			/*
			 *	Functions that corrects the content of externalSignals and co. NB: renaming has a direction
			 */
			void fillSignalStructure();
			
			//Method used for automatically extract inner modules from the internal structure
            void extractInnerModules();
            
            /*
             * Method used for extracting the connections between internal cells outputs and the module's outputs
             */
            std::pair<std::vector<Signal::Signal>, std::vector<Signal::Signal>> extractModuleOutputBinding(RTLIL::Module *module);
            
            /*
             * Method used for knowing if a specific cell's output has some bindings with the module's outputs
             * (i.e. this method as to be called only in case of output/inout cells, NOT in case of input ones ,
             * since this method only exploits output connections between a cell and a module)
             */
            void setCellOutputModuleOutputBindings(  RTLIL::Cell *cell, //RTLIL::Cell that we want to inspect
                                                     Graph_Port::Port *newPort, //Port that we have created for our purposes
                                                     std::pair<std::vector<Signal::Signal>, std::vector<Signal::Signal>> moduleOutputBindings, //Binding of the module with the extern
                                                     bool inout //Boolean flag used to decree if the port is an inout port (true -> inout port, false->output port)
                                                    );

            /*
             * Method used for determine if a specific port is an InOut port and which signals 
             * are connected to that port in such case
             */
            std::pair<bool, std::vector<Signal::Signal>> extractInoutPorts(RTLIL::SigSpec conn);


			/*
			 *	This method generates the final graph by taking the default subcircuit graph and the design.
			 */
			bool buildGraph(SubCircuit_v2::Graph& myg, RTLIL::Module *module);

			/*
			 *	Functions that builds the final graph and then s it.
			 */
			bool buildConsistentGraph(SubCircuit_v2::Graph& myg, RTLIL::Module *module, bool verbose=false);
			
            /*
			 *	Returns the structure we need to perform bit condensing.
			 */
			void prepareSlices();

			//=================================================================================================================================== GRAPH_END

		protected:

			//=================================================================================================================================== CHECK_BEGIN

			/*
			 *	Check function #1 : check no port has direction INVALID_PORT.
			 */
			int CHECK_portIsValid(bool verbose=false);

			/*
			 *	Check function #2 : check portId validity
			 */
			int CHECK_portIdIsValid(bool verbose=false);

			/*
			 *	Check function #3 : port connected to extern must be outputs or inouts.
			 */
			int CHECK_portConnectionToExtern(bool verbose=false);

			/*
			 *	Check function #4 : check that nodes were counted correctly.
			 */
			int CHECK_correctNumOfNodes(bool verbose=false);

			/*
			 *	Check function #5 : checking that inputs are not set on output ports and viceversa.
			 */
			int CHECK_consistentSignalVectors(bool verbose=false);

			/*
			 *	Check function #6 : method to check is results of startingNodes and endingNodes matches with Receiver and Emitter nodes.
			 */
			int CHECK_SE_match_RE(bool verbose=false);

			/*
			 *	Check function #7 : check slice structure's consistency
			 */
			int CHECK_StructureSupport(bool verbose=false);

			/*
			 *	After we build the graph, we need to check everything was done correctly. Each check is done separately,
			 *	do them together may result in a mappazzone.
			 *	NB: if a new check is added, you must add +1 to the CHECK_NUMBER constant, otherwise nothing will work. 
			 */
			bool check(bool verbose=false);

			//=================================================================================================================================== CHECK_END


		public:
			//Constructor
			Generator(SubCircuit_v2::Graph& myg, RTLIL::Module *module, bool verbose=false);
			
			/*
			 *	Given a signal, it returns all the slices sig[x:y] which appear as inputs in some ports.
			 */
			std::vector<Signal::Signal> findSignalFriends(Signal::Signal sig, bool includeSelf);

			/*
			 *	Method that, given the name of a signal, returns a list of nodes where said signal enters as a pure signal (meaning without any modification whatsoever).
			 *	Es: \CLK usually enters in the clock port without modifications. If the clock is modified at the start, maybe by doubling its period, then this component
			 *	will be the only one where \CLK enters purely. If we're discriminating, starting nodes won't include nodes where the signal enters as a selection.
			 *	Signals are considered both in name and dimension.
			 */
			std::vector<Graph_Node::Node *> startingNodes_fullSignal( Signal::Signal externalSignal, bool discriminating, bool verbose=false);

			/*
			 *	Method that, given the name of a signal, returns a list of nodes where said signal enters as a pure signal (meaning without any modification whatsoever).
			 *	Es: \CLK usually enters in the clock port without modifications. If the clock is modified at the start, maybe by doubling its period, then this component
			 *	will be the only one where \CLK enters purely. If we're discriminating, starting nodes won't include nodes where the signal enters as a selection.
			 *	Signals are considered only in name.
			 */
			std::vector<Graph_Node::Node *> startingNodes_byNameOnly( Signal::Signal externalSignal, bool discriminating, bool verbose=false );
            

			/*
			 *	Find a correspondant among the coupledSignals. If getNew, we search for a forward renaming y such that x -> y, otherwise x <- y
			 *	<newName , oldName>
			 */
			std::string getRenaming(std::string signalName, bool getNew=true);

			/*
			 *	Is the signal passed an external signal
			 */
			bool isExternalSignal_byNameOnly(Signal::Signal signal);
			
			/*
			 *	Returns the list of nodes which receives as inputs signals coming from outside the current module.
			 */
			std::vector<Graph_Node::Node *> signalReceivers(bool discriminating);

			/*
			 *	Returns the list of nodes which outputs a signal exiting the current module.
			 */
			std::vector<Graph_Node::Node *> signalEmitters();

			/*
			 *	Is the node a receiver?
			 */
			bool isReceiver(Graph_Node::Node *node);

			/*
			 *	Method to transform the support structure in a string.
			 *	std::vector< std::pair< Signal::Signal, std::vector<char> > >
			 */
			std::string condenseBit();
			
			/*
             * Method used for knowing if a module contains other modules
             */
            bool isAtomic();

			//Getters
			std::string getModuleName();
			SupportGraph::SupportGraph getSupportGraph();
			std::vector<Signal::Signal> getEnteringSignals();
			std::vector<Signal::Signal> getExitingSignals();
			std::vector<Signal::Signal> getExternalSignals();
			std::vector<std::pair< Signal::Signal, std::vector<char> > > getSliceStructure();
			std::vector<std::pair<std::string, std::string> > getCoupledSignals();
			std::vector<std::pair<Signal::Signal, Signal::Signal>> getPairedSignals();
			std::vector<std::pair<std::string,std::string>> getInnerModules();
			std::vector<Signal::Signal> getInoutSignals();
            
            /*
             * Method used for instantiating the slice structure of the current module
             */
			void setSliceStructure( std::vector< std::pair< Signal::Signal, std::vector<char> > > structure );

			//Classic toString method
            std::string toString();
	};
}

#endif