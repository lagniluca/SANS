/*
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
 *
 *	yosys -- Yosys Open SYthesis Suite
 *
 *	This extension allows to transform a Verilog specification into a graph
 *	to perform various actions. The main functionality is to output the list of
 *	external signal together with their type, being it a "data" signal or a
 *	"selection" signal.
 *
 *
 *	Build instruction
 *		-	Create a new directory inside the yosys main folder (the one containing frontends/, backends/, and so on..)
 *		-	Compile the module through		yosys-config --build <filename>.so <filename>.cc
 *		-	Execute 						yosys -m <filename>.so -p "<instructions>"
 *			Example of instructions:		"read_verilog component.sv; proc; opt; mpu"
 *
 *
 *	The extension is called 'Signal Analyzer aNd Stuffs'. It's easy to use, you don't need to be a Fox to understand it.
 *
 *	Current extension = v4.0
 *		-	Include guards supported
 */

//Yosys library
#include "kernel/sigtools.h"
#include "backends/ilang/ilang_backend.h"

//Application libraries
#include "generator_library.h"
#include "stdio_support.h"
#include "design_analyzer.h"

#define FILE_TXT "./OUTPUT/sans.txt"
#define FILE_XML "./OUTPUT/sans.xml"
#define FILE_JSON "./OUTPUT/sans.json"
#define FILE_CSV "./OUTPUT/sans.csv"

USING_YOSYS_NAMESPACE

PRIVATE_NAMESPACE_BEGIN

struct flags
{
    bool condenseResults = false;
    bool printModules = false;
    bool print_STDOUT = true;
    bool print_PLAINTEXT = false;
    bool print_XML = false;
    bool print_JSON = false;
    bool print_CSV = false;
} current_execution_flags;

struct SignalAnalyzer : public Pass
{
    private:
        
        /*
         *  Handle allowed CMD options. Returns number of options.
         */
        size_t cmd_args(std::vector<std::string> args, RTLIL::Design *design)
        {
            size_t argidx = 0;

            for (argidx = 1; argidx < args.size(); argidx++)
            {
                if (args[argidx] == "-condense") {
                    current_execution_flags.condenseResults = true;
                    continue;
                }
                if (args[argidx] == "-printModules") {
                    current_execution_flags.printModules = true;
                    continue;
                }
                if (args[argidx] == "-nout") {
                    current_execution_flags.print_STDOUT = false;
                    continue;
                }
                if (args[argidx] == "-txt") {
                    current_execution_flags.print_PLAINTEXT = true;
                    continue;
                }
                if (args[argidx] == "-xml") {
                    current_execution_flags.print_XML = true;
                    continue;
                }
                if (args[argidx] == "-json") {
                    current_execution_flags.print_JSON = true;
                    continue;
                }
                if (args[argidx] == "-csv") {
                    current_execution_flags.print_CSV = true;
                    continue;
                }
                break;
            }

            extra_args(args, argidx, design);

            return argidx;
        }

        /*
         *  Following the specified options, output all the requested stuff.
         */
        void outputResults(DesignAnalyzer::Final_Results frf)
        {
            if( current_execution_flags.print_STDOUT || current_execution_flags.print_PLAINTEXT )
            {
                std::cout << frf.plaintext << std::endl;
            }

            if( current_execution_flags.print_PLAINTEXT )
            {
                //Write plaintext file
                IOSupport::writeResultToCleanFile(FILE_TXT, frf.plaintext);
            }
            if( current_execution_flags.print_XML )
            {
                //Write xml file
                IOSupport::writeResultToCleanFile(FILE_XML, frf.xml);
            }

            if( current_execution_flags.print_JSON )
            {
                //Write json file
                IOSupport::writeResultToCleanFile(FILE_JSON, frf.json);
            }

            if( current_execution_flags.print_CSV )
            {
                //Write csv file
                IOSupport::writeResultToCleanFile(FILE_CSV, frf.csv);
            }
        }
    
    public:
        SignalAnalyzer() : Pass("sans") { }

        /*
         *	What happens when the SignalAnalyzer pass is called (basically our main)
         */
        void execute(std::vector<std::string> args, RTLIL::Design *design) YS_OVERRIDE
        {
            //CMD Support
            cmd_args(args, design);

            //This must be a singleton. It's not implemented like that in our project though. If future releases need two of this, be careful with files handling.
			DesignAnalyzer::DesignAnalyzer dAnalyzer(design);
            
            //dAnalyzer.PRINT_allModulesSupportGraphs();
            
            if(current_execution_flags.printModules)
            {
               dAnalyzer.PRINT_allModules();
            }

            //Final Results for File
            DesignAnalyzer::Final_Results frf;
            frf = dAnalyzer.DFS(    
                                    true,      //Deep scan heritage
                                    current_execution_flags.condenseResults,    //Condense results
                                    current_execution_flags.print_STDOUT || current_execution_flags.print_PLAINTEXT,    //Prepare the plaintext for output/plaintext file
                                    current_execution_flags.print_XML,  //Prepare the xml for xml file
                                    current_execution_flags.print_JSON, //Prepare the json for json file
                                    current_execution_flags.print_CSV   //Prepare the csv for csv file
                                );
            outputResults(frf);
		}


} SignalAnalyzer;

PRIVATE_NAMESPACE_END
