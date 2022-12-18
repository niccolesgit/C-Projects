module Top(clk, rst, enable, upDown, count, seg7);
// count[3:0] output included for convenience in debug and testbenches
	input clk, rst, enable, upDown;
	output [3:0] count;
	output [6:0] seg7;
	
   wire clkCounter;  // divided clock for Counter module
	ClkDiv cd (clk, clkCounter);
	Count4 c4 (clkCounter, rst, enable, upDown, count);
	Seg7Decode sd7 (count, seg7);
endmodule
