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

#include "generator_library.h"
#include "dfs.h"

DFS::DFSDSS::DFSDSS(Generator::Generator *targetModule)
{
    this->targetModule = targetModule;
}

void DFS::DFSDSS::setPreviouslyProcessedResults(std::vector< std::pair<Signal::Signal, std::pair<bool, bool>>> previouslyProcessedResults)
{
    this->previouslyProcessedResults = previouslyProcessedResults;
}

Generator::Generator* DFS::DFSDSS::getTargetModule() { return this->targetModule; }

std::vector< std::pair<Signal::Signal, std::pair<bool, bool>>> DFS::DFSDSS::getPreviouslyProcessedResults()
{
    return this->previouslyProcessedResults;
}

bool DFS::DFS::DFS_MooreCorrection( Signal::Signal signal )
{
	bool toCorrect = false;

	for( auto node : currentModule->startingNodes_byNameOnly(signal, false) )
	{
		if(toCorrect)
		{
			break;
		}

		for( auto inputPort : node->getInputs() )
		{
			if(!node->isModulePlaceholder())
			{
				//If it's a selection port
				if( VectorSupport::isIn( (cr->getCell(node->getType()))->getSelectionInputs(), inputPort->getPortName() ) )
				{
					if( Signal::isIn_byNameOnly( inputPort->getInputSignals(), signal ) )
					{
						toCorrect = true;
					}
				}
			}
			
		}
	}

	return toCorrect;
}

void DFS::DFS::DFS_Data( Graph_Node::Node *currentNode, std::vector<bool> visited, bool &data )
{
	//Mark current node as visited
	visited[ currentNode->getId() ] = true;
    
    SupportGraph::SupportGraph sg = this->currentModule->getSupportGraph();

	if( currentNode->isEmitter() )
	{
		data = true;
	}

	for( auto reachedNode : sg.allNextNodes(currentNode, true) )
	{
		//If data was already set, it's useless to proceed.
		if(data)
		{
			break;
		}

		if( !visited[reachedNode->getId() ] )
		{
			DFS_Data(reachedNode, visited, data);
		}
	}
}

void DFS::DFS::DFS_Select( Graph_Node::Node *currentNode, std::vector<bool> visited, bool &select )
{
	//Mark current node as visited
	visited[ currentNode->getId() ] = true;
    
    SupportGraph::SupportGraph sg = this->currentModule->getSupportGraph();

        //Foreach port that goes out
        for( auto currentNodePort : currentNode->getOutputs() )
        {
            if(select)
            {
                break;
            }
            

            //Take the next nodes without discriminating
            for( auto nextNode : sg.nextNodes(currentNode, currentNodePort, false) )
            {
                
                for( auto nextPortPair : /* std::vector<Graph_Node::Port *> */ sg.reachedPorts(currentNode, currentNodePort, nextNode) )
                {
                    if(!nextNode->isModulePlaceholder())
                    {
                        //std::cout << "checking if the cell is a selection port" << std::endl;
                        //std::cout << "node: " << nextNode->getType() << " port:" << nextPortPair.first->getPortName() << std::endl;
                        if( VectorSupport::isIn( (cr->getCell(nextNode->getType()))->getSelectionInputs(), nextPortPair.first->getPortName() ) )
                        {
                            select = true;
                        }
                    }
                    else
                    {
                        if(deepScanEnabled && VectorSupport::isIn(nextNode->getModuleSelectionPorts(), nextPortPair.first))
                        {
                            select = true;
                        }
                    }
                } 
            }
            
        }

	for( auto reachableNode : sg.allNextNodes(currentNode, false))
	{
		if( !visited[reachableNode->getId()] )
		{
			DFS_Select( reachableNode, visited, select );
		}
	}
}

std::pair<bool, bool> DFS::DFS::DFS_DeepScan( Graph_Node::Node *analyzedNode,Graph_Port::Port *analyzedPort )
{
    std::pair<bool,bool> result = std::make_pair(false, false);
    bool found = false;
    std::vector< std::pair<Signal::Signal, std::pair<bool, bool>>> res ;
    
    Generator::Generator *analyzedNodeModule = nullptr;
    
    //Here we are searching for the analyzedNode Generator among our list
    for(auto mw : this->designModules)
    {
        if(mw->getModuleName() == analyzedNode->getType())
        {
            analyzedNodeModule = mw;
            found = true;
        }
        
        if(found)
        {
            break;
        }
    }
    
    found = false;

    //Looking if we already have the results for tracking related to the current analyzed module
    for(auto pps : this->previouslyProcessedModules)
    {
        if((pps->getTargetModule() == analyzedNodeModule))
        {
            res = pps->getPreviouslyProcessedResults();
            found = true;
        }
        
        if(found)
        {
            break;
        }
    }
    
    if(!found)
    {
        DFS dfs(analyzedNodeModule, this->designModules);
        dfs.enableDeepScan();
        dfs.setPreviouslyProcessedModules(this->previouslyProcessedModules);
        //Here we are performing the DFS on the inner node, performing the deepscan indeed
        res = dfs.DFS_FTSA(false);
        
        DFSDSS *newRes = new DFSDSS(analyzedNodeModule);
        newRes->setPreviouslyProcessedResults(res);
        
        this->previouslyProcessedModules.push_back(newRes);
    }
    
    found = false;
    
    //Here we cycle the signals that the previous operations in order to checking if some of them is referred to the current port 
    for(auto s : res)
    {
        if(s.first.getSignalName() == analyzedPort->getPortName())
        {
            result.first = result.first || s.second.first;
            result.second = result.second || s.second.second;
            found = true;
        }
        
        if(found)
        {
            break;
        }
    }
        
    return result; 
}

void DFS::DFS::TEST_DFS_NodeAnalyzer()
{  
	for( auto node : this->currentModule->getSupportGraph().getNodes() )
	{
		DFS_NodeAnalyzer( node, true);
	}	
}

void DFS::DFS::TEST_DFS_SignalAnalyzer()
{
	/*
	 *	Technically incorrect, DFS should be called only on entering signals. This because the analysis on
	 *	exiting signals may return some results, but they are totally inconsistent. This is a test method though,
	 *	better to test on broader extents to check for sporadic segfaults.
	 */
	for( auto sig : this->currentModule->getExternalSignals() )
	{
		DFS_SignalAnalyzer(sig, true);
	}
}

DFS::DFS::DFS( Generator::Generator *currentModule, std::vector<Generator::Generator *> designModules, bool deepScanEnabled )
{
    this->currentModule = currentModule;
    this->designModules = designModules;
    if(deepScanEnabled)
        this->enableDeepScan();
    else
        this->disableDeepScan();
}

Generator::Generator* DFS::DFS::getCurrentModule() {return this->currentModule; }
std::vector<Generator::Generator *> DFS::DFS::getDesignModules() {return this->designModules; }

bool DFS::DFS::isDeepScanEnabled() {return this->deepScanEnabled; }

bool DFS::DFS::enableDeepScan()
{
    this->deepScanEnabled = false;
    
    if(this->designModules.size() > 0)
    {
        this->deepScanEnabled = true;
    }
    
    return deepScanEnabled;
}

bool DFS::DFS::disableDeepScan()
{
    this->deepScanEnabled = false;
    
    return deepScanEnabled;
}

void DFS::DFS::setPreviouslyProcessedModules(std::vector<DFSDSS *> previouslyProcessedModules)
{
    this->previouslyProcessedModules = previouslyProcessedModules;
}

std::pair<bool, bool> DFS::DFS::DFS_SignalAnalyzer( Signal::Signal signal, bool verbose )
{
	std::stringstream ss;
	// <Data, Select>
	std::pair<bool, bool> signalPair = std::make_pair(false, false);


	if( !this->currentModule->isExternalSignal_byNameOnly(signal) )
	{
		if(verbose)
		{
			ss << "\tcannot procede DFS_Launcher because it's not an external signal.\n";
			std::cout << ss.str() << std::endl;
		}

		return signalPair;
	}
	
	bool entered = false;
	for( auto node : this->currentModule->startingNodes_fullSignal(signal, true) )
	{
        entered = true;
        std::pair<bool, bool> res = std::make_pair(false, false);
        
        bool dataPort = false;
        
        if(node->isModulePlaceholder() && deepScanEnabled)
        { 
            //Verifico se il nodo viene usato come selezione
           for(auto sp : node->getModuleSelectionPorts())
           {
               for(auto inputSig : sp->getInputSignals())
               {
                   if(inputSig.getSignalName() == signal.getSignalName())
                   {
                       res.second = true;
                   }
               }
           }
           
           //Verifico se il segnale viene usato come data anche da qualche modulo interno
           for(auto dp : node->getModuleDataPorts())
           {
               for(auto inputSig : dp->getInputSignals())
               {
                   if(inputSig.getSignalName() == signal.getSignalName())
                    {
                        dataPort = true;
                    }
               }
           }
           
           //Il segnale non viene mai usato come data, posso affermare che non è un segnale dati per questo nodo
           if(!dataPort)
           {
                std::pair<bool, bool> ress = DFS_NodeAnalyzer(node);
                res.first = false;
                res.second = res.second || ress.second;
           }
            //Se viene usato come data , vado a verificare lungo il path se entra in qualche output
           else
           {
               std::pair<bool, bool> ress = DFS_NodeAnalyzer(node);
                res.first = ress.first;
                res.second = res.second || ress.second;
           }
        }
        else
        {
            res = DFS_NodeAnalyzer(node);
        }
        
        signalPair.first = signalPair.first || res.first;
        signalPair.second = signalPair.second || res.second;     
	}

    //Here I check if the input signal directly goes into a selection input without exiting
    if(!entered)
    {
        if(this->currentModule->startingNodes_fullSignal(signal, false).size() > 0)
        {
            signalPair.first = false;
            signalPair.second = true;
        }
    }
	
	signalPair.second = signalPair.second || DFS_MooreCorrection(signal);

	if(verbose)
	{
		ss << "\t<" << signalPair.first << " " << signalPair.second << ">\n\n";;
		std::cout << ss.str() << std::endl;
	}

	return signalPair;
}

std::pair<bool, bool> DFS::DFS::DFS_NodeAnalyzer( Graph_Node::Node *currentNode, bool verbose )
{
	std::stringstream ss;
	bool select = false;
	bool data = false;
    
    std::vector<Graph_Node::Node *> nodes = currentModule->getSupportGraph().getNodes();

	if(verbose)
	{
		ss << "============ DFS Launched from " << currentNode->getName();
	}

	//Boolean vector, each one represents a node, needed for the DFS
	std::vector<bool> visited;
	for(int i=0; i< (int) nodes.size(); i++)
	{
		visited.push_back(false);
	}
	
	DFS_Select( currentNode, visited, select);

	visited.clear();
	for(int i=0; i< (int) nodes.size(); i++)
	{
		visited.push_back(false);
	}

	DFS_Data( currentNode, visited, data );

	if(verbose)
	{
		ss << " <" << data << " " << select << ">\n";
	}

	std::pair<bool, bool> resultPair = std::make_pair(data, select);

	if(verbose)
	{
		std::cout << ss.str() << std::endl;
	}

	return resultPair;
}

std::vector< std::pair< Signal::Signal, std::pair<bool, bool> > > DFS::DFS::DFS_FTSA( bool verbose )
{
	std::stringstream ss;
	std::vector< std::pair< Signal::Signal, std::pair<bool, bool> > > temp;
    std::vector< std::pair< Signal::Signal, std::vector<char> > > sliceStructure = this->currentModule->getSliceStructure();
	
    //Foreach entering signal
	for( auto enteringSignal : this->currentModule->getEnteringSignals() )
	{
		int supportIndex = -1;
        
        //Find the correct index for the sliceSupport vector.
		for( int i=0; i<(int)sliceStructure.size(); i++ )
		{
			if( sliceStructure[i].first.getSignalName() == enteringSignal.getSignalName() )
			{
				supportIndex = i;
            }
		}
		
		std::vector<Signal::Signal> toAnalyze = enteringSignal.width() > 1 ? this->currentModule->findSignalFriends(enteringSignal, false) : this->currentModule->findSignalFriends(enteringSignal, true);

		//Search for all slices of that signal and keep the original one
		for( auto signal : /*findSignalFriends(enteringSignal, true)*/ toAnalyze )
		{
            std::pair<bool, bool> data_select;
            bool directOut = false;
            
            //Here we are checking if a signal is directly connected to the output
            for(auto couple : currentModule->getCoupledSignals())
            {
                if(signal.getSignalName().compare(couple.first) == 0)
                {
                    directOut = true;
                }
            }
            
            //If the signal is directly connected with the output we can say for sure that it is a data signal, we have to check only
            //the selection case
            if(directOut)
            {
                data_select = DFS_SignalAnalyzer(enteringSignal);
                data_select.first = true;
            }
            else
            {
                data_select = DFS_SignalAnalyzer(signal);
                
                //Here I check if the signal is directly connected to an InOut port of the module , if so it's a data input
                    for(auto node : this->currentModule->getSupportGraph().getNodes())
                    {
                        for(auto inputPort : node->getInputs())
                        {
                            if(inputPort->isInOutPort())
                            {
                                for(auto sig : inputPort->getSignals())
                                {
                                    if(sig.getSignalName() == signal.getSignalName())
                                    {
                                        data_select.first = true;
                                    }
                                }
                            }
                        }
                    }
            }

			//Remainder : std::vector< std::pair< Signal::Signal, std::vector<char> > > sliceStructure;
			for(int j=signal.getLSB(); j<signal.getMSB()+1; j++ )
			{
				if( j < (int)sliceStructure[supportIndex].second.size() )
				sliceStructure[supportIndex].second[j] = Generator::nextLetter( sliceStructure[supportIndex].second[j], data_select );
			}
			
			if(directOut)
            {
                temp.push_back( std::make_pair( enteringSignal, data_select ) );
            }
            else
            {
                temp.push_back( std::make_pair( signal, data_select ) );
            }

			
		}
	}

	if(verbose)
	{
		ss << "\n\tDFS_FTSA() ************************************************************\n\n";
		for( auto pair : temp )
		{
			ss << "\t\t" << pair.first.toString() << " <" << pair.second.first << " " << pair.second.second << ">\n\n";
		}

		std::cout << ss.str() << std::endl;
	}
    
    this->currentModule->setSliceStructure(sliceStructure);
	return temp;
}
