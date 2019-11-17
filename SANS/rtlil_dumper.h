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

/*
 * Set of functions used for exploiting connection between RTLIL::Cells or RTLIL::Modules
 */
#ifndef RTLILDUMPER_H
#define RTLILDUMPER_H

//Standard C++11 libraries
#include <string>
#include <iostream>
#include <stdexcept>

//Yosys libraries
#include "kernel/yosys.h"

//Application libraries
#include "signal_library.h"

USING_YOSYS_NAMESPACE

namespace RTLIL_DUMPER
{
    /*
     * Method used for checking if a string can be a portID
     */
    bool isValidPortID(std::string portID);
    
    /*
     * Method used for converting a string (Which represent an RTLIL::IdString of a port) into an integer
     */
    std::pair<bool, int> portIDDumper(std::string portID);
    
    /*
     * Method used for converting an RTLIL::SigChunk associated to a signal to
     * a Signal
     *
     * The first value of the pair (bool) is used to state if it was possible to convert
     * the Chunk to a Signal
     * 
     * result.first = true -> result.second = Signal
     * result.first = false -> result.second = void signal -> no solution found
     */
    std::pair<bool,Signal::Signal> sigChunkDumper(const RTLIL::SigChunk &chunk);
    
    /*
     * Method used for converting an RTLIL::SigSpec into a vector of Signals (i.e. the chunks associated to the SigSpec)
     * 
     * the first boolean flag of the pair states if we have reached a solution or (as in case of all constants) not.
     */
    std::pair<bool, std::vector<Signal::Signal>> sigSpecDumper(const RTLIL::SigSpec &sig);
    
    /*
     * Method used for converting an a connection among two RTLIL::Cell or modules into a pair of vectors that represents 
     * the chunks associated to such connections.
     * 
     * Since the connections are based on (paired) wires , we will always have that the two vectors are equals since all
     * the signals are related
     */
    std::pair<std::vector<Signal::Signal>,std::vector<Signal::Signal>> connDumper(const RTLIL::SigSpec &left,const RTLIL::SigSpec &right);
}

#endif