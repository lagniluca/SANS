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

#ifndef DFS_H
#define DFS_H

//Standard libraries
#include <string>
#include <vector>
#include <iostream>

//Yosys libraries
#include "kernel/yosys.h"
#include "backends/ilang/ilang_backend.h"

//Application libraries
#include "stdvector_support.h"
#include "stdstring_support.h"
#include "supportGraph.h"
#include "module_to_graph.h"
#include "cellrepo.h"
#include "generator_library.h"

USING_YOSYS_NAMESPACE

namespace DFS
{
    /*
     *  Depth First Search Deep Scan Support
     */
    class DFSDSS
    {
        private:
            //Reference of the analyzed node
            Generator::Generator *targetModule = nullptr;
            //List of results obtained in previous calls of the deepscan
            std::vector< std::pair<Signal::Signal, std::pair<bool, bool>>> previouslyProcessedResults;
        
        public:
            DFSDSS(Generator::Generator *targetModule);
            
            /*
             *  Method used for setting the results obtained by previous analysys
             */
            void setPreviouslyProcessedResults(std::vector< std::pair<Signal::Signal, std::pair<bool, bool>>> previouslyProcessedResults);

            Generator::Generator *getTargetModule();

            std::vector< std::pair<Signal::Signal, std::pair<bool, bool>>> getPreviouslyProcessedResults();
    };
    
    /*
     *  Depth-First Search
     */
    class DFS
    {
        private:
            //Module where we have to start the DFS analisys
            Generator::Generator *currentModule = nullptr;
            //All the modules of the design, used for performing the deepscan
            std::vector<Generator::Generator *> designModules ;
            CellRepository::CellRepository *cr = new CellRepository::CellRepository();
            //Boolean flag used to state if the deepscan (i.e. looking inside module cells) is enabled
            bool deepScanEnabled = false;
            
            std::vector<DFSDSS *>previouslyProcessedModules;
			
			/*
			 *	Method that covers the last possible selection case that the DFS could not cover due to forced discrimination. If the signal enters
			 *	straight into a selection port, discrimination will trim the whole node before the analysis could proceed. Here we perform startingNodes()
			 *	without discrimination to enter said node, retrieve which port we entered and potentially set toCorrect = true, meaning that it's a selection
			 *	signal.
			 */
			bool DFS_MooreCorrection( Signal::Signal signal );

			/*
			 *	Sub-method of DFS. It performs the checks to know whether a signal is a data signal. The result is stored inside bool& data.
			 */
			void DFS_Data( Graph_Node::Node *currentNode, std::vector<bool> visited, bool &data );
			
			/*
			 *	Sub-method of DFS. It performs the checks to know wheter a signal is a selector. The result is stored inside bool& select.
			 */
			void DFS_Select( Graph_Node::Node *currentNode, std::vector<bool> visited, bool &select );
			
			/*
             *  Method used for looking inside inner modules in order to decide if a signal is used as DATA/SELECT inside inner modules.
             */ 
            std::pair<bool,bool> DFS_DeepScan( Graph_Node::Node *analyzedNode,Graph_Port::Port *analyzedPort );

        protected:        
            //Test method for DFS on a node
			void TEST_DFS_NodeAnalyzer();
			
			//Test method for DFS on a signal
			void TEST_DFS_SignalAnalyzer();
            
        public:
            DFS( Generator::Generator *currentModule, std::vector<Generator::Generator *> designModules, bool deepScanEnabled = true );
            
            //Getters
            Generator::Generator *getCurrentModule();
            std::vector<Generator::Generator *> getDesignModules();

            //Method used for decree if the deepscan is enabled
            bool isDeepScanEnabled();

            /*
             * Method used for enabling the deepscan (i.e. looking inside inner modules in case of nested modules)
             * 
             * deepscan can be enabled only if the design modules have been setted
             */
            bool enableDeepScan();
            
            /*
             * Method used for disabling the deepscan.
             * it is always possible to disable such operation
             */
            bool disableDeepScan();
            
            /*
             * Method used for passing the previously processed signals in case of deep-scan search
             * to further iteration DFS instances
             */
            void setPreviouslyProcessedModules(std::vector<DFSDSS *> previouslyProcessedModules);
            
            /*
			 *	Given a single signal it returns the result of a DFS Select + Data analysis.
			 */
			std::pair<bool, bool> DFS_SignalAnalyzer( Signal::Signal signal, bool verbose=false );
            
            /*
			 *	Method that prepares the field for the DFS algorithm to explore paths.
			 *	The returned pair has the shape <Data, Select>
			 */
			std::pair<bool, bool> DFS_NodeAnalyzer( Graph_Node::Node *currentNode, bool verbose=false );
			
			/*
			 *  DFS analysis with bit slices enabled. DFS_FullTrackSignalAnalysis
			 */
			std::vector< std::pair< Signal::Signal, std::pair<bool, bool> > > DFS_FTSA( bool verbose=false );
    };
}

#endif
