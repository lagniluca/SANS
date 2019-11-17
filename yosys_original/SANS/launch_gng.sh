#!/bin/bash

yosys-config --build pass.so pass.cc circuit.cc cellrepo_cellrepository.cc cellrepo_cellstructure.cc design_analyzer.cc dfs.cc generator_analyzer.cc generator_library.cc module_to_graph.cc node_library.cc port_library.cc rtlil_dumper.cc signal_library.cc stdio_support.cc stdstring_support.cc supportGraph.cc 
yosys -m pass.so -p "read_verilog gng.v gng_coef.v gng_ctg.v gng_interp.v gng_lzd.v gng_smul_16_18.v gng_smul_16_18_sadd_37.v; proc; sans; show gng"
