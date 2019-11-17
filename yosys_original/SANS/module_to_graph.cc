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

#include "module_to_graph.h"

/*
 *	Method belonging to the SubCircuit library. We include it here with slight modifications to be able to build the whole project as a Yosys' integrable module.
 *	Otherwise, each person that needs our functionalities will need to recompile Yosys, and like no, thanks..
 */
std::vector< std::pair< std::string, int > > ModToGraph::mod2graph(SubCircuit_v2::Graph &graph, RTLIL::Module *mod)
{
	std::vector< std::pair<std::string,int> > externalSignals;
	std::map<RTLIL::SigBit, bit_ref_t> sig_bit_ref;
	std::map<std::pair<RTLIL::Wire*, int>, int> sig_use_count;

	SigMap sigmap(mod);
	
	//Take each module's cell
	for ( auto &cell_it : mod->cells_ )
	{
		RTLIL::Cell *cell = cell_it.second;
		
		std::string type = cell->type.str();
		
		if (type.substr(0, 2) == "\\$")
		{
			type = type.substr(1);
		}
		
		//Create a node of the graph
		graph.createNode(cell->name.str(), type, (void*)cell);
		
		for ( auto &conn : cell->connections() )
		{
			graph.createPort(cell->name.str(), conn.first.str(), conn.second.size());
			
			RTLIL::SigSpec conn_sig = conn.second;
			sigmap.apply(conn_sig);

			for (int i = 0; i < conn_sig.size(); i++)
			{
				auto &bit = conn_sig[i];
				if (bit.wire == NULL)
				{
					graph.createConstant(cell->name.str(), conn.first.str(), i, int(bit.data));
					continue;
				}

				if (sig_bit_ref.count(bit) == 0)
				{
					bit_ref_t &bit_ref = sig_bit_ref[bit];
					bit_ref.cell = cell->name.str();
					bit_ref.port = conn.first.str();
					bit_ref.bit = i;
				}
				bit_ref_t &bit_ref = sig_bit_ref[bit];
				graph.createConnection(bit_ref.cell, bit_ref.port, bit_ref.bit, cell->name.str(), conn.first.str(), i);
			}
		}
	}

	//Mark external signals (used in module ports)
	for ( auto &wire_it : mod->wires_ )
	{
		RTLIL::Wire *wire = wire_it.second;
		if (wire->port_id > 0)
		{
			externalSignals.push_back( std::make_pair(wire->name.str(), wire->width ) );
			
			RTLIL::SigSpec conn_sig(wire);
			sigmap.apply(conn_sig);
			
			for (auto &bit : conn_sig)
			{
				if (sig_bit_ref.count(bit) != 0)
				{
					bit_ref_t &bit_ref = sig_bit_ref[bit];
					graph.markExtern(bit_ref.cell, bit_ref.port, bit_ref.bit);
				}
			}
		}
	}
	
	/*if(verbose)
	{
		graph.print();
	}
	//*/

	return externalSignals;
}
