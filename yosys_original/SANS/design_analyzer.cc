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

#include "design_analyzer.h"

void DesignAnalyzer::DesignAnalyzer::wrapModule(RTLIL::Module *module)
{
    SubCircuit_v2::Graph internalState;
    Generator::Generator *gen = nullptr ;
    
    gen = new Generator::Generator(internalState, module);
    
    //GeneratorAnalyzer::GeneratorAnalyzer undyne;
    //undyne.PRINT_supportGraph(gen);

    this->wrappedModules.push_back(gen);
}

std::vector< std::pair< Signal::Signal, std::pair<bool, bool> > > DesignAnalyzer::DesignAnalyzer::sort( std::vector< std::pair< Signal::Signal, std::pair<bool, bool> > > toSort)
{
    std::vector<Signal::Signal> toSortTemp;

    for( auto sigPair : toSort )
    {
        toSortTemp.push_back(sigPair.first);
    }

    std::vector<Signal::Signal> sortedTemp = Signal::sort(toSortTemp);

    std::vector< std::pair< Signal::Signal, std::pair<bool, bool> > > sorted;

    for( auto sortedSig : sortedTemp )
    {
        for( auto unsortedPair : toSort )
        {
            if( sortedSig == unsortedPair.first)
            {
                sorted.push_back(unsortedPair);
            }
        }
    }

    return sorted;
}

std::vector<std::string> DesignAnalyzer::DesignAnalyzer::prepareResults( std::string title, Generator::Generator *mod, std::vector< std::pair< Signal::Signal, std::pair<bool, bool> > > allResults, bool condense )
{
    std::vector<std::string> res;

    res.push_back(preparePlaintext(title, mod, allResults, condense));
    res.push_back(prepareXML(title, mod, allResults, condense));
    res.push_back(prepareJSON(title, mod, allResults, condense));
    res.push_back(prepareCSV(title, mod, allResults, condense));

    return res;
}

std::string DesignAnalyzer::DesignAnalyzer::preparePlaintext( std::string title, Generator::Generator *mod, std::vector< std::pair< Signal::Signal, std::pair<bool, bool> > > allResults, bool condense )
{
    std::stringstream plaintext;
    //Not condensing
    if(!condense)
    {
        plaintext << "\n========================= { Module: " << title << " } =========================\n\n";

        for( auto pair : allResults )
        {
            plaintext << "\t" << pair.first.toString() << "\t<" << pair.second.first << " " << pair.second.second << ">\n";
        }

        plaintext << "\n";
    }
    //Condensing
    else
    {
        plaintext << "\n========================= { Module: " << title << " } =========================\n\n";

        for( auto pair : mod->getSliceStructure())
        {
            plaintext << "\t" << pair.first.toString();
            plaintext << " [";

            for( int i=(int)pair.second.size()-1; i>=0; i--)
            {
                plaintext << pair.second[i];

            }

            plaintext << "]\n";
        }
    }

    return plaintext.str();
}

std::string DesignAnalyzer::DesignAnalyzer::prepareXML( std::string title, Generator::Generator *mod, std::vector< std::pair< Signal::Signal, std::pair<bool, bool> > > allResults, bool condense )
{
    std::stringstream xml;
    
    //Not condensing
    if(!condense)
    {
        xml << "<module>\n" << "\t<name>\n\t\t" << title << "\n\t</name>\n";
        
        for( auto pair : allResults )
        {
            xml << "\t<signal>\n";
            xml << "\t\t<signalName>" << pair.first.toString() << "</signalName>\n";
            xml << "\t\t<data>" << pair.second.first << "</data>\n";
            xml << "\t\t<select>" << pair.second.second << "</select>\n";
            xml << "\t</signal>\n";
        }

        xml << "</module>\n";
    }
    //Condensing
    else
    {
        xml << "<module>\n" << "\t<name>\n\t\t" << title << "\n\t</name>\n";
        
        for( auto pair : mod->getSliceStructure())
        {
            xml << "\t<signal>\n";
            xml << "\t\t<signalName>" << pair.first.toString() << "</signalName>\n";
            xml << "\t\t<slice>";

            for( int i=(int)pair.second.size()-1; i>=0; i--)
            {
                xml << pair.second[i];
            }

            xml << "</slice>\n\t</signal>\n";
        }

        xml << "</module>\n";
    }

    return xml.str();
}

std::string DesignAnalyzer::DesignAnalyzer::prepareJSON( std::string title, Generator::Generator *mod, std::vector< std::pair< Signal::Signal, std::pair<bool, bool> > > allResults, bool condense )
{
    std::stringstream json;

    //Not condensing
    if(!condense)
    {
        json << "{\"module\": {\n \"name\": \"" << title << "\",\n \"signals\": [\n";
        
        //Counter for JSON syntax
        int counter = 0;

        for( auto pair : allResults )
        {

            json << "     {\"signalName\": \"" << pair.first.toString() << "\", \"data\": \"" << pair.second.first << "\", \"select\": \"" << pair.second.second << "\"}";
        
            if(counter < (int)allResults.size())
            {
                json << ",\n";
            }
            else
            {
                json << "\n";
            }

            counter++;
        }

        json << "   ]\n  }\n}\n";
    }
    //Condensing
    else
    {
        json << "{\"module\": {\n \"name\": \"" << title << "\",\n \"signals\": [\n";

        for( auto pair : mod->getSliceStructure())
        {
            json << "     {\"signalName\": \"" << pair.first.toString() << "\", \"slice\": \"";

            for( int i=(int)pair.second.size()-1; i>=0; i--)
            {
                json << pair.second[i];
            }

            json << "\"}\n";
        }

        json << "  }\n}\n";
    }

    return json.str();
}

std::string DesignAnalyzer::DesignAnalyzer::prepareCSV( std::string title, Generator::Generator *mod, std::vector< std::pair< Signal::Signal, std::pair<bool, bool> > > allResults, bool condense )
{
    std::stringstream csv;

    //Not condensing
    if(!condense)
    {
        csv << "\"ModuleName\",\t\"signalName\",\t\"data\",\t\"select\"\n";
    
        for( auto pair : allResults )
        {
            csv << "\"" <<  title << "\",\t\"" << pair.first.toString() << "\",\t" << pair.second.first << ",\t" << pair.second.second << "\n";
        }
    }
    //Condensing
    else
    {
        csv << "\"ModuleName\",\t\"signalName\",\t\"slice\"\n";

        for( auto pair : mod->getSliceStructure())
        {
            csv << "\"" <<  title << "\",\t\"" << pair.first.toString() << "\",\t\"";
            
            for( int i=(int)pair.second.size()-1; i>=0; i--)
            {
                csv << pair.second[i];
            }

            csv << "\"\n";
        }
    }
    
    return csv.str();
}

DesignAnalyzer::DesignAnalyzer::DesignAnalyzer(RTLIL::Design *design)
{
    for(auto module : design->modules())
    {
        this->wrapModule(module);
    }
}

void DesignAnalyzer::DesignAnalyzer::PRINT_allModules()
{
    for(auto wa : this->wrappedModules)
    {
        std::cout << wa->toString() << std::endl;
    }
}

void DesignAnalyzer::DesignAnalyzer::PRINT_allModulesSupportGraphs()
{
    for(auto wa : this->wrappedModules)
    {
        std::cout << wa->getSupportGraph().toString() << std::endl;
    }
}

void DesignAnalyzer::DesignAnalyzer::PRINT_CoupledSignals()
{
    for(auto wa : this->wrappedModules)
    {
        std::cout << "++++++{Module: " << wa->getModuleName() << "}++++++" << std::endl;
        
        for(auto cs : wa->getCoupledSignals())
        {
            std::cout << "signal 1: " << cs.first << " signal 2: " << cs.second << std::endl;
        }
    }
}
