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

#ifndef IOSUPPORT_H
#define IOSUPPORT_H

#include <fstream>

namespace IOSupport
{
	/*
	 *	Opens FILENAME and write results in there.
	 */
	void writeResultToCleanFile(std::string filepath, std::string results);
	/*
	 *	Reset the content of a file.
	 */
	void resetFile(std::string filepath);

	/*
	 *	Append results to FILENAME.
	 */
	void appendToFile(std::string filepath, std::string results);
}

#endif