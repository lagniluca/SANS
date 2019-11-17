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
 
#ifndef VECSUPPORT_H
#define VECSUPPORT_H

#include <algorithm>
#include <iostream>
#include <vector>

namespace VectorSupport
{
	/*
	 *	Safely concatenate two vectors.
	 */
	template<typename T>
	std::vector<T> append(std::vector<T> source, std::vector<T> toAppend)
	{
		source.insert( std::end(source), std::begin(toAppend), std::end(toAppend) );
		return source;
	}

	/*
	 *	Linear search in a vector.
	 */
	template<typename T>
	bool isIn(std::vector<T> haystack, T needle)
	{
		//The found position is not the end of the array means that somewhere the needle has been found.
		if( std::find( std::begin(haystack), std::end(haystack), needle) != std::end(haystack) )
		{
			return true;
		}
		return false;
	}

	/*
	 *	Insert with no repetitions.
	 */
	template<typename T>
	std::vector<T> conditionalInsert(std::vector<T> vector, T toInsert)
	{
		if( !isIn(vector, toInsert) )
		{
			vector.push_back(toInsert);
		}

		return vector;
	}

	/*
	 *	Merge the given vectors with no repetitions.
	 */
	template<typename T>
	std::vector<T> merge(std::vector<T> left, std::vector<T> right)
	{
		for( auto elem : right )
		{
			left = conditionalInsert(left, elem);
		}

		return left;
	}

	/*
	 *	Returns the vector of elements which are in the first but not in the second vector. Like set difference.
	 */
	template<typename T>
	std::vector<T> minus(std::vector<T> inHere, std::vector<T> notInHere)
	{
		std::vector<T> temp;

		//Elem in first vector
		for( auto elem : inHere )
		{
			//Is not in second vector
			if( !isIn(notInHere, elem) )
			{
				temp.push_back(elem);
			}
		}

		return temp;
	}

	/*
	 *	Checks if two vectors are equal.
	 */
	template<typename T>
	bool equals(std::vector<T> first, std::vector<T> second)
	{
		return 	( (minus(first, second).empty()) && (minus(second, first).empty()) ) ? true : false;
	}

	/*
	 *	Delete n-th element. NB: first element is n=0, this ain't MATLAB
	 */
	template<typename T>
	void pop_n( std::vector<T>& vec, int n=0)
	{
		if(n<0)
		{
			std::cout << "Cannot delete at position < 0.\n";
		}
		else
		{
			vec.erase( vec.begin() + n );
		}
		
	}
}

#endif