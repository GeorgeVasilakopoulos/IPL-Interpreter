#include <stdio.h>

static int error_flag;


//Originally declared in instruction_block.c
struct instruction_block{
	int line_number;
	char* line_of_code;
	struct instruction_block*next;
	struct instruction_block*branch;
};

#include "error_handling.h"


void declare_error(int error_type, void* ptr){
	error_flag = error_type;
	struct instruction_block* block =(struct instruction_block*)ptr; 
	if(block){
		switch(error_flag){
			case 1:
				//Invalid variable name
				printf("Error at Line %d: Invalid Variable Name\n",block->line_number);
				break;
			case 2:
				//Invalid expression
				printf("Error at Line %d: Invalid Expression\n",block->line_number);
				break;
			case 3:
				//Indentation error
				printf("Error at Line %d: Invalid Indentation\n",block->line_number);
				break;
			case 4:
				//File not found
				printf("File named %s not found!\n",(char*)block);
				break;
			case 5:
				//Else without if previously
				printf("Error at Line %d: \"else\" statement without previous \"if\" statement\n",block->line_number);
				break;
			case 6:
				printf("Error at Line %d: Incorrectly parenthesized expression\n",block->line_number);
				break;
			case 7:
				printf("Error at Line %d: Array index out of bounds\n",block->line_number);
				break;
			case 8:
				//Unknown character
				printf("Error at Line %d: Unknown character\n",block->line_number);
				break;
			case 9:
				//Unexpected expression after else
				printf("Error at Line %d: Unexpected keyword after \"else\" statament\n",block->line_number);
				break;
			case 10:
				printf("Error at Line %d: Invalid or missing operator",block->line_number);
				break;
			case 11:
				//Expected an integer or '\n' after break
				break;
		}
	}
}


int error_occurred(){
	return error_flag;
}

