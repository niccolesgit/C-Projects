// Garage door opener FSM  
module Opener(            
   input clk,b,c,o,r,s,  // b: button, c: closed limit switch, o: open limit switch, r = reset, s = sensor
   output reg d,u,       // d: run the motor downwards to close the door, u: run the motor upwards to open the door
   output reg [1:0] State // an output for troubleshooting and simulation verification
);
 
 // insert code here
 always @ (posedge clk) begin
   if(r && c)
      if(c)
         State <= 0; //closed
      if(~c&~o)
         u = 1;
         State<=1; //opening
      if(o)
         State <= 2; //open
   if(c&o == 0)
      State <= State; // 3 = closing

   if(State == 0) // closed -> opening
      if (b==1)
         State<=1;
   if(State == 1) //opening -> open
      if (o==1)
         State<=2;
   if(State == 2) //open -> closing unless s = 1
      if (b&~s)
         State <= 3;
   if(State == 3) //closing -> opening
      if(c&~b&~s)
         State <= 0;
      else if(s | b)
         State <= 1;
   
   if(State == 1) //setting u when opening
      u <= 1;
   else
      u <=0;
   
   if(State == 3) // setting d when closing
      d <= 1;
   else
      d<=0;
 end
endmodule