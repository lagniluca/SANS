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
 *  Set of functions used for exploiting connection between RTLIL::Cells or RTLIL::Modules
 */

//Standard C++11 libraries
#include <string>
#include <iostream>
#include <stdexcept>

//Yosys libraries
#include "kernel/yosys.h"
#include "rtlil_dumper.h"

//Application libraries
#include "stdvector_support.h"

bool RTLIL_DUMPER::isValidPortID(std::string portID)
{
    if(portID.size() < 2)
    {
        return false;
    }
    
    std::string trimmed = portID.substr(1);
    
    try
    {
        std::stoi(trimmed);
    }
    catch(std::invalid_argument& e)
    {
        return false;
    }
    
    return true;
}

std::pair<bool, int> RTLIL_DUMPER::portIDDumper(std::string portID)
{
    std::pair<bool, int> res;
    bool done = false;
    int value = -1;
    
    if(isValidPortID(portID))
    {
        std::string trimmed = portID.substr(1);
        
        try
        {
            done = true;
            value = std::stoi(trimmed);
        }
        catch(std::invalid_argument& e)
        {
            done = false;
            value = -1;
        }
            
    }
    
    res = std::make_pair(done, value);
    
    return res;
}

std::pair<bool,Signal::Signal> RTLIL_DUMPER::sigChunkDumper(const RTLIL::SigChunk &chunk)
{
    
    std::pair<bool, Signal::Signal> result;
    
    //Since we are looking for signals, in case of a costant we simply avoid to dump the chunk
    if(chunk.wire == NULL)
    {
        Signal::Signal sig;
        
        result = std::make_pair(false, sig);
    }
    else
    {
        Signal::Signal sig(
            chunk.wire->name.str(),
            //MSB
            (chunk.width + chunk.offset -1),
            //LSB
            chunk.offset
          );
        
        result = std::make_pair(true, sig);                            
    }
    
    return result;
}

std::pair<bool, std::vector<Signal::Signal>> RTLIL_DUMPER::sigSpecDumper(const RTLIL::SigSpec &sig)
{
    std::pair<bool, std::vector<Signal::Signal>> result;
    std::vector<Signal::Signal> chunks;
    
    if(sig.is_chunk())
    {
        std::pair<bool, Signal::Signal> res = sigChunkDumper(sig.as_chunk());
        
        if(res.first)
        {
            chunks.push_back(res.second);
        }
        
        result = std::make_pair(res.first, chunks);
    }
    else
    {
        bool hasChunks = false ;
        
        for(auto it = sig.chunks().rbegin(); it != sig.chunks().rend(); it++)
        {
            std::pair<bool, Signal::Signal> res = sigChunkDumper(*it);
            
            if(res.first)
            {
                chunks.push_back(res.second);
                hasChunks = true;
            }
        }
        
        result = std::make_pair(hasChunks, chunks);
    }
    
    return result;
}

std::pair<std::vector<Signal::Signal>,std::vector<Signal::Signal>> RTLIL_DUMPER::connDumper(const RTLIL::SigSpec &left,const RTLIL::SigSpec &right)
{
    std::pair<std::vector<Signal::Signal>, std::vector<Signal::Signal>> result;
    
    std::pair<bool, std::vector<Signal::Signal>> tmp1 = sigSpecDumper(left);
    std::pair<bool, std::vector<Signal::Signal>> tmp2 = sigSpecDumper(right);
    
    result = std::make_pair(tmp1.second, tmp2.second);
    
    return result;
}