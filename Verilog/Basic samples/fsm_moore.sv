/*
 *==============================[FSM MOORE]==============================
 * 
 * This is a System Verilog example of a Finite State Machine (FSM) .
 * 
 * Here a user insert some coins (TEN or TWENTY) and , when he reches the 
 * amount of 30 (THIRTY) coins , the machine gives to him its product.
 * The machine doesn't give any change.
 *
 * Example provided by Prof.Zoni
 * 
 */

module fsm(
	   //Input signals 
	   input  clock,     //Clock (input) signal
	   input  reset,     //Reset (input) signal
	   input  ten_in,    //Input of ten coins signal
	   input  twenty_in, //Input of twenty coins signal
	   //Output signals
	   output open_out   //Output signal
	   );

   reg [1:0] 	  ss;
   localparam IDLE=2'b00, TEN=2'b01, TWENTY=2'b10, OPEN_CMD=2'b11;

   always@(posedge clock)
     begin
	if(reset) ss<=0;
	else
	  begin
	     case(ss)
	       IDLE:
		 begin
		    if(ten_in) ss<=TEN;
		    else if(twenty_in) ss<=TWENTY;
		    else ss<=IDLE;
		 end
	       TEN:
		 begin
		    if(ten_in) ss<=TWENTY;
		    else if(twenty_in) ss<=OPEN_CMD;
		    else ss<=TEN;
		    
		 end
	       TWENTY:
		 begin
		    if(ten_in) ss<=OPEN_CMD;
		    else if(twenty_i) ss<=OPEN_CMD;
		    else ss<=TWENTY;
		 end
	       OPEN_CMD: ss<=IDLE;
	     endcase // case (ss)
	  end // else: !if(reset)
     end // always@ (posedge clock)

   assign open_out = (ss==OPEN_CMD);

endmodule // fsm


		    
