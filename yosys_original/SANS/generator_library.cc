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

#include <string>
#include <iostream>

#include "stdvector_support.h"
#include "stdstring_support.h"
#include "module_to_graph.h"
#include "rtlil_dumper.h"
#include "generator_library.h"

USING_YOSYS_NAMESPACE

/*
 *	Decides what letter the support structure shall contain for each bit of each signal.
 *	NB: downgrades are not feasible. A signal which has been recognized as SELECT somewhere will forever remain at least a SELECT signal, that's
 *	why there are some 'return currentCode'.
 */
char Generator::nextLetter( char currentCode, std::pair<bool, bool> data_select )
{
	switch(currentCode)
	{
		case NONE:
			if( data_select.first && data_select.second)
			{
				return BOTH;
			}
			else if(data_select.first)
			{
				return DATA;
			}
			else if(data_select.second)
			{
				return SELECT;
			}
			else
			{
				return NONE;
			}
		break;

		case SELECT:
			if(data_select.first)
			{
				return BOTH;
			}
			else
			{
				return currentCode;
			}
		break;

		case DATA:
			if(data_select.second)
			{
				return BOTH;
			}
			else
			{
				return currentCode;
			}
		break;

		case BOTH:
			return currentCode;
		break;

		default:
			std::cout <<"Unrecognized letter code in nextLetter :" << currentCode << "\n";
			return ERROR_CODE;
	}
}

		/*
		 *	Removes unneded parts of a particular string (used in cleanseConnection)
		 */
		bool Generator::Generator::isConnection(std::string str)
		{
			std::string support;

			for(char& c : str)
			{
				support.push_back(c);

				if(support == "wire" || support == "attribute" || support == "parameter" || support == "module")
				{
					return false;
				}
			}

			return true;
		}

		/*
		 *	Functions that corrects the content of externalSignals and co. NB: renaming has a direction
		 */
		void Generator::Generator::fillSignalStructure()
		{
			/*	From external all is OK.
			 *	Entering:	- unconnected (show in results)
			 *				- enter renaming : for( inputs, if there's a rename, put it in, the old value)
			 *	Exiting:	- for( outputs, if there0s a rename, put it (the new value))
			 *
			 *	<newName <- oldName>
			 *	std::string getRenaming(std::string signalName, bool getNew=true)
			 */
			for( auto node : sg.getNodes() )
			{
				for( auto inputsignal : node->getInputSignals() )
				{
					//Returns oldName
					std::string temp = getRenaming( inputsignal.getSignalName(), false );

					Signal::Signal sig = fetchSignal(this->externalSignals, temp);
					
					if( !VectorSupport::isIn(enteringSignals, sig) && !temp.empty() && sig.getSignalName()!="Empty")
					{
						//std::cout << "ENTERING " << sig.toString() << "\n";
						this->enteringSignals.push_back(sig);
					}
				}

				for( auto outputsignal : node->getOutputSignals() )
				{
					//Returns newName
					std::string temp = getRenaming( outputsignal.getSignalName(), true );

					Signal::Signal sig = fetchSignal(this->externalSignals, temp);

					if( !VectorSupport::isIn(exitingSignals, sig) && !temp.empty() && sig.getSignalName()!="Empty" )
					{
						//std::cout << "EXITING " << sig.toString() << "\n";
						this->exitingSignals.push_back(sig);
					}
				}
			}

			//std::cout << StringSupport::DUMP_vector(coupledSignals);

			for( auto pair : this->coupledSignals )
			{
				Signal::Signal newsig = Signal::fetchSignal(exitingSignals, pair.first);
				Signal::Signal oldsig = Signal::fetchSignal(enteringSignals, pair.second);

				if( oldsig.getSignalName()=="Empty"	&& newsig.getSignalName()=="Empty" )
				{
					Signal::Signal oldsigExt = Signal::fetchSignal(externalSignals, pair.second);
					Signal::Signal newsigExt = Signal::fetchSignal(externalSignals, pair.first);

					if( oldsigExt.getSignalName()!="Empty" && newsigExt.getSignalName()!="Empty" )
					{
						this->enteringSignals.push_back(oldsigExt);
						this->exitingSignals.push_back(newsigExt);
					}
				}
			}
		}
		
		//Method used for automatically extract inner modules from the internal structure
        void Generator::Generator::extractInnerModules()
        {
            bool found = false ;
            
            //Looking inside the current module's nodes to see if we have any module placeholder
            for(auto node : sg.getNodes())
            {
                found = false ;
                
                //Case of module placeholder found
                if(node->isModulePlaceholder())
                {
                    //Looking if we have already inserted the analyzed module inside the list of internal modules
                    for(auto imp : this->innerModules)
                    {
                        if(imp.first == node->getName())
                        {
                            found = true;
                        }

                        if(found) break;
                    }
                    
                    //In case of fresh new inner module found , we add it to our inner module's list
                    if(!found)
                    {
                        this->innerModules.push_back(std::make_pair(node->getName(), node->getType()));
                    }
                }
            }
        }
        
        /*
         * Method used for extracting the connections between internal cells outputs and the module's outputs
         */
        std::pair<std::vector<Signal::Signal>, std::vector<Signal::Signal>> Generator::Generator::extractModuleOutputBinding(RTLIL::Module *module)
        {
            std::pair<std::vector<Signal::Signal>, std::vector<Signal::Signal>> moduleOutputBindings ;
            std::vector<Signal::Signal> internalCellsOutputs; 
            std::vector<Signal::Signal> moduleOutputs;        
            
            moduleOutputBindings = std::make_pair(internalCellsOutputs, moduleOutputs);
            
            for(auto it = module->connections().begin(); it != module->connections().end(); it++)
            {
                //pair that contains all the signals associated to a module outputs and internal cells outputs connected to the module's ones
                std::pair<std::vector<Signal::Signal>, std::vector<Signal::Signal>> tmp = RTLIL_DUMPER::connDumper(it->first, it->second);
                
                //Signals associated to module's (internal) cells
                for(auto internalCellOutput : tmp.first)
                {
                    moduleOutputBindings.first.push_back(internalCellOutput);
                    
                }
                
                //Signals associated to module's output signals
                for(auto moduleOutput : tmp.second)
                {
                    moduleOutputBindings.second.push_back(moduleOutput);
                    
                }
            }
            
            return moduleOutputBindings;
        }
        
        /*
         * Method used for knowing if a specific cell's output has some bindings with the module's outputs
         * (i.e. this method as to be called only in case of output/inout cells, NOT in case of input ones ,
         * since this method only exploits output connections between a cell and a module)
         */
        void Generator::Generator::setCellOutputModuleOutputBindings(  RTLIL::Cell *cell, //RTLIL::Cell that we want to inspect
                                                 Graph_Port::Port *newPort, //Port that we have created for our purposes
                                                 std::pair<std::vector<Signal::Signal>, std::vector<Signal::Signal>> moduleOutputBindings, //Binding of the module with the extern
                                                 bool inout //Boolean flag used to decree if the port is an inout port (true -> inout port, false->output port)
                                                )
        {
            //Here we are checking if the inout cell has some bindings with the current module's outputs
            for(auto &it : cell->connections())
            {
                //Extracting the connections of the cell
                std::pair<bool, std::vector<Signal::Signal>> tmp = RTLIL_DUMPER::sigSpecDumper(it.second);
                
                //Looking if the connection extraction has been successfully performed
                if(tmp.first)
                {
                    //In case of successfull extraction, we cycle through the cell's signals
                    for(auto sig : tmp.second)
                    {
                        //Cycling throu the module output bindings                                
                        for(auto mob : moduleOutputBindings.second)
                        {
                            //Looking if the signal associated to the cell is the same as the one coming out the module
                            if(sig == mob)
                            {
                                //Case of inout port
                                if(inout)
                                {
                                   newPort->pushInOutSignal(sig);
                                   newPort->setConnectionToExtern(true); 
                                }
                                //case of output port
                                else
                                {
                                    newPort->pushOutputSignal(sig);
                                    //newPort->setConnectionToExtern(true);
                                }  
                            }
                        }
                    }
                }
            }
        }
        
        /*
         * Method used for determine if a specific port is an InOut port and which signals 
         * are connected to that port in such case
         */
        std::pair<bool, std::vector<Signal::Signal>> Generator::Generator::extractInoutPorts(RTLIL::SigSpec conn)
        {
            std::pair<bool, std::vector<Signal::Signal>> result;
            bool inoutPort = false;
            std::vector<Signal::Signal> signals;
            
            std::pair<bool, std::vector<Signal::Signal>> conny = RTLIL_DUMPER::sigSpecDumper(conn);
                                        
            for(auto sig : conny.second)
            {                        
                for(auto exSig : inoutSignals)
                {
                    if(sig.getSignalName().compare(exSig.getSignalName()) == 0)
                    {
                        inoutPort = true;
                        signals.push_back(exSig);
                	}
                }
            }
            
            result = std::make_pair(inoutPort, signals);
            
            return result;
        }


		/*
		 *	This method generates the final graph by taking the default subcircuit graph and the design.
		 */
		bool Generator::Generator::buildGraph(SubCircuit_v2::Graph& myg, RTLIL::Module *module)
		{
            this->moduleName = module->name.c_str();
			std::vector< std::pair<std::string, std::string> > couples;

			std::vector<std::string> design_modules;

			//Extracting the names of the modules of our design
			for(auto mod : module->design->modules())
			{
				design_modules.push_back(mod->name.str());
			}

			//Evaluate the module for signal renaming
            /*
             * Here we are extraction the coupled signals of the module, i.e. the signals that 
             * are part of a connection for the module itself
             */
            for(auto conn: module->connections())
            {
                std::pair<std::vector<Signal::Signal>, std::vector<Signal::Signal>> result = RTLIL_DUMPER::connDumper(conn.first, conn.second);
                
                for(auto fSig : result.first)
                {
                    for(auto sSig : result.second)
                    {
                        coupledSignals.push_back(std::make_pair(fSig.getSignalName(), sSig.getSignalName()));
                        pairedSignals.push_back(std::make_pair(fSig, sSig));
                    }
                }
                
                
            }
            
            //Here we extract the memories of the module
            std::vector<std::string> memories ;
            for (auto it : module->memories)
            {
                //std::cout << "memory: " << it.second->name.c_str() << std::endl;
                memories.push_back(it.second->name.c_str());
            }
            

			for( auto spec : ModToGraph::mod2graph(myg, module) )
			{
				Signal::Signal temp ( spec.first, spec.second-1, 0 ); 
				externalSignals = VectorSupport::conditionalInsert( externalSignals, temp );

				/*
				 * Here we are discriminating the external signals of the module and collecting them 
                 * in their associated lists 
				 */
				for(auto wire : module->wires())
				{
					if(wire->name.str().compare(temp.getSignalName()) == 0)
					{
                        //case of inout signal
						if((wire->port_input) && (wire->port_output))
						{
							enteringSignals = conditionalInsert_checkSignalName(enteringSignals, temp);
							exitingSignals = conditionalInsert_checkSignalName(exitingSignals, temp);
                            inoutSignals = conditionalInsert_checkSignalName(inoutSignals, temp);
						}
						
						//Case of input signal
						if((wire->port_input) && (!wire->port_output))
						{
							enteringSignals = conditionalInsert_checkSignalName(enteringSignals, temp);
						}

						//Case of output signal
						if((!wire->port_input) && (wire->port_output))
						{
							exitingSignals = conditionalInsert_checkSignalName(exitingSignals, temp);
						}	
						
					}
				}
			}
			
			/*
             * Extracting the bindings between internal components of a module and the output signals of
             * the module itself
             */
			std::pair<std::vector<Signal::Signal>, std::vector<Signal::Signal>> moduleOutputBindings ;
            moduleOutputBindings = this->extractModuleOutputBinding(module);

			//Then, we run on all the nodes of the DESIGN (condensed in the graph)
			for( auto subcircuitNode : ((Generator&)myg).nodes )
			{
				//Temporary variable to store the nodeId that we're going to find
				int subCircuitId = -1;
				//Count is used as a check, if we find more than one match, the id is not unique (and we have a problem)
				int count = 0;
				
				for( auto nodeOfMap : ((Generator&)myg).nodeMap )
				{	//subcircuitNode.nodeId is the future node name, this is unfortunate name overlapping
					if( subcircuitNode.nodeId ==  nodeOfMap.first )
					{
						subCircuitId = nodeOfMap.second;
						count++;
					}
				}

				//Here we are checking if the cell is a module placeholder (i.e. a node that represents another module)
				bool isModule = false;
				for(auto mod : design_modules)
				{
					if(mod.compare(subcircuitNode.typeId) == 0) 
					{
						isModule = true;
					}
						
				}

				//Just copy what we need from the existent graph.
				Graph_Node::Node *newNode = new Graph_Node::Node(subCircuitId, subcircuitNode.nodeId, subcircuitNode.typeId, isModule);

					//Do NOT remove this.
					SigMap sigmap(module);

					for( auto cell : module->cells() )
					{
						//Join done.
						if( subcircuitNode.nodeId == cell->name.str() )
						{
							//Then, foreach port of said node, we extract the info needed.
							for( auto subcircuitPort : subcircuitNode.ports )
							{
								//We perform another join to match the port with its connection.
								for( auto conn : cell->connections() )
								{
									if( subcircuitPort.portId == conn.first.c_str() )
									{
										/*	DO NOT DELETE THIS
											std::cout << "NodeId " << subCircuitId << ", PortId " << subcircuitNode.portMap[subcircuitPort.portId] << "\n";
											std::cout << "Node " << subcircuitNode.nodeId << ", PortName " << subcircuitPort.portId << "\n\n";
										//*/
                                            
                                        std::pair<bool, std::vector<Signal::Signal>> inOut = this->extractInoutPorts(conn.second);
                                        bool isInOut = inOut.first;

										Graph_Port::Port *newPort = new Graph_Port::Port(subcircuitPort.portId, subcircuitNode.portMap[subcircuitPort.portId]);
                                        
                                        //Here we check if the cell is connected to some of the module's memories
                                        bool trovato = false;
                                        for (auto &it : cell->connections()) 
                                        {
                                               std::pair<bool, std::vector<Signal::Signal>> res = RTLIL_DUMPER::sigSpecDumper(it.second);
                                               for(auto sig : res.second)
                                               {
                                                   for(auto memory : memories)
                                                   {
                                                       std::size_t found = sig.getSignalName().find(memory);
                                                       
                                                       if(found != std::string::npos)
                                                       {
                                                           trovato = true;
                                                           //std::cout << "cell conn: " << sig.toString() << " ";
                                                           //std::cout << "memory: " << memory << " connection found" << std::endl;
                                                       }
                                                   }
                                                   
                                                        
                                               }
                                               
                                        }
                                        
                                        /*******Cases of a-priori known cell's ports direction ********/
                                        
										if ((cell->input(conn.first) && cell->output(conn.first)) || isInOut)
										{
											//2: INOUT
											newPort->setDirection(Graph_Port::PortDirection::INOUT);
                                            
                                            this->setCellOutputModuleOutputBindings(cell, newPort, moduleOutputBindings, true);
                                            
                                            for(auto inoutSignal : inOut.second)
                                            {
                                                newPort->pushInOutSignal(inoutSignal);
                                            }
										}
										else if (cell->input(conn.first) && !cell->output(conn.first))
										{
											//0: INPUT
											newPort->setDirection(Graph_Port::PortDirection::INPUT);
										}
										else if (!cell->input(conn.first) && cell->output(conn.first) )
										{
											//1: OUTPUT
											newPort->setDirection(Graph_Port::PortDirection::OUTPUT);
                                            
                                            this->setCellOutputModuleOutputBindings(cell, newPort, moduleOutputBindings, false);
										}
										else
										{
                                            /**
                                             * *******Cases of a-priori unknown cell's ports direction ********
                                             * 
                                             * We reach this branch in case of (bad) design choiches in which we don't have
                                             * a specific bindings between the signals coming from the current module and 
                                             * the signals that goes in the inner module (so we cannot know which type of
                                             *port we are working on)
                                             */ 
                                            if((isModule) && (RTLIL_DUMPER::isValidPortID(subcircuitPort.portId)))
                                            {
                                                //The cell is a module placeholder and it's (unnamed) port connection is valid 
                                                
                                                //portValue = number associated to the inspecetd port (since it is not named but only numerated)
                                                int portValue = RTLIL_DUMPER::portIDDumper(subcircuitPort.portId).second;
                                                bool found = false;
                                                
                                                //Looking inside other application modules to check if we see the module of which our cell is the placeholder
                                                for(auto innerModule : module->design->modules())
                                                {
                                                    if(innerModule->name.str() == subcircuitNode.typeId)
                                                    {
                                                        //In case of match found, we have to exploit the module wires (external connections) in order to retrive the port names
                                                        for(auto imWire : innerModule->wires())
                                                        {
                                                            //Looking if the wire (external connection) id is the same of our port 
                                                            if(imWire->port_id == portValue)
                                                            {
                                                               
                                                               newPort = new Graph_Port::Port(imWire->name.c_str(),
                                                                                              subcircuitNode.portMap[subcircuitPort.portId]
                                                                                             );
                                                               
                                                               //INOUT cell
                                                               if((imWire->port_input) && (imWire->port_output))
                                                               {
                                                                   newPort->setDirection(Graph_Port::PortDirection::INOUT);
                                                                   
                                                                   this->setCellOutputModuleOutputBindings(cell, newPort, moduleOutputBindings, true);  
                                                                   
                                                                    for(auto inoutSignal : inOut.second)
                                                                    {
                                                                        newPort->pushInOutSignal(inoutSignal);
                                                                    }
                                                               }
                                                               
                                                               //INPUT cell
                                                               if((imWire->port_input) && (!imWire->port_output))
                                                               {
                                                                    newPort->setDirection(Graph_Port::PortDirection::INPUT);
                                                               }
                                                               
                                                               //OUTPUT cell
                                                               if((!imWire->port_input) && (imWire->port_output))
                                                               {
                                                                    newPort->setDirection(Graph_Port::PortDirection::OUTPUT);
                                                                    
                                                                    this->setCellOutputModuleOutputBindings(cell, newPort, moduleOutputBindings, false);
                                                               }
                                                               
                                                               found = true;
                                                            }
                                                            
                                                            if(found)
                                                            {
                                                                break;
                                                            }
                                                        }
                                                    }
                                                    
                                                    if(found)
                                                    {
                                                        break;
                                                    }
                                                }
                                                
                                            }
                                            else
                                            {
                                                //Branch reached when we miss a module in the yosy call or a port binding is not valid
                                                std::stringstream ss ;
                                            
                                                ss << "\n\n************ ERROR: Module Declaration **********\n" ;
                                                ss << "[Problem> {" << subcircuitNode.typeId << "} module declaration not provided or invalid port binding in .sv file, cannot preoceed\n\n" ;

                                                std :: cout << ss.str() << std::endl;
                                                return false;
                                            }
                                           
										}

										//Each edge is a single bit signal.
										for( auto bit : subcircuitPort.bits )
										{
											newPort->addEdge( bit.edgeIdx);
										}

										//Retrieve the connection between ports and externalSignals.
										std::string logSig = log_signal( sigmap(conn.second) );
										std::vector< std::pair< std::string, std::pair<int,int> > > filtered;
										//Extraction of all the connections of a port (both signals and other ports)
										std::pair<bool, std::vector<Signal::Signal>> dumpedSignals = RTLIL_DUMPER::sigSpecDumper(sigmap(conn.second));

										//Extract the specification from the signal logSig used for discriminating among signals and ports I/Os
										for( auto sigSpecs : StringSupport::extractSignals(logSig) )
										{
											//Push back to the vector all the needed info to build the signal later
											filtered.push_back( std::make_pair(sigSpecs.first, std::make_pair(sigSpecs.second.second, sigSpecs.second.first) ) );
										}

										//Vector that contains only Signals connected to the port
										std::vector<Signal::Signal> realSignals ;

										//Adding only real port signals to our port
										for(auto f : filtered)
										{
											for(auto ds : dumpedSignals.second)
											{
												//If the dumped signal name corresponds to a real signal (not another port input/output)...
												if(f.first == ds.getSignalName()) 
												{
													realSignals = VectorSupport::conditionalInsert(realSignals, ds);
												}
											}
										}

										//I trust the assignment done previously in this function, so I use them to assign the port to the rightful queue
										switch(newPort->getPortDirection())
										{
											case Graph_Port::PortDirection::INPUT:

												for(auto rs : realSignals)
												{
													newPort->pushInputSignal(rs);
												}
												newNode->addPort(newPort, Graph_Port::PortDirection::INPUT);
                                               
											break;

											case Graph_Port::PortDirection::OUTPUT:
												if(conn.second.is_wire())
												{

													for(auto rs : realSignals)
													{
														newPort->pushOutputSignal(rs);
													}
                                                    
												}
                                                newNode->addPort(newPort, Graph_Port::PortDirection::OUTPUT);	
                                                 if(trovato)
                                                    newPort->setConnectionToExtern(true);
                                                break;

											case Graph_Port::PortDirection::INOUT:
												for(auto rs : realSignals)
												{
													newPort->pushInOutSignal(rs);
												}
												newNode->addPort(newPort, Graph_Port::PortDirection::INOUT);
                                                if(trovato)
                                                    newPort->setConnectionToExtern(true);
												break;

											default:
												return false;
										}	
									}
								}
							}
						}
					}
				sg.addNode(newNode);
			}

			/*
			 *	Correction of enteringSignals. Some outputs of a single module may be used as an input somewhere in the same module (think of SR or JK retro-action).
			 *	Instead, module inputs cannot be used as outputs in the same module.
			 */
            //this->enteringSignals = VectorSupport::minus(enteringSignals, exitingSignals); //Original

			/*
			 *	Correction for signal renaming. Because some signals are renamed before the output, we must sign the port and not the signal as "extern". To avoid
			 *	adding a boolean, we connect Signal and Port, then marking as extern all the signals
			 */
			for( auto externPair : /*std::vector<std::pair<int, int>>*/ myg.getAllExtern() )
			{
				/*
				 *	Despite the direction of the port, we store the extern value (we need it for the outputs, why not for the inputs). Moreover, an output
				 *	signal has an unique name, the inputs can be more than one, so it's meaningless to say that a signal that enters a port is "extern". The part
				 *	of entering signals is treated above.
				 */
				Graph_Port::Port *temp = sg.getNode(externPair.first)->getPort(externPair.second);
				if( (temp->getPortDirection() == Graph_Port::PortDirection::OUTPUT) || (temp->getPortDirection() == Graph_Port::PortDirection::INOUT) )
				{
					temp->setConnectionToExtern(true);
				}
			}
			
			//We use this function for extracting the (other) modules that are called inside the current module
			this->extractInnerModules();

			this->fillSignalStructure();
            
			return true;
		}

		/*
		 *	Functions that builds the final graph and then s it.
		 */
		bool Generator::Generator::buildConsistentGraph(SubCircuit_v2::Graph& myg, RTLIL::Module *module, bool verbose)
		{
			//Graph was not generated correctly.
			if(!buildGraph(myg, module))
			{
				log("\tError while building graph...\n\n");
				return false;
			}

			//Graph did not pass consistency tests (more dangerous than the first exit)
			if(!check(verbose))
			{
				log("\tError while checking graph...\n\n");
				return false;
			}

			//this->getCG().buildMatrixes(0,0,0);

			return true;
		}
		
        /*
		 *	Returns the structure we need to perform bit condensing.
		 */
		void Generator::Generator::prepareSlices()
		{
			//Foreach entering signal
			for( auto externalSig : this->enteringSignals )
			{
				std::vector<char> externalSignalWidthSupport;

				//Push a 'false' foreach bit of that signal
				for( int i=0; i<externalSig.width(); i++ )
				{
					externalSignalWidthSupport.push_back(NONE);
				}

				sliceStructure.push_back( std::make_pair(externalSig, externalSignalWidthSupport) );
			}
		}



		/*
		 *	Check function #1 : check no port has direction INVALID_PORT.
		 */
		int Generator::Generator::CHECK_portIsValid(bool verbose)
		{
			if(verbose)
			{
				log("Checking the validity of ports' direction...\n");
			}

			bool ok = true;

			for( auto node: sg.getNodes() )
			{
				for( auto port : node->getPorts() )
				{
					if( port->isInvalidPort() )
					{
						ok = false;
						std::cout << "Invalid input port found:\n" << node->toString() << " in port: " << port->toString() << std::endl;
					}
				}
			}

			if(ok)
			{
				return 1;
			}
			return 0;
		}

		/*
		 *	Check function #2 : check portId validity
		 */
		int Generator::Generator::CHECK_portIdIsValid(bool verbose)
		{
			if(verbose)
			{
				log("Checking the validity of ports' id...\n");
			}

			bool ok = true;

			for( auto node: sg.getNodes() )
			{
				for( auto port : node->getPorts() )
				{
					if( port->getPortId() < 0 )
					{
						ok = false;
					}
				}
			}

			if(ok)
			{
				return 1;
			}
			return 0;
		}

		/*
		 *	Check function #3 : port connected to extern must be outputs or inouts.
		 */
		int Generator::Generator::CHECK_portConnectionToExtern(bool verbose)
		{
			if(verbose)
			{
				log("Checking connections to outer signals...\n");
			}

			bool ok = true;

			for( auto node : sg.getNodes() )
			{
				for( auto port : node->getPorts() )
				{
					if( (port->getPortDirection() == Graph_Port::PortDirection::INPUT) && (port->isConnectedToExtern()) )
					{
						ok = false;
					}
				}
			}
			
			if(ok)
			{
				return 1;
			}
			return 0;
		}

		/*
		 *	Check function #4 : check that nodes were counted correctly.
		 */
		int Generator::Generator::CHECK_correctNumOfNodes(bool verbose)
		{
			if(verbose)
			{
				log("Counting the number of the nodes...\n");
			}

			if( (int)sg.getNodes().size() == sg.getNumOfNodes() )
			{
				return 1;
			}
			return 0;
		}

		/*
		 *	Check function #5 : checking that inputs are not set on output ports and viceversa.
		 */
		int Generator::Generator::CHECK_consistentSignalVectors(bool verbose)
		{
			if(verbose)
			{
				log("Checking consistency of port vectors...\n");
			}

			for( auto node : sg.getNodes() )
			{
				for( auto inputPort : node->getInputs() )
				{
					if( inputPort->getPortDirection() == Graph_Port::PortDirection::INPUT )
					{
						if( (int)inputPort->getOutputSignals().size() > 0 )
						{
							log("Found input port with non-empty output signals.\n");
							return 0;
						}
					}
				}

				for( auto outputPort : node->getOutputs() )
				{
					if( outputPort->getPortDirection() == Graph_Port::PortDirection::OUTPUT )
					{
						if( (int)outputPort->getInputSignals().size() > 0 )
						{
							log("Found output port with non-empty input signals.\n");
							return 0;
						}
					}
				}
			}

			return 1;
		}

		/*
		 *	Check function #6 : method to check is results of startingNodes and endingNodes matches with Receiver and Emitter nodes.
		 */
		int Generator::Generator::CHECK_SE_match_RE(bool verbose)
		{
			if(verbose)
			{
				log("Checking node analysis' methods consistency...\n");
			}

			std::vector<Graph_Node::Node *> starting;
			//std::vector<Graph_Node::Node *> ending;
			std::vector<Graph_Node::Node *> receivers;
			//std::vector<Graph_Node::Node *> emitters;

			for( auto sig : this->enteringSignals )
			{
				starting = VectorSupport::merge( starting, startingNodes_byNameOnly(sig, false) );
			}
			/*for( auto sig : this->exitingSignals )
			{
				ending = VectorSupport::merge( ending, endingNodes(sig) );
			}*/
			receivers = signalReceivers(false);
			//emitters = signalEmitters();

			if( VectorSupport::equals(starting, receivers) /*&& VectorSupport::equals(ending, emitters)*/ )
			{
				return 1;
			}
			return 0;
		}

		/*
		 *	Check function #7 : check slice structure's consistency
		 */
		int Generator::Generator::CHECK_StructureSupport(bool verbose)
		{
			if(verbose)
			{
				log("Checking support structure's consistency...\n");
			}

			for( auto sig : this->enteringSignals )
			{
				for( auto pair : this->sliceStructure )
				{
					if( sig.getSignalName() == pair.first.getSignalName() )
					{
						if( sig.width() != (int)pair.second.size() )
						{
							return 0;
						}
					}
				}
			}
			return 1;
		}

		/*
		 *	After we build the graph, we need to check everything was done correctly. Each check is done separately,
		 *	do them together may result in a mappazzone.
		 *	NB: if a new check is added, you must add +1 to the CHECK_NUMBER constant, otherwise nothing will work. 
		 */
		bool Generator::Generator::check(bool verbose)
		{
			int checkCount = 0;

			checkCount += CHECK_portIsValid(verbose);
			checkCount += CHECK_portIdIsValid(verbose);
			checkCount += CHECK_portConnectionToExtern(verbose);
			checkCount += CHECK_correctNumOfNodes(verbose);
			checkCount += CHECK_consistentSignalVectors(verbose);
			checkCount += CHECK_SE_match_RE(verbose);
			checkCount += CHECK_StructureSupport(verbose);

			if(verbose)
			{
				std::cout << "\nCheck completed succefully: " << checkCount << "/" << CHECK_NUMBER << ".\n" << std::endl;
			}

			if(checkCount==CHECK_NUMBER)
			{
				return true;
			}
			return false;
		}

		//=================================================================================================================================== CHECK_END

		//Constructor
		Generator::Generator::Generator(SubCircuit_v2::Graph& myg, RTLIL::Module *module, bool verbose) : SubCircuit_v2::Graph()
		{
            std::string log_string = "\n\t=========={ Building module: " + module->name.str() + " structure }==========";
            std::cout << log_string << std::endl;
			                
			log("\tBuilding backbone graph ...\n");
			//If either something in the generation or one check fails, nothing can proceed accordingly to our analysis.
			if(!buildConsistentGraph(myg, module, verbose))
			{
				log("Graph inconsistent, terminating execution...\n");
				abort();
			}
			log("\tGraph is consistent.\n");
           
			log("\tGenerating support structures...\n");
            
			this->prepareSlices();
            
			log("\tStructures ready.\n\n");
		}
		
		/*
		 *	Given a signal, it returns all the slices sig[x:y] which appear as inputs in some ports.
		 */
		std::vector<Signal::Signal> Generator::Generator::findSignalFriends(Signal::Signal sig, bool includeSelf)
		{
			std::vector<Signal::Signal> temp;

			if(includeSelf)
			{
				temp.push_back(sig);
			}

			for( auto node : sg.getNodes())
			{
				for( auto inputPort : node->getInputs() )
				{
					for( auto inputSignal : inputPort->getInputSignals() )
					{
						//If the signal name is the same, we need to push it only if the same dimensions are not in the temp already
						if( sig.getSignalName() == inputSignal.getSignalName() )
						{
							bool isIn = false;

							for( auto alreadyIn : temp )
							{
								if( (inputSignal.getMSB() == alreadyIn.getMSB()) && ( inputSignal.getLSB() == alreadyIn.getLSB() ) )
								{
									isIn = true;
								}
							}

							if(!isIn)
							{
								temp.push_back(inputSignal);
							}
						}
					}
					
				}
			}
			
			//In this section we check if the signal is directly connected to the output
			for(auto exitingSignal : exitingSignals)
            {
                for(auto couple : coupledSignals)
                {
                    if(couple.first.compare(exitingSignal.getSignalName())==0)
                    {
                        if(couple.second.compare(sig.getSignalName()) == 0)
                        {
                            temp.push_back(exitingSignal);
                        }
                    }
                }
            }
			
			return temp;
		}

		/*
		 *	Method that, given the name of a signal, returns a list of nodes where said signal enters as a pure signal (meaning without any modification whatsoever).
		 *	Es: \CLK usually enters in the clock port without modifications. If the clock is modified at the start, maybe by doubling its period, then this component
		 *	will be the only one where \CLK enters purely. If we're discriminating, starting nodes won't include nodes where the signal enters as a selection.
		 *	Signals are considered both in name and dimension.
		 */
		std::vector<Graph_Node::Node *> Generator::Generator::startingNodes_fullSignal( Signal::Signal externalSignal, bool discriminating, bool verbose)
		{
			std::vector<Graph_Node::Node *> starting;
			std::stringstream ss;

			if(verbose)
			{
				ss << "Initiating startingNodes( " << externalSignal.getSignalName() << " [" << externalSignal.getMSB() << ":" << externalSignal.getLSB() << "] )\n";
			}

			//If we pass something that is not an external signal, we do not allow the method to proceed.
			if( !isExternalSignal_byNameOnly(externalSignal) )
			{
				ss << "\tExternal signal : " << externalSignal.getSignalName() << " does not exist!\n";
				std::cout << ss.str() << std::endl;
				return starting;
			}
				
			if(verbose)
			{
				ss << "\tAllowed execution..\n\n";
			}
			
			//Foreach node
			for( auto node : sg.getNodes() )
			{
				if(verbose)
				{
					ss << "\tNode : " << node->getName() << "\n";
				}

				//Foreach port of that node
				for( auto port : node->getInputs() )
				{
					if(verbose)
					{
						ss << "\t\tPort : " << port->getPortName() << "\n";
					}

					//Foreach entering signal
					for( auto inputSignal : port->getInputSignals() )
					{
						ss << "\t\t\tChecking for presence of : " << inputSignal.getSignalName() << "\n";
						//The name is the same, width has not been considered yet
						if( (inputSignal.getSignalName() == externalSignal.getSignalName()) && (inputSignal.getMSB() == externalSignal.getMSB()) && (inputSignal.getLSB() == externalSignal.getLSB()))
						{
							ss << "\t\t\t\tFound signal match: " << inputSignal.getSignalName() << " [" << inputSignal.getMSB() << ":" << inputSignal.getLSB() << "]\n";;
							
							//If we're discriminating, then we check. If we're not, we simply keep entersSelection=false so that the next check collapses to the first condition only
							bool entersSelection = false;

							if((discriminating) && (!node->isModulePlaceholder()))
							{
								if( VectorSupport::isIn( (sg.getCellRepository()->getCell(node->getType()))->getSelectionInputs(), port->getPortName() ) )
								{
									entersSelection = true;
								}
							}
							
							if((discriminating) && (node->isModulePlaceholder()))
							{
								for(auto selectionInput : node->getModuleSelectionPorts())
                                {
                                    if(selectionInput->getPortName().compare(port->getPortName()) == 0)
                                    {
                                        entersSelection = true;
                                                    
                                        for(auto dataInput : node->getModuleDataPorts())
                                        {
                                            if(dataInput->getPortName().compare(selectionInput->getPortName()) == 0)
                                            {
                                                entersSelection = false;
                                            }
                                        }
                                                    
                                    }
                                }
							}

							//If the node is already there, do not insert it. The signalMatchCount supports multiple signal inputs.
							if( !VectorSupport::isIn(starting, node) && !entersSelection )
							{
								if(verbose)
								{
									ss << "\t\t\tInserting startingNode: " << node->getName() << "\n";
								}
								starting.push_back(node);
							}
						}
					}
				}
			}

			if(verbose)
			{
				ss << "StartingNode result for : " << externalSignal.getSignalName() << "\n";
				for( auto start : starting )
				{
					ss << "\t" << start->getName() << "\n";
				}
				ss << "\n";
				std::cout << ss.str() << std::endl;
			}

			return starting;
		}

		/*
		 *	Method that, given the name of a signal, returns a list of nodes where said signal enters as a pure signal (meaning without any modification whatsoever).
		 *	Es: \CLK usually enters in the clock port without modifications. If the clock is modified at the start, maybe by doubling its period, then this component
		 *	will be the only one where \CLK enters purely. If we're discriminating, starting nodes won't include nodes where the signal enters as a selection.
		 *	Signals are considered only in name.
		 */
		std::vector<Graph_Node::Node *> Generator::Generator::startingNodes_byNameOnly( Signal::Signal externalSignal, bool discriminating, bool verbose)
		{
			std::vector<Graph_Node::Node *> starting;
			std::stringstream ss;

			if(verbose)
			{
				ss << "Initiating startingNodes( " << externalSignal.getSignalName() << " [" << externalSignal.getMSB() << ":" << externalSignal.getLSB() << "] )\n";
			}

			//If we pass something that is not an external signal, we do not allow the method to proceed.
			if( !isExternalSignal_byNameOnly(externalSignal) )
			{
				ss << "\tExternal signal : " << externalSignal.getSignalName() << " does not exist!\n";
				std::cout << ss.str() << std::endl;
				return starting;
			}
				
			if(verbose)
			{
				ss << "\tAllowed execution..\n\n";
			}
			
			//Foreach node
			for( auto node : sg.getNodes() )
			{
				if(verbose)
				{
					ss << "\tNode : " << node->getName() << "\n";
				}

				//Foreach port of that node
				for( auto port : node->getInputs() )
				{
					if(verbose)
					{
						ss << "\t\tPort : " << port->getPortName() << "\n";
					}

					//Foreach entering signal
					for( auto inputSignal : port->getInputSignals() )
					{
						ss << "\t\t\tChecking for presence of : " << inputSignal.getSignalName() << "\n";
						//The name is the same, width has not been considered yet
						if( ( inputSignal.getSignalName() == externalSignal.getSignalName()) )
						{
							ss << "\t\t\t\tFound signal match: " << inputSignal.getSignalName() << " [" << inputSignal.getMSB() << ":" << inputSignal.getLSB() << "]\n";;
							
							//If we're discriminating, then we check. If we're not, we simply keep entersSelection=false so that the next check collapses to the first condition only
							bool entersSelection = false;

							if((discriminating) && (!node->isModulePlaceholder()))
							{
                                if(!node->isModulePlaceholder())
                                {
                                    if( VectorSupport::isIn( (sg.getCellRepository()->getCell(node->getType()))->getSelectionInputs(), port->getPortName() ) )
                                    {
                                        entersSelection = true;
                                    }
                                }
								
							}

							//If the node is already there, do not insert it. The signalMatchCount supports multiple signal inputs.
							if( !VectorSupport::isIn(starting, node) && !entersSelection )
							{
								if(verbose)
								{
									ss << "\t\t\tInserting startingNode: " << node->getName() << "\n";
								}
								starting.push_back(node);
							}
						}
					}
				}
			}

			if(verbose)
			{
				ss << "StartingNode result for : " << externalSignal.getSignalName() << "\n";
				for( auto start : starting )
				{
					ss << "\t" << start->getName() << "\n";
				}
				ss << "\n";
				std::cout << ss.str() << std::endl;
			}

			return starting;
		}
        

		/*
		 *	Find a correspondant among the coupledSignals. If getNew, we search for a forward renaming y such that x -> y, otherwise x <- y
		 *	<newName , oldName>
		 */
		std::string Generator::Generator::getRenaming(std::string signalName, bool getNew)
		{
			for( auto pair : this->coupledSignals )
			{
				if(getNew)
				{
					//Old name is in the pair.second
					if( signalName == pair.second )
					{
						return pair.first;
					}
				}
				else
				{
					//New name is in the pair.first
					if( signalName == pair.first )
					{
						return pair.second;
					}	
				}
			}
			return "";
		}

		/*
		 *	Is the signal passed an external signal
		 */
		bool Generator::Generator::isExternalSignal_byNameOnly(Signal::Signal signal)
		{
			for( auto sig : this->externalSignals )
			{
				if( signal.getSignalName() == sig.getSignalName() )
				{
					return true;
				}
			}
			return false;
		}
		
		/*
		 *	Returns the list of nodes which receives as inputs signals coming from outside the current module.
		 */
		std::vector<Graph_Node::Node *> Generator::Generator::signalReceivers(bool discriminating)
		{
			std::vector<Graph_Node::Node *> temp;

			for( auto sig : this->enteringSignals )
			{
				temp = VectorSupport::merge( temp, startingNodes_byNameOnly(sig, discriminating) );
			}

			return temp;
		}

		/*
		 *	Returns the list of nodes which outputs a signal exiting the current module.
		 */
		std::vector<Graph_Node::Node *> Generator::Generator::signalEmitters()
		{
			std::vector<Graph_Node::Node *> temp;

			for( auto node : sg.getNodes() )
			{
				if( node->isEmitter() )
				{
					temp = VectorSupport::conditionalInsert(temp, node);
				}
			}

			return temp;
		}

		/*
		 *	Is the node a receiver?
		 */
		bool Generator::Generator::isReceiver(Graph_Node::Node *node)
		{
			std::vector<Graph_Node::Node *> temp;

			for( auto receiver : signalReceivers(false) )
			{
				if( node->getName() == receiver->getName() )
				{
					return true;
				}
			}
			return false;
		}

		/*
		 *	Method to transform the support structure in a string.
		 *	std::vector< std::pair< Signal::Signal, std::vector<char> > >
		 */
		std::string Generator::Generator::condenseBit()
		{
			std::stringstream ss;
			
			for( auto pair : this->sliceStructure)
			{
				ss << "\t" << pair.first.toString();
				ss << " [";

				for( int i=(int)pair.second.size()-1; i>=0; i--)
				{
					ss << pair.second[i];
				}

				ss << "]\n\n";
			}

			return ss.str();
		}
		
		/*
         * Method used for knowing if a module contains other modules
         */
        bool Generator::Generator::isAtomic()
        {
            if(this->innerModules.size() == 0)
            {
                return true;
            }

            return false;
        }

		//Getters
		std::string Generator::Generator::getModuleName(){ return this->moduleName; }
		SupportGraph::SupportGraph Generator::Generator::getSupportGraph(){return this->sg; }
		std::vector<Signal::Signal> Generator::Generator::getEnteringSignals(){ return this->enteringSignals; }
		std::vector<Signal::Signal> Generator::Generator::getExitingSignals(){ return this->exitingSignals; }
		std::vector<Signal::Signal> Generator::Generator::getExternalSignals() { return this->externalSignals; }
		std::vector<std::pair< Signal::Signal, std::vector<char> > > Generator::Generator::getSliceStructure() { return this->sliceStructure; }
		std::vector<std::pair<std::string, std::string> > Generator::Generator::getCoupledSignals() { return this->coupledSignals; }
		std::vector<std::pair<Signal::Signal, Signal::Signal>> Generator::Generator::getPairedSignals(){return this->pairedSignals;}
		std::vector<std::pair<std::string,std::string>> Generator::Generator::getInnerModules(){return this->innerModules; }
		std::vector<Signal::Signal> Generator::Generator::getInoutSignals(){return this->inoutSignals; }
        
		void Generator::Generator::setSliceStructure( std::vector< std::pair< Signal::Signal, std::vector<char> > > structure )
		{
			this->sliceStructure = structure;
		}

		//Classic toString method
        std::string Generator::Generator::toString()
        {
            std::stringstream ss;

            ss << "\n==============={Module: " << this->getModuleName() << "}===============" << "\n";

            ss << "------{Entering Signals}------" << "\n";
            for(auto sig : this->enteringSignals)
            {
                ss << sig.toString() << "\n";
            }

            ss << "------{Exiting Signals}------" << "\n";
            for(auto sig : this->exitingSignals)
            {
                ss << sig.toString() << "\n";
            }

            ss << "IsAtomic: " ;
            if(!this->isAtomic()){
                 ss << " <false>" << "\n";
                 for(auto im : this->innerModules)
                 {
                     ss << " [inner module > name: " << im.first << " type: " << im.second << "\n";
                 }
            }
            else ss << " <true>" << "\n";

            return ss.str();
        }
