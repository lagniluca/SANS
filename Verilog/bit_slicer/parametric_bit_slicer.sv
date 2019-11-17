`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: Cugini Lorenzo
// 
// Create Date: 03.04.2019 13:30:50
// Design Name: 
// Module Name: parametric_bit_slicer
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


module parametric_bit_slicer
(
    input clk_i                 ,   //Clock signal
    input rst_i                 ,   //Reset signal
    input sel_i                 ,   //Selection signal
    input [31:0] data_i         ,   //Input signal
    output reg [15:0] data_o    ,
    output reg [15:0] selected
);

localparam slice_max_one = 15;
localparam slice_min_one = 0;


always @(posedge clk_i)
begin
    if(rst_i)
    begin
        data_o <= 0;
        selected = 0;
    end
    else
    begin
        data_o <= data_i[15:0];
        
        if(sel_i)
        begin
            selected <= data_i[slice_max_one: slice_min_one];
        end
        else
        begin
            selected <= data_i[slice_max_one+16-1: slice_min_one+16-1 ];
        end
    end
end

endmodule
