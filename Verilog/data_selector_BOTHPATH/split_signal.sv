`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: Cugini Lorenzo
// 
// Create Date: 27.03.2019 18:02:22
// Design Name: 
// Module Name: split_signal
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


module split_signal
(
    input clk_i                     ,   //Clock signal
    input [7:0] data_i              ,   //Data signal
    input [1:0] adr_i               ,   //Address selector for the output
    input read_sig_i                ,
    input write_sig_i               ,
                                            /*  { read_sig_i, write_sig_i }
                                                00 : data_o = 0
                                                01 : reading at address [adr]
                                                10 : writing at address [adr] data_i
                                                11 : data_o = 1
                                             */
    output reg [7:0] data_o
);

//Internal memory
reg [7:0] cell_1;
reg [7:0] cell_2;
reg [7:0] cell_3;
reg [7:0] cell_4;

always @(posedge clk_i)
begin
    case({read_sig_i, write_sig_i})
    2'b00   :   data_o = 0;
    2'b01   :   begin
                    case(adr_i)
                        2'b00   :   data_o = cell_1;
                        2'b01   :   data_o = cell_2;
                        2'b10   :   data_o = cell_3;
                        2'b11   :   data_o = cell_4;
                    endcase
                end
    2'b10   :   begin
                    data_o = 0;
                    
                    case(adr_i)
                        2'b00   :   cell_1 = data_i;
                        2'b01   :   cell_2 = data_i;
                        2'b10   :   cell_3 = data_i;
                        2'b11   :   cell_4 = data_i;
                    endcase
                end
    2'b11   :   begin
                    //Direct Data - allowed path
                    if(data_i)
                        data_o = data_i;
                    else
                        data_o = 64;
                end
    endcase
end

endmodule
