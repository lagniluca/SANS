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

//Yosys libraries
#include "kernel/yosys.h"
#include "backends/ilang/ilang_backend.h"

//Application libraries
#include "supportGraph.h"
#include "generator_analyzer.h"

USING_YOSYS_NAMESPACE
   
            
            std::stringstream GeneratorAnalyzer::GeneratorAnalyzer::getHeader(Generator::Generator *wrappedModule,std::string actionPerformed)
            {
                std::stringstream ss ;
                
                ss << "\n=========={Module: " << wrappedModule->getModuleName() << " performing: " << actionPerformed << "}========== \n";
                
                return ss;
            }
            
           GeneratorAnalyzer::GeneratorAnalyzer::GeneratorAnalyzer()
           {
               /*Test zone*/
				//TEST_findSignalFriends();      	//OK
				//TEST_entering_emitter_nodes(); 	//OK
				//TEST_node_classification();    	//OK
				//TEST_allNextNodes();           	//OK
				//TEST_startingNodes(false);     	//OK
				
				//log("\n\n\t***********************************************************************\n\n");

				/*Print zone*/
				//PRINT_externalSignals(); 		  	//OK

				//log("\t***********************************************************************\n\n");

				//PRINT_supportGraph();			 	//OK
           }
           
           //Method used for performing all the other tests
           void GeneratorAnalyzer::GeneratorAnalyzer::TEST_all(Generator::Generator *wrappedModule)
           {
              std::stringstream ss = this->getHeader(wrappedModule, "TEST_all"); 
              
              std::cout << ss.str() ;
              
              this->TEST_findSignalFriends(wrappedModule);
              this->TEST_entering_emitter_nodes(wrappedModule);
              this->TEST_node_classification(wrappedModule);
              this->TEST_allNextNodes(wrappedModule);
              this->TEST_startingNodes(wrappedModule, false);
           }
           
           //Method used for printing all options
           void GeneratorAnalyzer::GeneratorAnalyzer::PRINT_all(Generator::Generator *wrappedModule)
           {
              std::stringstream ss = this->getHeader(wrappedModule, "TEST_all"); 
              
              std::cout << ss.str() ; 
              
              this->PRINT_externalSignals(wrappedModule);
              this->PRINT_supportGraph(wrappedModule);
           }
            
            //Test method for allNextNodes()
			void GeneratorAnalyzer::GeneratorAnalyzer::TEST_allNextNodes(Generator::Generator *wrappedModule)
			{
				std::stringstream ss = this->getHeader(wrappedModule, "TEST_allNextNodes");
                
                SupportGraph::SupportGraph sg = wrappedModule->getSupportGraph();

				for( auto node : sg.getNodes() )
				{
					ss << "========================== Not Discriminating " << node->getName() << "\n";
					for( auto nextNode : sg.allNextNodes(node, false) )
					{
						ss << "\t" << nextNode->getName() << "\n";
						std::cout << ss.str() << std::endl;
					}

					ss << "========================== Discriminating " << node->getName() << "\n";
					for( auto nextNode : sg.allNextNodes(node, true) )
					{
						ss << "\t" << nextNode->getName() << "\n";
					}

					ss << "--------------------------------------------------------------\n";
				}
				std::cout << ss.str() << std::endl;
			}
			
			//Test method for startingNodes()
			void GeneratorAnalyzer::GeneratorAnalyzer::TEST_startingNodes(Generator::Generator *wrappedModule, bool verbose, bool checkAllSignals)
			{
				std::stringstream ss = this->getHeader(wrappedModule, "TEST_startingNodes");

				if(checkAllSignals)
				{
					for( auto externalSig : wrappedModule->getExternalSignals() )
					{
						ss << "\n\nNOT DISCRIMINATING ____________________________________ " << externalSig.toString() << "\n";

						for( auto node : wrappedModule->startingNodes_byNameOnly(externalSig, false, verbose) )
						{
							ss << "\t" << node->getName() << "\n";
						}

						ss << "\n\nDISCRIMINATING ____________________________________ " << externalSig.toString() << "\n";

						for( auto node : wrappedModule->startingNodes_byNameOnly(externalSig, true, verbose) )
						{
							ss << "\t" << node->getName() << "\n";
						}

						ss << "\n";
					}
				}
				else
				{
					for( auto externalSig : wrappedModule->getEnteringSignals() )
					{
						ss << "\n\nNOT DISCRIMINATING ____________________________________ " << externalSig.toString() << "\n";

						for( auto node : wrappedModule->startingNodes_byNameOnly(externalSig, false, verbose) )
						{
							ss << "\t" << node->getName() << "\n";
						}

						ss << "\n\nDISCRIMINATING ____________________________________ " << externalSig.toString() << "\n";

						for( auto node : wrappedModule->startingNodes_byNameOnly(externalSig, true, verbose) )
						{
							ss << "\t" << node->getName() << "\n";
						}

						ss << "\n";
					}
				}

				ss << "\n------------------------------------------------------------------------\n";

				std::cout << ss.str() << std::endl;
			}
			
			//Test method for findSignalFriends()
			void GeneratorAnalyzer::GeneratorAnalyzer::TEST_findSignalFriends(Generator::Generator *wrappedModule)
			{
				std::stringstream ss = this->getHeader(wrappedModule, "TEST_findSignalFriends");

				for( auto sig : wrappedModule->getExternalSignals() )
				{
					ss << "NOT INCLUDING SELF : " << sig.toString() << "\n";
					for( auto foundSig : wrappedModule->findSignalFriends(sig, false) )
					{
						ss << "\tFound : " << foundSig.toString() << "\n";
					}
					ss << "\n";

					ss << "INCLUDING SELF : " << sig.toString() << "\n";
					for( auto foundSig : wrappedModule->findSignalFriends(sig, true) )
					{
						ss << "\tFound : " << foundSig.toString() << "\n";
					}
					ss << "\n________________________________________________________\n";
				}

				std::cout << ss.str() << std::endl;

			}
			
			//Test method for node classification
			void GeneratorAnalyzer::GeneratorAnalyzer::TEST_node_classification(Generator::Generator *wrappedModule)
			{
				std::stringstream ss = this->getHeader(wrappedModule, "TEST_node_classification");
                
                SupportGraph::SupportGraph sg = wrappedModule->getSupportGraph();

				for( auto node : sg.getNodes() )
				{
					ss << node->getName() << " <";
					if( wrappedModule->isReceiver(node) )
					{
						ss << " R ";
					}
					if( node->isEmitter() )
					{
						ss << " E ";
					}
					ss << ">\n";
				}

				ss << "\n-----------------------------------------\n";

				std::cout << ss.str() << std::endl;
			}
			
			//Test method for receiver and emitter nodes
			void GeneratorAnalyzer::GeneratorAnalyzer::TEST_entering_emitter_nodes(Generator::Generator *wrappedModule)
			{
				std::stringstream ss = this->getHeader(wrappedModule, "TEST_entering_emitter_nodes");

				ss << "_____________________________________Entering nodes (not discriminating)\n";
				for( auto node : wrappedModule->signalReceivers(false) )
				{
					ss << "\t" << node->getName() << "\n";
				}

				ss << "_____________________________________Entering nodes (discriminating)\n";
				for( auto node : wrappedModule->signalReceivers(true) )
				{
					ss << "\t" << node->getName() << "\n";
				}

				ss << "_____________________________________Exiting nodes\n";
				for( auto node : wrappedModule->signalEmitters() )
				{
					ss << "\t" << node->getName() << "\n";
				}

				std::cout << ss.str() << std::endl;
			}
			
			//Print supportGraph
			void GeneratorAnalyzer::GeneratorAnalyzer::PRINT_supportGraph(Generator::Generator *wrappedModule)
			{
				std::stringstream ss = this->getHeader(wrappedModule, "PRINT_supportGraph");

				ss << wrappedModule->getSupportGraph().toString();

				std::cout << ss.str() << std::endl;
			}

			//Print externalSignals
			void GeneratorAnalyzer::GeneratorAnalyzer::PRINT_externalSignals(Generator::Generator *wrappedModule)
			{
				std::stringstream ss = this->getHeader(wrappedModule, "PRINT_externalSignals");

				ss << "\tExternal signals vector\n";
				for( auto sig : wrappedModule->getExternalSignals() )
				{
					ss << "\t\t" << sig.toString() << "\n";
				}
				ss << "\n";

				ss << "\tEntering signals vector\n";
				for( auto sig : wrappedModule->getEnteringSignals() )
				{
					ss << "\t\t" << sig.toString() << "\n";
				}
				ss << "\n";

				ss << "\tExiting signals vector\n";
				for( auto sig : wrappedModule->getExitingSignals() )
				{
					ss << "\t\t" << sig.toString() << "\n";
				}
				ss << "\n";

				std::cout << ss.str() << std::endl;
			}           
