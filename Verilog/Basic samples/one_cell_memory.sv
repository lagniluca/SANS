`timescale 1ns / 1ns
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: Cugini Lorenzo
// 
// Create Date: 24.11.2018 15:36:59
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


module memory(
input clk,                  //Clock signal
input read_sig,             //1: allow read
input write_sig,            //0: allow write
//input reg [31:0] addr,      //Address
input [3:0] dataIn,    //Data written (if any)
output reg [3:0] dataOut   //Data read (if any)
    );

//The actual memory
reg [3:0] mem;

always @(posedge clk)
begin
    case({read_sig, write_sig})
    2'b00   :   dataOut = 0;
    2'b01   :   mem = dataIn;
    2'b10   :   dataOut = mem;
    2'b11   :   begin
                    $display("Configuration not allowed.");
                    dataOut = 0;
                end
    endcase
end

endmodule