`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: Cugini Lorenzo
// 
// Create Date: 27.03.2019 12:30:19
// Design Name: 
// Module Name: tb_data_selector
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


module tb_data_selector();

reg clk_i;
reg [7:0] data_i;
reg [1:0] adr_i;
reg read_sig_i;
reg write_sig_i;
wire [7:0] data_o;

reg [7:0] cell_1_r;
reg [7:0] cell_2_r;
reg [7:0] cell_3_r;
reg [7:0] cell_4_r;

data_selector ds    (
                        .clk_i(clk_i),
                        .data_i(data_i),
                        .adr_i(adr_i),
                        .read_sig_i(read_sig_i),
                        .write_sig_i(write_sig_i),
                        .data_o(data_o)
                     );

//Content of internal memory
assign cell_1_r = tb_data_selector.ds.cell_1;
assign cell_2_r = tb_data_selector.ds.cell_2;
assign cell_3_r = tb_data_selector.ds.cell_3;
assign cell_4_r = tb_data_selector.ds.cell_4;
               
always #10 clk_i = ~clk_i;

initial
begin
    data_i = 0;
    adr_i = 0;
    read_sig_i <= 0;
    write_sig_i <= 0;
    clk_i <= 0;
    
    @(posedge clk_i);
    @(posedge clk_i);
    
    /*
        Writing in memory test
    */
    data_i = 64;
    adr_i = 2'b00;
    read_sig_i = 1;
    @(posedge clk_i);
    data_i = 32;
    adr_i = 1;
    @(posedge clk_i);
    data_i = 2;
    adr_i = 2;
    @(posedge clk_i);
    data_i = 1;
    adr_i = 3;
    @(posedge clk_i);
    read_sig_i = 0;
    @(posedge clk_i);
    @(posedge clk_i);
    
    /*
        Reading results in memory
    */
    adr_i = 1;
    write_sig_i = 1;
    @(posedge clk_i);
    @(posedge clk_i);
    @(posedge clk_i);
    
    $finish;
end
                 
endmodule
