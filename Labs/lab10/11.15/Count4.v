// 4-bit up/down counter
module Count4(clk,rst,enable,upDown, count);
   input clk, rst, enable, upDown;
   output reg [3:0] count = 0;
// insert code here 
   always @ (posedge clk) begin
   if(!rst) 
      begin
         if(enable == 0)
         count <= count;
         else
            if(upDown==0)
               count<=count-1;
            else if(upDown==1)
               count<=count+1;
      end		
   else
      count <=0;
   end
endmodule  // Count4
