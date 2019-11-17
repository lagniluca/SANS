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

#ifndef GENERATORANALYZER_H
#define GENERATORANALYZER_H

//Application libraries
#include "generator_library.h"

USING_YOSYS_NAMESPACE

namespace GeneratorAnalyzer
{
    
    struct GeneratorAnalyzer
    {
        private:
            
            /*
             *Method used for printing a standard header
             */
            std::stringstream getHeader(Generator::Generator *wrappedModule,std::string actionPerformed);
            
        public:
           GeneratorAnalyzer();
           
           //Method used for performing all the other tests
           void TEST_all(Generator::Generator *wrappedModule);
           
           //Method used for printing all options
           void PRINT_all(Generator::Generator *wrappedModule);
            
            //Test method for allNextNodes()
			void TEST_allNextNodes(Generator::Generator *wrappedModule);
			
			//Test method for startingNodes()
			void TEST_startingNodes(Generator::Generator *wrappedModule, bool verbose, bool checkAllSignals=false);
			
			//Test method for findSignalFriends()
			void TEST_findSignalFriends(Generator::Generator *wrappedModule);
			
			//Test method for node classification
			void TEST_node_classification(Generator::Generator *wrappedModule);
			
			//Test method for receiver and emitter nodes
			void TEST_entering_emitter_nodes(Generator::Generator *wrappedModule);
			
			//Print supportGraph
			void PRINT_supportGraph(Generator::Generator *wrappedModule);

			//Print externalSignals
			void PRINT_externalSignals(Generator::Generator *wrappedModule);
    };
}

#endif