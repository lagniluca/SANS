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

#ifndef SIGNAL_LIBRARY_H
#define SIGNAL_LIBRARY_H

#include<algorithm>
#include<string>
#include<vector>

#include "kernel/yosys.h"

namespace Signal
{
	#define EMPTYSIGNAL "Empty"

	/*
	 *	Struct representing a signal
	 */
	class Signal
	{
		private:
			//Name of the signal
			std::string signalName;
			//[MSB:LSB]
			std::pair<int, int> dimensionPair;

		public:
			//Constructor
			Signal() : signalName(EMPTYSIGNAL) {}
			Signal(std::string signalName, int MSB, int LSB);

			//Getters
			std::string getSignalName() const;
			std::pair<int, int> getDimensionPair() const;
			int getMSB() const;
			int getLSB() const;

			/*
			 *	Is the name EMPTYSIGNAL?
			 */
			bool isEmpty();

			/*
			 *	Return the width of the signal
			 */
			int width();

			/*
			 *	toString() interface
			 */
			std::string toString();

			/*
			 *	Comparison between signals
			 */
			friend bool operator==(const Signal& sig_1, const Signal& sig_2)
			{
				if 	(	(sig_1.getSignalName() == sig_2.getSignalName())
							&&
						(sig_1.getLSB() == sig_2.getLSB())
							&&
						(sig_1.getMSB() == sig_2.getMSB())
					)
				{
					return true;
				}
				return false;
			}
	};

	/*
	 *	Checks if the given signal sig is in list
	 */
	bool isIn( std::vector<Signal> haystack, Signal needle);

	/*
	 *	IsIn but checks only the signal name
	 */
	bool isIn_byNameOnly(std::vector<Signal> haystack, Signal needle);

	/*
	 *	Conditionally insert a signal in the vector if name matches
	 */
	std::vector<Signal> conditionalInsert_checkSignalName(std::vector<Signal> vector, Signal signal);

	/*
	 *	Comparison between signal. This is used for the sort algorithm during presentation.
	 *	True : sig_1 > sig_2
	 */
	bool compare(const Signal& sig_1, const Signal& sig_2);

	/*
	 *	Sort vector of signal.
	 */
	std::vector<Signal> sort(std::vector<Signal> vector);

	/*
	 *	Fetch signal with passed name.
	 */
	Signal fetchSignal(std::vector<Signal> vec, std::string sigName);
}

#endif