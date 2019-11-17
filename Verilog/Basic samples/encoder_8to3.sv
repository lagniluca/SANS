`timescale 1ns / 1ns
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: Cugini Lorenzo 
// 
// Create Date: 22.10.2018 22:27:15
// Design Name: 
// Module Name: encoder_8to3
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


module encoder_8to3(
    input [7:0] in,
    output reg [2:0] out
    );

//Code starts here
always @(in)
begin: ENCODER_8to3
    if(in[0]+in[1]+in[2]+in[3]+in[4]+in[5]+in[6]+in[7] >= 'd2)
        begin
        $display("Input error: number of 1s is greater than one");
        out = 3'd0;
        end
    else
        begin
            case({in[7],in[6],in[5],in[4],in[3],in[2],in[1],in[0]})
                8'b00000000 :   out = 3'd0;
                8'b00000001 :   out = 3'd1;
                8'b00000010 :   out = 3'd2;
                8'b00000100 :   out = 3'd3;
                8'b00001000 :   out = 3'd4;
                8'b00010000 :   out = 3'd5;
                8'b00100000 :   out = 3'd6;
                8'b01000000 :   out = 3'd7;
                8'b10000000 :   out = 3'd8;
            endcase
        end
end

endmodule