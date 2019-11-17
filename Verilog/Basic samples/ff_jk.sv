`timescale 1ns / 1ns
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: Cugini Lorenzo
// 
// Create Date: 24.11.2018 11:30:43
// Design Name: 
// Module Name: memory
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

module ff_jk    (
    clk ,   //Clock
    j   ,   //J signal
    k   ,   //K signal
    en  ,   //Enable signal
    q   ,   //Output
    qbar   //Output
                );

//Inputs
input clk;
input j;
input k;
input en;
//Outputs
output reg q;
output reg qbar;

//Code starts here

always @(posedge clk)
begin
if(en)
    begin
        case({j,k})
        2'b00: q<=q;
        2'b01: q<=1'b0;
        2'b10: q<=1'b1;
        2'b11: q<=~q;
        endcase
    end
else
    q<=1'b0;

end

endmodule
