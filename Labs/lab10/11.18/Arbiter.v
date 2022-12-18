//  Two-way arbiter FSM/HLSM.  
//   Similar to the example in the HLSM chapter, but eliminates extra state ("Arbitrate") and uses meaningful signal names  
module Arbiter(
   input clk,rst,ra,rb, // ra = request from requestor a, rb = request from requestor b, which are level (not pulse) signals
   input [1:0] PA, PB,  // priority values for requestor a and requestor b, unsigned, higher number is higher priority
   output reg ga,gb     // ga = grant to a, gb = grant to b
);
   reg [2:0] state;
   always @ (posedge clk or posedge rst) begin
      if(rst)begin 
         state <= 0;
         ga = 0;
         gb = 0;
      end
      else begin 
         case(state) //state
         0: begin //if state is 0
            if(ra&!rb)begin 
               state <= 1;
               ga = 1;
            end
             if(!ra&rb)begin 
               state <=2;
               gb = 1;
            end
             if(ra&rb)begin 
               if(PB > PA) begin 
                  state <= 2;
                  gb = 1;
               end
               else begin 
                  state <= 1;
                  ga = 1;
                  end
            end
         end
         1: begin // if state is 1 (Grant A)
            if(!ra) begin 
               state <= 0; //reset state
               ga = 0; //set ga to 0
            end
            gb=0;//set gb to 0
         end

         2: begin  //if state is 2 (Grant B)
            if(!rb) begin 
               state <= 0;
               gb = 0;
            end
            ga = 0;
         end
         endcase
      end
   end
endmodule