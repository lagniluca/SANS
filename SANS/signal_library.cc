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

#include<stdio.h>
#include "signal_library.h"
#include "kernel/yosys.h"

USING_YOSYS_NAMESPACE

Signal::Signal::Signal(std::string signalName, int MSB, int LSB) : signalName(signalName), dimensionPair(std::make_pair(MSB, LSB)) {}

std::string Signal::Signal::getSignalName() const { return this->signalName; }
std::pair<int, int> Signal::Signal::getDimensionPair() const { return this->dimensionPair; }
int Signal::Signal::getMSB() const { return this->dimensionPair.first; }
int Signal::Signal::getLSB() const { return this->dimensionPair.second; }

bool Signal::Signal::isEmpty()
{
	return ( this->signalName == EMPTYSIGNAL ) ? true : false;
}

int Signal::Signal::width()
{
	return (dimensionPair.first - dimensionPair.second) + 1;
}

std::string Signal::Signal::toString()
{
	std::stringstream ss;

	ss << signalName << " [" << dimensionPair.first << ":" << dimensionPair.second << "] ";

	return ss.str();
}

bool Signal::isIn( std::vector<Signal> haystack, Signal needle)
{
	bool isIn = false;

	for( auto signal : haystack )
	{
		//If it's in here, we don't check other elements
		if( needle == signal )
		{
			isIn = true;
			break;
		}
	}
	
	return isIn;
}

bool Signal::isIn_byNameOnly(std::vector<Signal> haystack, Signal needle)
{
	bool isIn = false;

	for( auto signal : haystack )
	{
		if( signal.getSignalName() == needle.getSignalName() )
		{
			isIn = true;
			break;
		}
	}

	return isIn;
}

std::vector<Signal::Signal> Signal::conditionalInsert_checkSignalName(std::vector<Signal> vector, Signal signal)
{
	bool isIn = false;
	for( auto sig : vector )
	{
		if( sig.getSignalName() == signal.getSignalName() )
		{
			isIn = true;
		}
	}

	if(!isIn)
	{
		vector.push_back(signal);
	}

	return vector;
}

bool Signal::compare(const Signal& sig_1, const Signal& sig_2)
{
	if( sig_1.getMSB() > sig_2.getMSB() )
	{
		return true;
	}
	else if( sig_1.getMSB() < sig_2.getMSB() )
	{
		return false;
	}
	else
	{
		if( sig_1.getLSB() > sig_2.getLSB() )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}


std::vector<Signal::Signal> Signal::sort(std::vector<Signal> vector)
{
	std::sort( vector.begin(), vector.end(), compare );
	return vector;
}


Signal::Signal Signal::fetchSignal(std::vector<Signal> vec, std::string sigName)
{
	for( auto signal : vec )
	{
		if( signal.getSignalName() == sigName )
		{
			return signal;
		}
	}

	Signal empty;
	return empty;
}