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

#ifndef DESIGNANALYZER_H
#define DESIGNANALYZER_H

//Yosys libraries
#include "kernel/yosys.h"
#include "backends/ilang/ilang_backend.h"

//Application libraries
#include "stdvector_support.h"
#include "stdstring_support.h"
#include "supportGraph.h"
#include "module_to_graph.h"
#include "generator_library.h"
#include "generator_analyzer.h"
#include "dfs.h"

USING_YOSYS_NAMESPACE

/*
 *  Main container of the RTLIL::Design. From here all analysis should start.
 */
namespace DesignAnalyzer
{
    /*
     *  Struct that contains elaborated data to print in various settings.
     */
    struct Final_Results
    {
        std::string plaintext;
        std::string xml;
        std::string json;
        std::string csv;
    };

    class DesignAnalyzer
    {
        private:
            Final_Results dfs_results;

            //List of all the modules' wrapper
            std::vector<Generator::Generator *> wrappedModules ;
            
            /*
             *  Creation of wrapper modules
             */
            void wrapModule(RTLIL::Module *module);
            
            /*
             *  Sorter for DFS' result vector
             */
            std::vector< std::pair< Signal::Signal, std::pair<bool, bool> > > sort( std::vector< std::pair< Signal::Signal, std::pair<bool, bool> > > toSort);

            /*
             *  Results are a list of strings. allResults must be passed through the sort() method beforehand to obtain organized results.
             */
            std::vector<std::string> prepareResults( std::string title, Generator::Generator *mod, std::vector< std::pair< Signal::Signal, std::pair<bool, bool> > > allResults, bool condense=false );

            /*
             *  Prepare plaintext.
             */
            std::string preparePlaintext( std::string title, Generator::Generator *mod, std::vector< std::pair< Signal::Signal, std::pair<bool, bool> > > allResults, bool condense=false );

            /*
             *  Prepare XML
             */
            std::string prepareXML( std::string title, Generator::Generator *mod, std::vector< std::pair< Signal::Signal, std::pair<bool, bool> > > allResults, bool condense=false );
            
            /*
             *  Prepare JSON
             */
            std::string prepareJSON( std::string title, Generator::Generator *mod, std::vector< std::pair< Signal::Signal, std::pair<bool, bool> > > allResults, bool condense=false );

            /*
             *  Prepare CSV
             */
            std::string prepareCSV( std::string title, Generator::Generator *mod, std::vector< std::pair< Signal::Signal, std::pair<bool, bool> > > allResults, bool condense=false );

        public:

            DesignAnalyzer(RTLIL::Design *design);
            
            /*
             *  Method used for printing all the modules.
             */
            void PRINT_allModules();
            
            /*
             *  Method used for printing all the modules' support graphs
             */
            void PRINT_allModulesSupportGraphs();
            
            /*
             *  Method used for printing all the modules.
             */
            void PRINT_CoupledSignals();
            
            /*
             *  Method used for printing the DFS of all the signals of the design.
             */
            Final_Results DFS(bool enableDeepScan, bool condense=false, bool pt=false, bool xm=false, bool js=false, bool cs=false)
            {
                std::stringstream plaintext;
                std::stringstream xml;

                if(condense)
                {
                    xml << "<condensed>\n";
                }
                else
                {
                    xml << "<wholesignals>\n";
                }

                std::stringstream json;
                std::stringstream csv;
                
                std::vector<DFS::DFSDSS *> processedAtomicModules;
                
                bool goAhead = false;
                bool found = false;
                std::vector<Generator::Generator *> analyzedWrappedModules;
                
                //Analizzo i moduli in ordine di "atomicità"
                do
                {
                    goAhead = false;
                    
                    //Guardo tra i miei wrapped modules
                    for(auto wm : this->wrappedModules)
                    {
                        found = false;
                        
                        //Per ognuno dei wrapped modules controllo che non siano già stati analizzati in fasi precedenti
                        for(auto awm : analyzedWrappedModules)
                        {
                            if(awm == wm)
                            {
                                found = true ;
                                break;
                            }
                        }
                        
                        //Non ho ancora analizzato il modulo
                        if(!found)
                        {
                            //Vettore di flag booleani che indica se abbiamo già esplorato un modulo interno
                            std::vector<bool> exploredInnerModules ;
                            //flag booleano che indica se possiamo procedere con l'analisi
                            bool canProcede = true;
                                
                            //Verifico se il modulo non è atomico ma contiene altri moduli atomici precedentemente esplorati
                            for(auto mn : wm->getInnerModules())
                            {
                                found = false ;
                                    
                                for(auto ppm : processedAtomicModules)
                                {
                                    //Se il modulo interno è tra quelli già esplorati lo setto a true
                                    if(mn.second == ppm->getTargetModule()->getModuleName())
                                    {
                                        found = true;
                                    }
                                }
                                    
                                //Aggiungo se il modulo interno è già stato esplorato
                                exploredInnerModules.push_back(found);
                            }
                                
                            //Verifico se vi sono moduli interni non ancora esplorati
                            for(auto eim : exploredInnerModules)
                            {
                              //Se è così non posso procedere 
                              if(!eim)
                              {
                                   canProcede = false;
                              }
                            }
                                
                            //Se non posso procedere , vedrò al prossimo ciclo
                            if(!canProcede)
                            {
                                goAhead = true;
                            }
                            //Posso procedere -> tutti i moduli interni sono già stati esplorati
                            else
                            {
                                //Verifico lo stato dei segnali del nodo attuale
                                DFS::DFS dfs(wm, this->wrappedModules, enableDeepScan);
                                //dfs.setPreviouslyProcessedModules(processedAtomicModules);
                                std::vector< std::pair< Signal::Signal, std::pair<bool, bool> > > results = dfs.DFS_FTSA(false);
                                
                        
                                DFS::DFSDSS *newRes = new DFS::DFSDSS(wm);
                                newRes->setPreviouslyProcessedResults(results);
                                //processedAtomicModules = VectorSupport::merge(processedAtomicModules, newRes);
                                bool isIn = false;
                                
                                //Verifico di non aver già inserito il modulo all'interno di quelli visitati in precedenza
                                for(auto pam : processedAtomicModules)
                                {
                                    if(pam->getTargetModule() == newRes->getTargetModule())
                                    {
                                        isIn = true;
                                    }
                                }
                                
                                //Se non l'ho già visitato, procedo
                                if(!isIn)
                                {
                                   processedAtomicModules.push_back(newRes);
                                }
                                
                                //processedAtomicModules.push_back(newRes);
                                
                                //Vettore di segnali su cui è già stato effettuato l'AND bit a bit
                                std::vector<std::pair<std::string, std::pair<bool, bool>>> exploredSignals;
                                //Segnali analizzati
                                std::vector<Signal::Signal> analyzedSignals ;
                                
                                //Verifico se ho già analizzato tutti i segnali di un certo tipo
                                for(auto sigs : newRes->getPreviouslyProcessedResults())
                                {
                                   
                                    bool explored = false;
                                    
                                    for(auto as : analyzedSignals)
                                    {
                                        //Se il segnale è già stato analizzato posso procedere oltre
                                        if(as == sigs.first)
                                        {
                                            explored = true;
                                            break;
                                        }
                                    }
                                    
                                    //Nel caso un segnale non fosse stato ancora analizzato, procedo con l'AND bit a bit ...
                                    if(!explored)
                                    {
                                        explored = false;
                                        
                                        //Verifico se sono presenti altri segnali dello stesso tipo e , se si , aggiorno lo stato di tali segnali
                                        for(auto es : exploredSignals)
                                        {
                                            if(es.first.compare(sigs.first.getSignalName()) == 0)
                                            {
                                                
                                                es.second.first = es.second.first || sigs.second.first;
                                                es.second.second = es.second.second || sigs.second.second;
                                                
                                                explored = true;
                                                break;
                                            }
                                        
                                        }
                                        
                                        //Inserisco una nuova enty con il nuovo valore
                                        if(!explored)
                                        {
                                            std::pair<bool, bool> dataSelect = std::make_pair(sigs.second.first, sigs.second.second);
                                            std::pair<std::string, std::pair<bool, bool>> signalState = std::make_pair(sigs.first.getSignalName(), dataSelect);
                                            exploredSignals.push_back(signalState);
                                        }
                                        
                                    }
                                }
                                
                                //Aggiorno tutti i nodi dei moduli che fanno riferimento al nodo module appena trovato
                                for(auto wa : this->wrappedModules)
                                {
                                    for(auto node : wa->getSupportGraph().getNodes())
                                    {
                                        //Vado ad aggiornare tutti i nodi che hanno a che vedere con questo modulo interno appena processato
                                        if(node->getType().compare(newRes->getTargetModule()->getModuleName()) == 0)
                                        {
                                            for(auto inputPort : node->getInputs())
                                            {
                                                for(auto exploredSignal : exploredSignals)
                                                {
                                                    //Per ogni porta del modulo , gli setto se è di selezione , data o entrambe
                                                    if(inputPort->getPortName().compare(exploredSignal.first) == 0)
                                                    {
                                                            
                                                        //std::cout << "node: " << node->getType() << "port: " << inputPort->getPortName() << " signal: " << exploredSignal.first << " data = " << std::to_string(exploredSignal.second.first) << " selection: " << std::to_string(exploredSignal.second.second) << std::endl;
                                                        node->setModulePort(inputPort,exploredSignal.second.first,exploredSignal.second.second);
                                                       
                                                    }
                                                    
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                                
                            
                        }
                    }
                    
                }while(goAhead);
                

                for(auto pam : processedAtomicModules)
                {
                    std::vector< std::pair< Signal::Signal, std::pair<bool, bool> > > results = pam->getPreviouslyProcessedResults();
                    
                    std::string title = pam->getTargetModule()->getModuleName() + " [Deep Scan: " ;
                    if(enableDeepScan)
                    {
                        title += "-enabled- ]";
                    }
                    else
                    {
                        title += "-disabled- ]" ;
                    }
                    
                    //std::vector<std::string> prepareResults( std::string title, Generator::Generator *mod, std::vector< std::pair< Signal::Signal, std::pair<bool, bool> > > allResults, bool condense=false )
                    std::vector<std::string> fr = prepareResults( title,  pam->getTargetModule(), sort(results), condense );
                    
                    if(pt)
                    {
                        plaintext << fr[0] << "\n";
                    }
                    if(xm)
                    {
                        xml << fr[1];
                    }
                    if(js)
                    {
                        json << fr[2] << "\n";
                    }
                    if(cs)
                    {
                        csv << fr[3] << "\n";
                    }
                }

                if(condense)
                {
                    xml << "</condensed>";
                }
                else
                {
                    xml << "</wholesignals>";
                }

                dfs_results.plaintext = plaintext.str();
                dfs_results.xml = xml.str();
                dfs_results.json = json.str();
                dfs_results.csv = csv.str();

                return dfs_results;
            }
    };
}

#endif
