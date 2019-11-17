`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: Cugini Lorenzo
// 
// Create Date: 28.03.2019 16:56:45
// Design Name: 
// Module Name: tb_clock_multiplier
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

module tb_clock_multiplier();

reg clk_i;
reg rst_i;
reg selector_i;
wire clk_o;
wire [7:0] res_o;
//For internal representation
reg ss;

clock_multiplier cm ( .clk_i(clk_i), .rst_i(rst_i), .selector_i(selector_i), .clk_o(clk_o), .res_o(res_o) );

always #5 clk_i = ~clk_i;

assign ss = tb_clock_multiplier.cm.ss;

initial
begin
    clk_i <= 0;
    rst_i = 1;
    selector_i <= 0;
    
    @(posedge clk_i);
    @(posedge clk_i);
    
    rst_i = 0;
    
    @(posedge clk_i);
    @(posedge clk_i);
    @(posedge clk_i);
    
    selector_i <= 1;
    
    @(posedge clk_i);
    @(posedge clk_i);
    @(posedge clk_i);
    @(posedge clk_i);
     $finish;
end

endmodule