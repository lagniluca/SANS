`timescale 1ns / 1ns
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: Cugini Lorenzo
// 
// Create Date: 07.12.2018 14:07:13
// Design Name: 
// Module Name: ff_d
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


module ff_d #(parameter W=8)
(
    input clk_i       		,   	//Clock signal
    input rst_i      		,   	//Reset signal
    input [W-1:0] data_i     	,   	//Data signal
    output reg q_o        		//Output signal
);

always @(posedge clk_i)
begin
    if(rst_i)
        begin
            q_o <= 0;
        end
    else
        begin
            q_o <= data_i;
        end
end

endmodule