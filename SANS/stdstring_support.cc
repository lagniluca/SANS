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

#include "stdstring_support.h"

void StringSupport::findAndReplaceAll(std::string &data, std::string toSearch, std::string replaceStr)
{
	// Get the first occurrence
	size_t pos = data.find(toSearch);
 
	// Repeat till end is reached
	while( pos != std::string::npos)
	{
		// Replace this occurrence of Sub String
		data.replace(pos, toSearch.size(), replaceStr);
		// Get the next occurrence from the current position
		pos = data.find(toSearch, pos + replaceStr.size());
	}
}

std::string StringSupport::cleanse(std::string dirty, bool keepWhitespaces)
{
	dirty.erase( std::remove(dirty.begin(), dirty.end(), '\n'), dirty.end() );
	dirty.erase( std::remove(dirty.begin(), dirty.end(), '\t'), dirty.end() );
	dirty.erase( std::remove(dirty.begin(), dirty.end(), '\v'), dirty.end() );
	dirty.erase( std::remove(dirty.begin(), dirty.end(), '\0'), dirty.end() );
	findAndReplaceAll(dirty, "  ", "");
	
	if(!keepWhitespaces)
	{
		dirty.erase( std::remove(dirty.begin(), dirty.end(), ' '), dirty.end() );
	}
	
	return dirty;
}

std::vector<std::string> StringSupport::cleanse(std::vector<std::string> dirty)
{
	std::vector<std::string> temp;

	for( auto elem : dirty )
	{
		temp.push_back( cleanse(elem, true) );
	}

	return temp;
}

std::string StringSupport::substr_delimiter(std::string toFilter, std::string startDelimiter, std::string endDelimiter)
{
	const char* start = strstr( toFilter.c_str(), startDelimiter.c_str() );
	const char* end = strstr( toFilter.c_str(), endDelimiter.c_str() );

	//Both strings are substrings of toFilter
	if( start && end )
	{
		int start_ = start ? start - toFilter.c_str() : -1;
		int end_ = end ? end - toFilter.c_str() : -1;

		return toFilter.substr( start_, (end_ - start_ + (int)endDelimiter.length() ) );
	}	
	return EMPTY;
}

bool StringSupport::startsWith(std::string haystack, std::string needle)
{
	if( haystack.find(needle.c_str(), 0, (int) needle.length() ) == 0 )
	{
		return true;
	}
	return false;
}

std::vector<std::string> StringSupport::split(std::string input, char delimiter, bool deleteEmpty, bool keepWhitespaces)
{
	std::vector<std::string> temp;

	//Turn the string into a stream
	std::stringstream ss(input);
	std::string tok;

	while( std::getline(ss, tok, delimiter) )
	{
		if(deleteEmpty)
		{
			if( (int)cleanse(tok, keepWhitespaces).size() > 0)
			{
				temp.push_back(tok);
			}
		}
		else
		{
			temp.push_back(tok);
		}
	}

	if( (int)cleanse(tok).size() > 0)
	{
		temp.push_back(tok);
	}
	else
	{
		temp.push_back(tok);
	}

	return temp;
}

std::vector<std::string> StringSupport::split(std::string input, std::string delimiter, bool removeFirst, bool cleanse)
{
	std::vector<std::string> temp;

	int offset = 0;
	int newOffset = 0;
	int count = 0;
	while( input.find(delimiter, offset) != std::string::npos )
	{
		newOffset = input.find(delimiter, offset);
        
        std::string currentToken = "";
        
        if(offset == 0)
        {
            currentToken = input.substr( offset , newOffset-offset);
        }
        else
        {
            if(offset > 0)
            {
                offset = offset-1;
                
                currentToken = input.substr( offset , newOffset-offset);
            }
        }
		
		if(cleanse)
		{
			currentToken = StringSupport::cleanse(currentToken);
		}

		if( !currentToken.empty() )
		{
			if(removeFirst)
			{
				if(count>0)
				{
					temp.push_back( currentToken );
				}
			}
			else
			{
				temp.push_back( currentToken );	
			}
		}

		//The ++ is needed to avoid infinite cycles
		offset = ++newOffset;
		count++;
	}

	return temp;
}

std::pair<int, int> StringSupport::extractPair(std::string pair, bool verbose)
{
	std::stringstream ss;

	if(verbose)
	{
		ss << "Original : " << pair << "\n";
	}

	//Removing [
	pair.erase( std::remove(pair.begin(), pair.end(), DIMENSION_START_CHAR), pair.end() );

	if(verbose)
	{
		ss << "Removed " << DIMENSION_START << " : " << pair << "\n";
	}

	//Removing ]
	pair.erase( std::remove(pair.begin(), pair.end(), DIMENSION_END_CHAR), pair.end() );

	if(verbose)
	{
		ss << "Removed " << DIMENSION_END << " : " << pair << "\n";
	}

	std::vector<std::string> dimensions = split(pair, COLON_CHAR);

	if(verbose)
	{
		std::cout << ss.str() << std::endl;
	}

	return std::make_pair( std::atoi(dimensions[0].c_str()), std::atoi(dimensions[1].c_str()) );
}

std::pair<int, int> StringSupport::singleSignalExtraction(std::string signal)
{
	//Check if it contains dimensions
	std::string substring = substr_delimiter(signal, DIMENSION_START, DIMENSION_END);
	if( substring.length() > 0 )
	{	
		//NB non si è mai verificata mentre testavo, se leggete questa stringa sistemate		
		return extractPair(substring);
	}
	else
	{
		return std::make_pair(0,0);
	}
}

std::vector< std::pair< std::string, std::pair<int, int> > > StringSupport::extractSignals(std::string toSplit, bool verbose)
{
	// vector of < signal name, <startingbit, endingbit> >
	std::vector< std::pair< std::string, std::pair<int, int> > > signalsSpec;
	std::stringstream ss;

	if(verbose)
	{
		ss << "\nextractSignals for : " << toSplit << "\n";
	}

	std::vector<std::string> splitted = split(toSplit);
	std::string support_one;
	std::string support_two;

	//It's 2 and not 1 because the split function also includes the last bit of the splitted string
	if( (int)splitted.size() == 2)
	{
		support_one = splitted[0];

		if(verbose)
		{
			ss << "\tOnly one signal : " << support_one;
		}

		//If it's only one signal overall
		if( startsWith(support_one, "\\") )
		{
			signalsSpec.push_back( std::make_pair(support_one, singleSignalExtraction(support_one) ) );
		}

		ss << "\n";
	}
	else
	{
		for( int pos=0; pos < (int)splitted.size()-1; pos++ )
		{
			//Empty supports to avoid collateral damage
			support_one.empty();
			support_two.empty();
			
			//Assign to current cycle data
			support_one = splitted[pos];
			support_two = splitted[pos+1];
			
			if(verbose)
			{
				ss << "\tMore than one : -" << support_one;
			}

			//The first signal is a pure signal (starts with \)
			if( startsWith(support_one, "\\") )
			{
				if(verbose)
				{
					ss <<"\t has \\";
				}

				//The second signal is an indication of dimension	
				if( startsWith(support_two, DIMENSION_START) )
				{
					if(verbose)
					{
						ss << " and [\n";
					}
					signalsSpec.push_back( std::make_pair(support_one, extractPair(support_two) ) );
				}
				else
				{
					if(verbose)
					{
						ss << " but not [\n";
					}
					signalsSpec.push_back( std::make_pair(support_one, singleSignalExtraction(support_one) ) );
				}
			}
			else
			{
				if(verbose)
				{
					ss << "\t\t\t-----> " << support_one << "\n";
				}
			}
			ss << "\n";
		}

		//Check for the last element is needed, since we've exited the for loop one step before
		if( startsWith(support_two, "\\") )
		{
			signalsSpec.push_back( std::make_pair(support_two, singleSignalExtraction(support_two) ) );
		}
	}
	if(verbose)
	{
		std::cout << ss.str() << std::endl;
	}

	return signalsSpec;
}

bool StringSupport::isSingleBit( std::pair<  std::string, std::pair<int, int>  > signal )
{
	if( signal.second.first == signal.second.second )
	{
		return true;
	}
	return false;
}

bool StringSupport::isSingleBit( std::string signal )
{
	std::vector< std::pair< std::string, std::pair<int, int> > > temp = extractSignals(signal);

	//NB: call this only on single signals, this for is there just for consistency wrt the data structure. When we call this function we must pass only one signal!
	for( auto sig : temp )
	{
		if( sig.second.first == sig.second.second )
			return true;
	}
	return false;
}

std::string StringSupport::DUMP_vector(std::vector<std::string> vec, bool dividePrint)
{
	std::stringstream ss;

	if(dividePrint)
	{
		ss << "Dumping vector _______________________________________________________________\n";
	}

	for( auto elem : vec )
	{
		ss << "--> " << elem << "\n";
	}

	ss << "\n";

	return ss.str();
}

std::string StringSupport::DUMP_vector(std::vector< std::pair<std::string, std::string> > vec, bool dividePrint)
{
	std::stringstream ss;

	if(dividePrint)
	{
		ss << "Dumping vector _______________________________________________________________\n";
	}

	for( auto pair : vec )
	{
		ss << pair.first << "  -  " << pair.second << "\n";
	}

	ss << "\n";

	return ss.str();
}