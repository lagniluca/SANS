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

#ifndef CELLREP_H
#define CELLREP_H

/*Standard C++11 libraries*/
#include <string>
#include <vector>
#include <cstdlib>

/*Yosys Libaries*/
#include "kernel/yosys.h"

namespace CellRepository
{
    /*
     * Enum that defines what a cell is used for:
     */
    enum CellCategory
    {
        //The cell purpose is to perform some kind of logical operation on data inputs
        LOGICAL_OPERATOR = 0,
        //The cell purpose is to perform some kind of arithmetical operation on data inputs
        ARITHMETIC_OPERATOR = 1,
        //The cell purpose is to perform operations on bit of data inputs
        BITWISE_OPERATOR = 2,
        //The cell purpose is to perform comparison on data inputs
        COMPARISON_OPERATOR = 3,
        //The cell purpose is to select among a set of possible data inputs
        MULTIPLEXER_OPERATOR = 4,
        //The cell purpose is to retain a value
        RETENTION_OPERATOR = 5,
        //The cell purpose is to reduce a vector to a scalar
        REDUCTION_OPERATOR = 6,
        //The cell purpose is to shift all the bits of inputs up to a defined value
        SHIFT_OPERATOR = 7, 
        //The operator perform a specific operation (which is not part of any other cathegory i.e. alu which can be both logical, mathematical, ...) on data inputs
        DEDICATED_OPERATOR = 9,
    };

    /*
     * Structure that represent an element(i.e. node) of the circuit exploiting its components(i.e. ports) by functionalities.
     */
    class CellStructure
    {
        private:
            //Component type of the cell (Mux, Adder, ...)
            std::string type;
            //Used for keeping track of the pourpose of the cell
            CellCategory category;
            
            /*
             * Boolean flag used to decree if the purpose of the node is, normally, to change or not a signal applied to its data input.
             * signal_changer = true -> when a signal is applied to a data input port of the node, it will (probably) be changed somehow (so it's an opertional element)
             * signal_changer = false -> when a signal is applied to a data input port of the node, it will not be changed anyhow (so, it's a configuration, communication or retention element)
             */
            bool signal_changer;
            //List of data input ports associated to the cell
            std::vector<std::string> data_inputs;
            //List of selection input ports associated to the cell
            std::vector<std::string> selection_inputs;
            //List of output ports associated to the cell
            std::vector<std::string> outputs; 

            //Private setters
            void setType(std::string type);
            void setCategory(CellCategory category);
            void setSignalChanger(bool signal_changer);
            void setDataInputs(std::vector<std::string> data_inputs);
            void setSelectionInputs(std::vector<std::string> selection_inputs);
            void setOutputs(std::vector<std::string> outputs);

        public:
            CellStructure(std::string type, CellCategory category, bool signal_changer, std::vector<std::string> data_inputs,std::vector<std::string> selection_inputs, std::vector<std::string> outputs);

            std::string getType();
            CellCategory getCategory();
            bool getSignalChanger();
            std::vector<std::string> getDataInputs();
            std::vector<std::string> getSelectionInputs();
            std::vector<std::string> getOutputs();

            /*
             * Method used to check if a specific port is a data input port:
             * return true -> unknown_port is a data input port
             * return false -> unknown_port is not a data input port (it can be a selection input, output or no one of the three)
             */
            bool isDataInput(std::string unknown_port);

            /*
             * Method used to check if a specific port is a selection input port:
             * return true -> unknown_port is a selection input port
             * return false -> unknown_port is not a selection input port (it can be a data input, output or no one of the three)
             */
            bool isSelectionInput(std::string unknown_port);
            /*
             * Method used to check if a specific port is an output port:
             * return true -> unknown_port is an output port
             * return false -> unknown_port is not an output port (it can be a selection input, data input or no one of the three)
             */
            bool isOutput(std::string unknown_port);
            /*
             * Method used to decree is a cell is a Logical Operator cell
             * return true -> is a logical operator cell
             * return false -> is not a logical operator cell
             */
            bool isLogicalOperator();
            /*
             * Method used to decree is a cell is an arithmetic operator
             * return true -> is an arithmetic operator cell
             * return false -> is not an arithmetic operator cell
             */
            bool isArithmeticOperator();

            /*
             * Method used to decree is a cell is a Bitwise Operator cell
             * return true -> is a bitwise operator cell
             * return false -> is not a bitwise operator cell
             */
            bool isBitwiseOperator();

            /*
             * Method used to decree is a cell is a Comparison Operator cell
             * return true -> is a comparison operator cell
             * return false -> is not a comparison operator cell
             */
            bool isComparisonOperator();
            /*
             * Method used to decree is a cell is a Multiplexer Operator cell
             * return true -> is a multiplexer operator cell
             * return false -> is not a multiplexer operator cell
             */
            bool isMultiplexerOperator();

            /*
             * Method used to decree is a cell is a Retention Operator cell
             * return true -> is a retention operator cell
             * return false -> is not a retention operator cell
             */
            bool isRetentionOperator();

            /*
             * Method used to decree is a cell is a Reduction Operator
             * return true -> is a reduction operator cell
             * return false -> is not a reduction operator cell
             */
            bool isReductionOperator();

            /*
             * Method used to decree is a cell is a Shift Operator
             * return true -> is a shift operator cell
             * return false -> is not a shift operator cell
             */
            bool isShiftOperator();

            /*
             * Method used to decree is a cell is a Dedicated Operator cell
             * return true -> is a dedicated operator cell
             * return false -> is not a dedicated operator cell
             */
            bool isDedicatedOperator();

            /*
             *  toString() interface.
             */ 
            std::string toString();
    };

    class CellRepository
    {
        private:
            
            /*
             * Vector that contains all the CellStructures that Yoys can provide, explored in their structure
             */
            std::vector<CellStructure *> cells_repository ;
            /*
             * Empty vector used when a specifici cell doesn't have certain ports (like data inputs or selection inputs)
             */
            std::vector<std::string> void_vector;

            /*
             *  Method used to define and store some standard unary operator cell structures
             */
            void setUnaryOperatorCellStructure(std::string name, CellCategory category, bool signal_changer);

            /*
             *  Method used to define and store some standard binary operator cell structures
             */
            void setBinaryOperatorCellStructure(std::string name, CellCategory category, bool signal_changer);

            /*
             *  Method used to define and store some standard mux cell structures
             */
            void setMuxCellStructure(std::string name);

            /*
             * Method used to define and store all the cell structures provided by Yosys according to their structure and functionality
             */
            void buildCellRepository();

        public:
            CellRepository();
            //Method used for retriving the singleton instance of CellRepository
            static CellRepository *getInstance() ;

            std::vector<CellStructure *> getCellRepository(){ return this->cells_repository; }

            /*
             * Method that return a specific cell based on its cellType :
             * return nullptr -> No cell found with such name
             * return ! nullptr -> Cell requested
             */
            CellStructure *getCell(std::string cellType);

            virtual ~CellRepository()
            {
                for(auto s : this->getCellRepository()) delete s ;
            }
    };
}

#endif
