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
  `assert(State, Opening, "State","r=0, b=1, closed","State transitions to Opening when b = 1 when Closed", "%b")
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
   b = 0;
  #10  // t = 170
   b = 0; s = 0;
   
  /////////////// test: Open state, b = 1, s = 0, State transitions from Open to Closing
   #10  // t = 180
   b = 1; s = 0;
   #10  // t = 190
    // assert u = 0, d = 1, State = Closing 
  `assert(u, 1'b0, "u","b=1,s=0, opened","u = 0 when b=1,s=0, opened", "%b")
  `assert(d, 1'b1, "d","b=1,s=0, opened","d = 1 when b=1,s=0, opened", "%b")
  `assert(State, Closing, "State","b=0, opened","State transitions to Closing when b=1,s=0, opened", "%b")
  b = 0;
  #10  // t = 200
  o = 0;
   
   /////////////// test: Closing state, b = 0, s = 0, c = 1 State transitions from Closing to Closed
   #30  // t = 230
   b = 0; c = 1; s = 0;
   #10  // t = 240
    // assert u = 0, d = 1, State = Closing 
  `assert(u, 1'b0, "u","b=0,s=0,closed","u = 0 when b=0,s=0,closed", "%b")
  `assert(d, 1'b0, "d","b=0,s=0,closed","d = 0 when b=0,s=0,closed", "%b")
  `assert(State, Closed, "State","b = 0, s = 0, closed","State transitions from Closing to Closed when b=0,s=0,c=1", "%b")
 
   /////////////// test: Closed state, b = 1, s = 0, c = 1 State transitions from Closed to Opening again
   #10  // t = 250
   b = 1; c = 1; s = 0;
   #10  // t = 260
  // assert u = 1, d = 0, State = Opening 
  `assert(u, 1'b1, "u","r=0,b=1, closed","u = 1, b = 1 when closed", "%b")
  `assert(d, 1'b0, "d","r=0,b=1, closed","d = 0, b = 1 when closed", "%b")
  `assert(State, Opening, "State","r=0, b=1, closed","State transitions to Opening when b = 1 when Closed", "%b")
   b = 0;
   #10  // t = 270
   c = 0;
 
   /////////////// test transition from Opening to Open becuase o = 1 again
  #20  // t = 290
   o = 1;  
  #10  // t = 300
   // assert u = 0, d = 0, State = Open 
  `assert(u, 1'b0, "u","r=0,b=0, opened","u = 0 when fully opened", "%b")
  `assert(d, 1'b0, "d","r=0,b=0, opened","d = 0 when fully opened", "%b")
  `assert(State, Open, "State","r=0,b=0, opened","State = Open when fully opened", "%b")
  
   /////////////// test transition from Closing to Opening if s = 1 when Closing
  b = 1;
  #10   // t = 310 
  `assert(State, Closing, "State","b=1,s=0","State transition from Open to Closing if b = 1, s = 0", "%b")
  b = 0; 
  #10   // t = 320 
  o = 0;  // no longer opened
  #10   // t = 330 
  s = 1;  // sensor indicates obstacle
  #10   // t = 340 
  // assert u = 1, d = 0, State = Opening 
  `assert(u, 1'b1, "u","b=0,s=1,was closing","u = 1 when State transitions from Closing to Opening if s = 1", "%b")
  `assert(d, 1'b0, "d","b=0,s=1,was closing","d = 0 when State transitions from Closing to Opening if s = 1", "%b")
  `assert(State, Opening, "State","b=0,s=1","State transition from Closing to Opening if s = 1", "%b")
  #10   // t = 350 
  s = 0;  // sensor indicates no obstacle

  #20  $finish;
 end 
endmodule