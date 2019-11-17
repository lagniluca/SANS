`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: Cugini Lorenzo
// 
// Create Date: 10.12.2018 16:44:53
// Design Name: 
// Module Name: fsm
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


module fsm
(
    input clk,
    input rst,
    input [1:0] in,
    output [2:0]out
);

reg [2:0] ss;
reg [2:0] ss_n;
reg [2:0] out_r;

localparam A = 3'b000, B = 3'b001, C = 3'b010, D = 3'b011, E = 3'b100;

always @(posedge clk, posedge rst)
begin
    if(rst)
    begin
        ss <= A;
        out_r <= 3'bx;
    end
    else
    begin
        ss <= ss_n;
    end
end

always @(*)
begin
    out_r = 3'b0;
    ss_n = ss;
    
    case(ss)
    
    A:
    begin
        out_r = 3'b0;
        case (in)
            2'b00: ss_n = B;
            2'b01: ss_n = A;
            2'b10: ss_n = C;
            2'b11: ss_n = C;
        endcase
    end
    
    B:
    begin
        out_r = 'd1;
        case (in)
            2'b00: ss_n = D;
            2'b01: ss_n = B;
            2'b10: ss_n = C;
            2'b11: ss_n = A;
        endcase
    end
    
    C:
    begin
        out_r = 'd5;
        case (in)
            2'b00: ss_n = E;
            2'b01: ss_n = C;
            2'b10: ss_n = C;
            2'b11: ss_n = D;
       endcase
    end
    
    D:
    begin
        out_r = 'd6;
        case (in)
            2'b00: ss_n = B;
            2'b01: ss_n = D;
            2'b10: ss_n = D;
            2'b11: ss_n = C;
        endcase
    end
    
    E:
    begin
        out_r = 'd7;
    end
    
    endcase
    
end

assign out = out_r;


endmodule