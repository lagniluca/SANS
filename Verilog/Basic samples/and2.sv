`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: Cugini Lorenzo
// 
// Create Date: 21.10.2018 15:08:55
// Design Name: 
// Module Name: and2
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module and2(
a   ,   //First input
b   ,   //Second input
y       //AND result
            );
//Input ports
input a;
input b;
//Output ports
output wire y;

//Code starts here
and a1 (y, a, b);
endmodule