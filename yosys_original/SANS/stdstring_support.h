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

#ifndef STRSUPPORT_H
#define STRSUPPORT_H

#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <vector>

#define DIMENSION_START "["
#define DIMENSION_END "]"
#define EMPTY ""

#define DIMENSION_START_CHAR '['
#define DIMENSION_END_CHAR ']'
#define COLON_CHAR ':'
#define WHITESPACE_CHAR ' '

namespace StringSupport
{
	/*
	 *	Replaces all the occurences of toSearch with replaceStr
	 */
	void findAndReplaceAll(std::string &data, std::string toSearch, std::string replaceStr);

	/*
	 *	Cleanse the string for signal renaming analysis.
	 */
	std::string cleanse(std::string dirty, bool keepWhitespaces=false);

	/*
	 *	Cleanse the whole vector content.
	 */
	std::vector<std::string> cleanse(std::vector<std::string> dirty);

	/*
	 *	Returns the substring of toFilter between startDelimiter and endDelimiter
	 */
	std::string substr_delimiter(std::string toFilter, std::string startDelimiter = DIMENSION_START, std::string endDelimiter = DIMENSION_END);

	/*
	 *	startsWith method
	 */
	bool startsWith(std::string haystack, std::string needle);

	/*
	 *	Split with char delimiter
	 */
	std::vector<std::string> split(std::string input, char delimiter = WHITESPACE_CHAR, bool deleteEmpty=false, bool keepWhitespaces=false);

	/*
	 *	Split with std::string delimiter. The removeFirst option is only to speed up a particular method.
	 */
	std::vector<std::string> split(std::string input, std::string delimiter, bool removeFirst=false, bool cleanse=false);

	/*
	 *	Takes a vector of strings and returns them in couples.
	 */
	/*
	std::vector< std::pair<std::string, std::string> > couple(std::vector<std::string> toCouple)
	{
		std::vector< std::pair<std::string, std::string> > temp;

		if( (int)toCouple.size() % 2 == 0)
		{
			for(int i=0; i < (int)toCouple.size()-1; i=i+2)
			{
				temp.push_back( std::make_pair(toCouple[i], toCouple[i+1]) );
			}
		}
		return temp;
	}
	*/

	/*
	 *	From the string of the shape [x,y] return x and y within the std::pair
	 */
	std::pair<int, int> extractPair(std::string pair, bool verbose=false);

	/*
	 *	Extract all the info about a signal
	 */
	std::pair<int, int> singleSignalExtraction(std::string signal);

	/*
	 *	Given a bunch of signal informations, return them in an organized way, highlithing both name and used bit of that signal. Dual function of recompact()
	 *	It ignores inner signals (starting with the dollar sign $)
	 */
	std::vector< std::pair< std::string, std::pair<int, int> > > extractSignals(std::string toSplit, bool verbose=false);

	/*
	 *	Recompact the given inputs into one single string. Dual function of extractSignals()
	 */
	/*
	std::string recompact(std::pair<  std::string, std::pair<int, int>  > toCompact)
	{
		std::stringstream ss;

		ss << toCompact.first;
		//NB: do not modify the format of this string, it will result in this function not being the dual of extractSignals() anymore
		ss << " [" << toCompact.second.first << ":" << toCompact.second.second << "]";

		return ss.str();
	}
	*/

	/*
	 *	Is a single bit signal? <for pair>
	 */
	bool isSingleBit( std::pair<  std::string, std::pair<int, int>  > signal );

	/*
	 *	Is a single bit signal? <for string>
	 */
	bool isSingleBit( std::string signal );

	/*
	 *	Returns a string representation of the string vector.
	 */
	std::string DUMP_vector(std::vector<std::string> vec, bool dividePrint=false);

	/*
	 *	Returns a string representation of the string vector.
	 */
	std::string DUMP_vector(std::vector< std::pair<std::string, std::string> > vec, bool dividePrint=false);
}

#endif