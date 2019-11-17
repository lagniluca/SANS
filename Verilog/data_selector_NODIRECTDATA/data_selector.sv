`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: Cugini Lorenzo
// 
// Create Date: 27.03.2019 11:51:24
// Design Name: 
// Module Name: data_selector
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


module data_selector
(
    input clk_i                             ,   //Clock signal
    input [7:0] data_i                      ,   //Data signal
    input /*reg*/ [1:0] adr_i               ,   //Address selector for the output
    input /*reg*/ read_sig_i                ,
    input /*reg*/ write_sig_i               ,
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
                    $display("Configuration not allowed.");
                    data_o = 11111111;
                end
    endcase
end

endmodule
