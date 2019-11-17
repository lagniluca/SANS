`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: Cugini Lorenzo
// 
// Create Date: 28.03.2019 11:34:05
// Design Name: 
// Module Name: clock_divider
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

module clock_divider
(
    input clk_i     ,   //Input clock signal
    input rst_i     ,   //Reset signal
    output reg clk_o    //Output clock signal
);

//Internal representation
reg ss;
reg ss_n;

reg clk_r;

//Synchronous logic update
always @(posedge clk_i, posedge rst_i)
begin
    if(rst_i)
        begin
            ss <= 0;
            clk_o <= 0;
            clk_r <= 0;
        end
    else
        begin
            ss <= ss_n;
            clk_o <= clk_r;
        end
end

//Internal logic update
always @(*)
begin
    ss_n = ss;
    
    if(rst_i)
        clk_o = 0;
        
    else
    begin
        case(ss)
        1'b0:
            begin
                //In zero we negate the clock
                clk_r = ~clk_r;
                ss_n = 1'b1;
            end
    
         1'b1:
            begin
               //In one we do not perform any action
                ss_n = 1'b0;
            end
    
        endcase
    end
end

endmodule
