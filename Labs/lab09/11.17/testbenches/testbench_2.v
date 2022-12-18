`define assert(actual, expected, outputName, inputs, description, type) \
    $write("\nTIME: "); $write($realtime); \
    if (actual == expected) \
        $display("   PASSED:   "); \
    else begin \
        $display(" ** FAILED:   "); \
        if (description) $display("TEST: %s", description); \
    end \
    $write("%s = ", outputName, type, actual); \
    $write(", EXPECTED: ", type, expected); \
    if (inputs) $display(" WITH: %s", inputs);
// End of `assert macro.

// It looks like this testbench contains a "$finish" system task.
// Be aware that some simulation programs (e.g. Modelsim) will immediately close once the "$finish" system task is executed.
// If you find that your simulation program is closing unexpectedly, try removing any "$finish" system tasks in this file.

// Testbench for Opener module
`timescale 1 ns / 1 ns

module testbench();
                                        
reg clk,b,c,o,r,s;             // inputs   
wire d,u; wire [1:0] State; // outputs  

localparam [1:0]  Closed = 0, //b'00;   //state encodings
                  Opening = 1,   
                  Open = 2,    
                  Closing = 3;                 

Opener uut (clk,b,c,o,r,s,d,u,State);

initial clk = 0; 
 always begin  // Clk wave, period of 10
      #5  clk = ~clk;
 end

initial
 begin
  b = 0; c = 0; o = 0; s = 0; r = 1; //reset in middle of travel range
  #10  // t = 10
   r = 0;
 // assert u = 1, d = 0, State = Opening when reset, not open or closed
  `assert(u, 1'b1, "u","r=1, not Open or Closed","u = 1, reset not open or closed", "%b")
  `assert(d, 1'b0, "d","r=1, not Open or Closed","d = 0, reset", "%b")
  `assert(State, Opening, "State","r=1, not Open or Closed","State = Opening when reset and !c!o", "%b")
  #20  // t = 30
   b = 0; c = 0; o = 1; s = 0; r = 1; //reset when fully opened
  #10  // t = 40
   r = 0;
    // assert u = 0, d = 0, State = Open when reset and already opened
  `assert(u, 1'b0, "u","r=1, Opened","u = 0, reset when already opened", "%b")
  `assert(d, 1'b0, "d","r=1, Opened","d = 0, reset when already opened", "%b")
  `assert(State, Open, "State","r=1, not Open or Closed","State = Open when reset and o", "%b")
   #20  // t = 60
   b = 0; c = 1; o = 0; s = 0; r = 1; //reset when fully closed
   #10  // t = 70
   r = 0;
 // assert u = 0, d = 0, State = Open when reset and already closed
  `assert(u, 1'b0, "u","r=1, closed","u = 0, reset when already closed", "%b")
  `assert(d, 1'b0, "d","r=1, closed","d = 0, reset when already closed", "%b")
  `assert(State, Closed, "State","r=1, Closed","State = Closed when reset and c", "%b")
  #10  // t = 80
   r = 0;
   
   /////////////// test b when closed
  #10  // t = 90
   b = 1; c = 1; o = 0; s = 0; r = 0; 
  #10  // t = 100
   b = 0;
 // assert u = 1, d = 0, State = Opening 
  `assert(u, 1'b1, "u","r=0,b=1, closed","u = 1, b = 1 when closed", "%b")
  `assert(d, 1'b0, "d","r=0,b=1, closed","d = 0, b = 1 when closed", "%b")
  `assert(State, Opening, "State","r=0, b=1, closed","State = Opening when b = 1 when closed", "%b")
  #10  // t = 110
   c = 0;
   
      
   /////////////// test transition from Opening to Open becuase o = 1
  #20  // t = 130
   b = 0; c = 0; o = 1; s = 0; r = 0; 
  #10  // t = 140
   // assert u = 0, d = 0, State = Open 
  `assert(u, 1'b0, "u","r=0,b=0, opened","u = 0 when fully opened", "%b")
  `assert(d, 1'b0, "d","r=0,b=0, opened","d = 0 when fully opened", "%b")
  `assert(State, Open, "State","r=0,b=0, opened","State = Open when fully opened", "%b")
  
   /////////////// test: Open state, b = 1, s =1, stays Open
  #10  // t = 150
   b = 1; s = 1;
  #10  // t = 160
   // assert u = 0, d = 0, State = Open 
  `assert(u, 1'b0, "u","r=0,b=1,s=1, opened","u = 0 when b=1,s=1, opened", "%b")
  `assert(d, 1'b0, "d","r=0,b=1,s=1, opened","d = 0 when b=1,s=1, opened", "%b")
  `assert(State, Open, "State","r=0,b=0, opened","State remains Open when b=1,s=1, opened", "%b")
 
  #20  $finish;
 end 
endmodule