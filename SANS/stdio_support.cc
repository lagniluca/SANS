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

#include "stdio_support.h"

void IOSupport::writeResultToCleanFile(std::string filepath, std::string results)
{
	std::ofstream outfile;

	//Open the stream for writing (out) and truncate its content (trunc)
	outfile.open(filepath, std::ios::out | std::ios::trunc);

	//Write results in there.
	outfile << results << std::endl;

	//Close the stream.
	outfile.close();
}

void IOSupport::resetFile(std::string filepath)
{
	std::ofstream file;

	file.open(filepath, std::ios::trunc);

	file.close();
}

void IOSupport::appendToFile(std::string filepath, std::string results)
{
	std::ofstream outfile;

	outfile.open(filepath, std::ios::app);

	outfile << results << std::endl;

	outfile.close();
}