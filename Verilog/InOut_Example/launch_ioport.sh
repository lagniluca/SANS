#!/bin/bash

yosys-config --build pass.so pass.cc circuit.cc
yosys -m pass.so -p "read_verilog Configuration.v ioPort.v; proc; sans; show ioPort"
