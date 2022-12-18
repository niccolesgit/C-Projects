
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include <stdbool.h>
#include<errno.h>

typedef struct {
	char line[128];
	char type[50];
	char f_register[50];
	char o_register[50];
	char p_stages[9][4];
} struct_instr;

//print function to print each instance
void print(struct_instr* inst, int c){
	//starts simulation
	printf("START OF SIMULATION\n\n");

	//cycle counter
	int c_cycle = 0;
	//counter to check when it's done
	bool complete = false;
	//making sure we don't go past 9 cycles
	while (c_cycle < 9){

		//print cycles
		printf("CPU Cycles ===>");
		//sets the cycle numbers
		int cycle = 1;
		//loops through all 9
		while (cycle < 10){
			//prints each cycle with a tab (format)
			printf("\t%d", cycle);
			cycle++;
		}
		//print a new line
		printf("\n");


		//print instructions
		for (int i = 0; i < c; i++){
			//print the instruction line
			printf("%s", inst[i].line);
			//loops through all the cycles
			for (int j = 0; j < 9; j++){ 
				if (j > c_cycle){
					//prints a tab and period if there is none
					printf("\t.");
				} else {
					//else pring a tab and it's current stage
					printf("\t%s", inst[i].p_stages[j]);

					//if the last instruction was finished, stop the instruction
					if (i == c-1 && strcmp(inst[i].p_stages[j], "WB") == 0){
						//set complete to true
						complete = true;
					}
				}
			}
			//print a new line (format)
			printf("\n");
		}
		//if the instructions are complete
		if (complete){
			//break out of the loop
			break;
		}

		//increase the cycle count
		c_cycle++;
		//print a new line (format)
		printf("\n");
	}

	//print that it's the end of the simulation
	printf("\nEND OF SIMULATION\n");
}

int main(int argc, char *argv[])
{
	//gets file name from command line
	char *filename = argv[1]; 
	//checks the number of arguments
	if(argc != 2){
		printf("INCORRECT NUMBER OF ARGUMENTS (1 INPUT ONLY) \n");
		return -1;
	}
	//opens and reads te file
	FILE *file = fopen(filename, "r");
	char buffer[128]; 

	//creates memory for the instruction struct
	struct_instr* inst = (struct_instr*) malloc (5 * sizeof(struct_instr));
	//counter
	int c = 0;

	//loops through the whole file
	while (fgets(buffer, 128, file)){ 
		buffer[strcspn(buffer, "\n")] = 0;
		//copies the specified line into the struct 
		// attribute
		strcpy(inst[c].line, buffer);

		//sets a placeholder from the line
		char *pl_holder = strtok(buffer, " ");
		//copies the specified type into the struct 
		// attribute
		strcpy(inst[c].type, pl_holder); 

		//taking the registers
		pl_holder = strtok(NULL, " ");
		pl_holder = strtok(pl_holder, ",");	

		//copies the first register into the struct 
		// attribute
		strcpy(inst[c].f_register, pl_holder); 
		pl_holder = strtok(NULL, "");

		//copies any other registers into the struct 
		// attribute
		strcpy(inst[c].o_register, pl_holder);

		//increments through
		c++;
	}

	//creates a array with all possible stages
	char a_stages[5][4] = {"IF", "ID", "EX", "MEM", "WB"};

	//loops through
	for (int i = 0; i < c; i++){
		for (int j = 0; j < 9; j++){
			//copies the appropriate stage
			strcpy(inst[i].p_stages[j], ".");
		}
	}
	//counter for the stage number
	int s_num = 0;

	//loops through
	for (int i = 0; i < c; i++){
		//checks the stages are less than 4
		while(s_num < 4){
			//if so loop through and copy the appropriate stage
			for (int j = i; j < i+5; j++){
				strcpy(inst[i].p_stages[j], a_stages[s_num]);
				//increment the stage number
				s_num++;
			}
		}
		//reset the stage number
		s_num = 0;
	}
	//print all the instructions and the counter
	print(inst, c);

}