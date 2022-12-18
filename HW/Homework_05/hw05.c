#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include <math.h>

//make global arrays to hold t and s registers
char* temp_r[] = {"$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7"};
char* saved_r[] = {"$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7"};

//creates a struct that holds all of the attributes of the registers
typedef struct{
	int s_sign[8]; 
	int nums[10]; 
	int numCount; 
    char s[8];
	char temp[10];
    int sReg; 
	int tReg;
} struct_reg;


//function that divides by 2 and counts every time it occurs
int getPow2(int num) {
	int temp = num; 
	int count = 0;
	while(temp != 1) {
		temp /= 2; 
		count++; 
	}
	return count;
}

//function that counts the number of operations in each line of the file
int countOp(char* b_ptr) {
	int op_counter = 0;
	while(*b_ptr) {
		if(*b_ptr == '+' || *b_ptr == '-' || *b_ptr == '*' || *b_ptr == '/' || *b_ptr == '%') {
			op_counter++;
		}
		b_ptr++;
	}
	return op_counter;
}

//function that changes a string into a number
long getNum(char* b_ptr, int check, struct_reg* r, int numsPerLine) {
	long val;
	r->numCount++;
	int currNumsInLine = 0;
	while (*b_ptr) {
		if(check) {
			if(isdigit(*b_ptr) ) {
		        val = strtol(b_ptr, &b_ptr, 10);
				currNumsInLine++;
		    }
		} else if(isdigit(*b_ptr) || ((*b_ptr=='-') && isdigit(*(b_ptr+1)))) {
			val = strtol(b_ptr, &b_ptr, 10);
			currNumsInLine++;
	        r->nums[r->numCount] = val;
	    }
		if (currNumsInLine == numsPerLine) {
			numsPerLine++;
			return val;
		}
	    b_ptr++;
	}
	return val;
}

// function that computes add
int add(char* buffer, int pos_curr, int opCount, int op_curr, struct_reg* r) {
	int l_side = pos_curr - 2; // left side
	int r_side = pos_curr + 2; //right side
	int tReg_ = r->tReg; //keeps track of current tReg

	//prints the specified instruction
	if(isdigit(buffer[r_side])) 
		printf("addi "); 
	else printf("add "); 
	
	//determines if it goes to the saved or temp register
	//if it's the last operation, it goes to the saved register
	if(op_curr == opCount) { 
		r->sReg++;
		r->s[r->sReg] = buffer[0];
		printf("%s,", saved_r[r->sReg]);
	}
	//if not, it goes to the temp register
	else {
		r->tReg++;
		printf("%s,", temp_r[r->tReg]);
	}
	//if it goes to the first operation, it goes to
	// a saved register
	if(op_curr == 1) { 
		char c = buffer[l_side];
		for(int i = 0; i <= r->sReg; i++) {
			if(r->s[i] == c)
				printf("%s,", saved_r[i]);  
		}
	// else go to the temp register
	} else printf("%s,", temp_r[tReg_]); 

	//if the number is a constant, print the number
	if(isdigit(buffer[r_side]))
		printf("%ld\n", getNum(buffer, 0, r, 1));  
	//else, print what is on the right side of the register
	else { 
		char c = buffer[r_side]; 
		for(int i = 0; i <= r->sReg; i++) {
			if(r->s[i] == c) 
				printf("%s\n", saved_r[i]); 
		}
	}

	return 0;
}

//does the same as add, except prints a negative sign on for the right side and
// prints the sub command instead of add
int sub(char* buffer, int pos_curr, int opCount, int op_curr, struct_reg* r) {

	int l_side = pos_curr - 2; //left side
	int r_side = pos_curr + 2; //right side
	int tReg_ = r->tReg; // keeps track of the tReg
	//print instruction
	if(isdigit(buffer[r_side]))
		printf("addi ");
	else printf("sub "); //prints sub

	if(op_curr == opCount) {
		r->sReg++;
		r->s[r->sReg] = buffer[0];
		printf("%s,", saved_r[r->sReg]); 
	}
	else {
		r->tReg++;
		printf("%s,", temp_r[r->tReg]);
	}

	if(op_curr == 1) {
		char c = buffer[l_side];
		for(int i = 0; i <= r->sReg; i++) {
			if(r->s[i] == c)
				printf("%s,", saved_r[i]);  
		}
	} else printf("%s,", temp_r[tReg_]);

	if(isdigit(buffer[r_side])) {
		printf("-%ld\n", getNum(buffer, 0, r, 1)); 
	} else {
		char c = buffer[r_side]; 
		for(int i = 0; i <= r->sReg; i++) {
			if(r->s[i] == c) 
				printf("%s\n", saved_r[i]); 
		}
	}
	return 0;
}

//function for multiplication
int mult(char* buffer, int pos_curr, int opCount, int op_curr, struct_reg* r, int numsPerLine) {
	
	int l_side = pos_curr - 2; //left side
	int r_side = pos_curr + 2; //right side
	int tReg_ = r->tReg; // tReg
	int sReg_ = 0; // sets temp sReg to 0

	//values to check the left side and negative/positive values
	int slc_left = 0; 
	int check = 0;

	//sll (left shift)
	//if the right side is a value or negative sign
	if(isdigit(buffer[r_side]) || ((buffer[r_side] == '-' && isdigit(buffer[r_side+1])))) {
		//move to the right if its a negative sign
		if(buffer[r_side] == '-')
			r_side = pos_curr + 3; 

		//then search for the register of the left side
		char c = buffer[l_side];
		for(int i = 0; i < r->sReg; i++) {
			if(r->s[i] == c) {
				sReg_ = i;  
			}
		}
		//increments both t registers
		r->tReg++;
		tReg_++;

		//finds the current number
		int num = getNum(buffer, 0, r, numsPerLine);
		
		//when num equals 0, it prints the register,
		// value and 0
		if(num == 0) {
			if(opCount > 1) {
				r->tReg++;
				printf("li %s,0\n", temp_r[r->tReg]);
			} else {
				r->sReg++;
				printf("li %s,0\n", saved_r[r->sReg]);
			}
			return 0;
		}
		// if the number is negative
		else if(num < 0) { 
			check = 1; // change check to true
			//find the new number
			num = getNum(buffer, 1, r, numsPerLine);
		}
		//if the number is one or negtive one
		// prints the commands
		if(num == 1) { 
			printf("move %s,%s\n", temp_r[tReg_], saved_r[r->sReg]);
			r->sReg++;
			if(check)
				printf("sub %s,$zero,%s\n", saved_r[r->sReg], temp_r[tReg_]);
			else printf("move %s,%s\n", saved_r[r->sReg], temp_r[tReg_]);
			
			return 0;
		}

		//loops through the powers of two of the number. prining all the correct 
		// instructions
		while(num != 1) {
			int pow2 = getPow2(num);

			//print instructions
			printf("sll %s,%s,%d\n", temp_r[tReg_], saved_r[sReg_], pow2); 
			if(slc_left == 0) {  //if its the first sll, print move
				r->tReg++; 
				printf("move %s,%s\n", temp_r[r->tReg], temp_r[tReg_]); 
			}
			//else print add
			else printf("add %s,%s,%s\n", temp_r[r->tReg], temp_r[r->tReg], temp_r[tReg_]);
			int result = pow(2, pow2);

			//subtract result form num
			num -= result; 
			//increment the left shift
			slc_left++;
		}
		//if the num is one or negative one, print the following instructions
		if(num == 1 || num == -1) {
			printf("add %s,%s,%s\n", temp_r[r->tReg], temp_r[r->tReg], saved_r[sReg_]);
			r->sReg++;
			if(check) 
				printf("sub %s,$zero,%s\n", saved_r[r->sReg], temp_r[r->tReg]);
			else printf("move %s,%s\n", saved_r[r->sReg], temp_r[r->tReg]);

			
		}

			
	//if there are no constants there are all variables
	} else { 
		//prints mult and all the variables
		printf("mult ");
		if(op_curr == 1) {
			char c = buffer[l_side];
			for(int i = 0; i <= r->sReg; i++) {
				if(r->s[i] == c)
					printf("%s,", saved_r[i]);  
			}
		} else printf("%s,", temp_r[tReg_]);
		char c = buffer[r_side];
		if (c == '-') {
			c = buffer[r_side + 1];
		}
		printf("%c", c);
		for(int i = 0; i <= r->sReg; i++) {
			printf("%c", r->s[i]);
			if(r->s[i] == c)
				printf("%s\n", saved_r[i]);
		}

		//prints mflow
		printf("mflo "); 

		if(op_curr == opCount) {
			r->sReg++;
			printf("%s\n", saved_r[r->sReg]);
		} else {
			r->tReg++;
			printf("%s\n", temp_r[r->tReg]);
		}
	}
	return 0; 
}

//divide function
int divide(char* buffer, int pos_curr, int opCount, int op_curr, struct_reg* r, int numsPerLine) {
	int l_side = pos_curr - 2; //left side
	int r_side = pos_curr + 2; //right side
	int tReg_ = r->tReg; // tReg
	int sReg_ = 0; //sets temp sReg to 0
	int check = 0; //false value
    int check2 = 1; //true value
	int c_left = 0; // checks the left side

	//searches for the register given the right side
	char c = buffer[l_side];
	for(int i = 0; i < r->sReg; i++) {
		if(r->s[i] == c) {
			sReg_ = i;  
		}
	}
	if(r->s_sign[sReg_] == 0) {
		check2 = 0; 
	}
	//srl (right shift)
	if(check2 && (isdigit(buffer[r_side]) || ((buffer[r_side] == '-' && isdigit(buffer[r_side+1])) ))) {
		printf("bltz %s,L%d\n", saved_r[sReg_], c_left); 
		c_left++;

		//gets num
		int num = getNum(buffer, 0, r, numsPerLine);
		
		//if num is negtaive
		if(num < 0) {
			check = 1;
			num = getNum(buffer, 1, r, numsPerLine);
		}
		//if num equals 1
		if(num == 1) {
			r->sReg++;
			if(check)
				printf("sub %s,$zero,%s\n", saved_r[r->sReg], saved_r[sReg_]);
			else printf("move %s,%s\n", saved_r[r->sReg], saved_r[sReg_]);
			
			return 0;
		}
		//prints the instructions 
		int pow2 = getPow2(num);
		r->sReg++;
		printf("srl %s,%s,%d\n", saved_r[r->sReg], saved_r[sReg_], pow2); 
		if(check)
			printf("sub %s,$zero,%s\n", saved_r[r->sReg], saved_r[r->sReg]);
		printf("j L%d\n", c_left);
		c_left--;
		printf("L%d:\n", c_left); 
		c_left++;
		r->tReg++; 
		tReg_++; 
		printf("li %s,%d\n", temp_r[tReg_], num); 


	} 
	//if false
	if(!check2) {	
		//find a num, increment the tRegs and print the li instruction
		int num = getNum(buffer, 0, r, numsPerLine);
		r->tReg++;
		tReg_++;
		printf("li %s,%d\n", temp_r[tReg_], num);
	}
	
	//printing div with no constants
	printf("div ");

	//if you are at a current operation
	// print the necessary instructions
	if(op_curr == 1) {
		char c = buffer[l_side];
		for(int i = 0; i <= r->sReg; i++) {
			if(r->s[i] == c)
				printf("%s,", saved_r[i]);  
		}
	} else printf("%s,", temp_r[tReg_]);
	//printf("%c", buffer[r_side]);
	if(isdigit(buffer[r_side]) || (buffer[r_side] == '-' && isdigit(buffer[r_side+1]))) {
		printf("%s\n", temp_r[tReg_]);
	} else {
		char c = buffer[r_side];
		for(int i = 0; i <= r->sReg; i++) {
			if(r->s[i] == c) 
				printf("%s\n", saved_r[i]); 
		}

	}
	
	//prints mflo with registers
	printf("mflo ");

	if(op_curr == opCount) {
		r->sReg++;
		printf("%s\n", saved_r[r->sReg]);
	} else {
		r->tReg++;
		printf("%s\n", temp_r[r->tReg]);
	}

	if(isdigit(buffer[r_side]) || ((buffer[r_side] == '-' && isdigit(buffer[r_side+1])) )) {
		printf("L%d:\n", c_left); 
	}
	
	return 0; 
}

//function that handles modular division
int modDivide(char* buffer, int pos_curr, int opCount, int op_curr, struct_reg* r) {
	int l_side = pos_curr - 2; //left side
	int r_side = pos_curr + 2; //right side
	int tReg_ = r->tReg; //temp tReg based on original tReg

	//prints div and registers for div
	printf("div ");

	// register cases similar to add and sub
	if(op_curr == 1) {
		char c = buffer[l_side];
		for(int i = 0; i <= r->sReg; i++) {
			if(r->s[i] == c)
				printf("%s,", saved_r[i]);  
		}
	} else printf("%s,", temp_r[tReg_]);

	if(isdigit(buffer[r_side])) {
		printf("REG2\n");
	} else {
		char c = buffer[r_side]; 
		for(int i = 0; i <= r->sReg; i++) {
			if(r->s[i] == c) 
				printf("%s\n", saved_r[i]); 
		}
	}


	//prints mfhi and registers
	printf("mfhi "); 

	if(op_curr == opCount) {
		r->sReg++;
		printf("%s\n", saved_r[r->sReg]);
	} else {
		r->tReg++;
		printf("%s\n", temp_r[r->tReg]);
	}

	return 0; 
}


int main(int argc, char* argv[])
{
    //error checks
	if(argc < 2)
		return -1;

	//allocates memory for the struct_register
	struct_reg* r = (struct_reg*)malloc(sizeof(struct_reg));
	char* filename = argv[1];

	//sets default values of the register
	r->sReg = -1;
	r->tReg = -1;
	r->numCount = -1; 

	//opens and reads the file
	FILE* my_file = fopen(filename, "r");

	//creates a buffer and intializes the spaces
  	char buffer[256];
  	int spaces = 0;
	int numsPerLine;

	//loops through the lines of the file
  	while(fgets(buffer, sizeof(buffer), my_file)) {
        //prints the command line
  		printf("# %s", buffer);
  		numsPerLine = 1;

		//counts through all the number of spaces
  		char* sp_ptr = buffer;
  		while(*sp_ptr) {
  			if(isspace(*sp_ptr)) {
  				spaces++;
  			}
  			sp_ptr++;
  		}
		//if there are three spaces 
  		if(spaces == 3) {
			//get the numbers
  			long num = getNum(buffer, 0, r, 1);
			//increments and sets the sregister
  			r->sReg++; 
  			r->s[r->sReg] = buffer[0]; 
			//if the number is negative
			//print the registers
  			if(num < 0)
  				r->s_sign[r->sReg] = 0; 
  			else r->s_sign[r->sReg] = 1;  
  			printf("li %s,%ld\n", saved_r[r->sReg], num);
  		}
  		else {
			//creates a pointer to increment through the buffer
			char * b_ptr = buffer;
			//sets a counter for the operators
  			int opCount = 0;
			//loops through and counts all the operators
			while(*b_ptr) {
				if(*b_ptr == '+' || *b_ptr == '-' || *b_ptr == '*' || *b_ptr == '/' || *b_ptr == '%') {
					//adds to the count
					opCount++;
				}
				//increments the pointer
				b_ptr++;
			}
			//sets current position and function count
  			int op_curr = 1;
  			int funCount = 0;

			//string characters to keep track of the lines
  			char* b1 = buffer;
  			char* b2 = b1 - 2;

			//goes through each line of the operations
  			while(*b1) {
				//if its a plus, go the add function
  				if(*b1 == '+') {
  					add(buffer, funCount, opCount, op_curr, r); 
  					op_curr++;
  				}
				//if its a dash, go to te sub function
  				else if(*b1 == '-' && *b2 != '*' && *b2 != '/' && *b2 != '%') {
  					sub(buffer, funCount, opCount, op_curr, r);
  					op_curr++;
  				}
				//if its an asterisk, go to the mult function
  				else if(*b1 == '*') {
  					mult(buffer, funCount, opCount, op_curr, r, numsPerLine);
  					op_curr++; 
  				}
				//if its a slash, go to the divide function
  				else if(*b1 == '/') {
  					divide(buffer, funCount, opCount, op_curr, r, numsPerLine); 
  					op_curr++;
  				}
				//if its a percent sign, go to the mod function
  				else if(*b1 == '%') {
  					modDivide(buffer, funCount, opCount, op_curr, r);
  				}
  				b1++;
  				b2++;
  				funCount++;
  			}
		}	
  		spaces = 0;
	}


    return 0;
}