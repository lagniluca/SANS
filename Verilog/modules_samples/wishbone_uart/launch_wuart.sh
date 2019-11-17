#!/bin/bash

yosys-config --build pass.so pass.cc circuit.cc cellrepo_cellrepository.cc cellrepo_cellstructure.cc design_analyzer.cc dfs.cc generator_analyzer.cc generator_library.cc module_to_graph.cc node_library.cc port_library.cc rtlil_dumper.cc signal_library.cc stdio_support.cc stdstring_support.cc supportGraph.cc 
yosys -m pass.so -p "read_verilog rxuart.v rxuartlite.v txuart.v txuartlite.v ufifo.v wbuart.v ; proc; sans; show wbuart"
