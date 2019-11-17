`timescale 1ns / 1ns
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: Cugini Lorenzo
// 
// Create Date: 09.12.2018 16:51:41
// Design Name: 
// Module Name: greatmodule
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


module greatmodule
(
    input clk_i         ,   //Clock signal
    input rst_i         ,   //Reset signal
    input en_i          ,   //Enable signal
    input a             ,
    input b             ,
    input c             ,
    output reg x        ,
    output reg y        ,
    output reg z
);

reg a1, a1_n;
reg a2, a2_n;
reg a3, a3_n;
reg x_r, y_r, z_r;


always @(posedge clk_i)
begin
    if(!en_i)
    begin
        //Module not enabled.
        x_r <= 0;
        y_r <= 0;
        z_r <= 0;
    end
    else
    begin
        a1 <= a1_n;
        a2 <= a2_n;
        a3 <= a3_n;
        
        x_r = a1;
        y_r = a2;
        z_r = a3;
    end
end

always @(*)
begin
    a1_n = a1;
    a2_n = a2;
    a3_n = a3;

    a1_n <= a || b;
    a2_n <= a && b;
    a3_n <= (a1 || a2) && ((a || b) || (a && b));
    
end

assign x = x_r;
assign y = y_r;
assign z = z_r;

endmodule
