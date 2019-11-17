`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: Cugini Lorenzo
// 
// Create Date: 09.04.2019 09:522:24
// Design Name: 
// Module Name: bit_slicer_v2
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
    output reg [3:0] selected
);

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
        data_o <= data_i[31:16];
        
        if(sel_i)
        begin
			if(data_i[4:0])
			begin
            	selected <= data_i[4:1];
			end
			else
			begin
				selected <= data_i[12:9];
			end
        end
        else
        begin
            selected <= data_i[5:1];
        end
    end
end

endmodule
