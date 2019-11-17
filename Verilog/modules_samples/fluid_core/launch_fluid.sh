#!/bin/bash

yosys-config --build pass.so pass.cc circuit.cc cellrepo_cellrepository.cc cellrepo_cellstructure.cc design_analyzer.cc dfs.cc generator_analyzer.cc generator_library.cc module_to_graph.cc node_library.cc port_library.cc rtlil_dumper.cc signal_library.cc stdio_support.cc stdstring_support.cc supportGraph.cc 
yosys -m pass.so -p "read_verilog Configuration.v data_mem.v EX_Stage.v FluidCore.v ID_Stage.v IF_Stage.v Inst_Mem.v int_ALU.v interrupt_unit.v ioPort.v MEM_Stage.v P_Reg.v Programming.v Reg_File.v Reg_Hist.v Shifter.v Staller.v uOP_Store.v WB_Stage.v; proc; sans; show ioPort"
