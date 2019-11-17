`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: Cugini Lorenzo
// 
// Create Date: 03.04.2019 11:56:59
// Design Name: 
// Module Name: bit_slicer
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


module bit_slicer
(
    input clk_i                 ,   //Clock signal
    input rst_i                 ,   //Reset signal
    input sel_i                 ,   //Selection signal
    input [31:0] data_i         ,   //Input signal
    output reg [15:0] data_o    ,
    output reg [15:0] selected
);

//Internal representation
//reg ss, ss_n;

always @(posedge clk_i)
begin
    if(rst_i)
    begin
        //ss <= 0;
        data_o <= 0;
        selected = 0;
    end
    else
    begin
        data_o <= data_i[15:0];
        
        if(sel_i)
        begin
            selected <= data_i[15:0];
        end
        else
        begin
            selected <= data_i[31:16];
        end
    end
end

endmodule
