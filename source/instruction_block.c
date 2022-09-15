#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#include "line_reader.h"
#include "memory_management.h"
#include "expressions.h"

struct instruction_block{
	int line_number;
	char* line_of_code;
	struct instruction_block*next;
	struct instruction_block*branch;
};

#include "error_handling.h"

static FILE* main_file;

static void loadFile(char* ipli_file){
	main_file=fopen(ipli_file, "r");
}




static char* getNextLine(char* destination, int* line_number_destination){
	static int line_counter=1;
	char* null_check = fgets(destination, MAX_SIZE,main_file);
	line_counter++;
	//Ignore lines that are empty or have only comments.
	while(null_check!=NULL && (*skip_spaces(null_check) == '\n' ||*skip_spaces(null_check) == '\0'|| *skip_spaces(null_check) == '#')){
		null_check = fgets(destination, MAX_SIZE,main_file);
		line_counter++;
	}
	if(null_check == NULL) return NULL;
	char*ptr=destination;
	while(*ptr != '#' && *ptr!='\n' && *ptr != '\0')ptr++; //don't copy the comments onto the block.
	*ptr='\0';
	*line_number_destination = line_counter;
	return destination;
}

static struct instruction_block* getNextBlock(){
	struct instruction_block* new_block = malloc(sizeof(struct instruction_block));
	new_block->line_number=0;
	new_block->next = NULL;
	new_block->branch = NULL;
	new_block->line_of_code = malloc(sizeof(char)*MAX_SIZE);
	if(getNextLine(new_block->line_of_code,&(new_block->line_number)) == NULL){
		free(new_block);
		return NULL;	//Free what you just allocated
	}
	return new_block;
}




//If the first word of the block is the same as the string keyword, return a pointer right after the word.
//Otherwise, return NULL
static char* is__Block(char* keyword, struct instruction_block* block){
	if(block == NULL)return NULL;
	return nextWordIs(keyword,block->line_of_code);
}




//If the first word is a conditional keyword, return a pointer after it
//NULL otherwise
static char* isConditional(struct instruction_block* block){
	char* condition = is__Block("if",block);
	if(condition){
		return condition;
	}
	condition = is__Block("while",block);
	if(condition){
		return condition;
	}
	condition = is__Block("else", block);
	if(condition){
		return condition;
	}
	return NULL;
}



//Returns a pointer pointing to the last block.
static struct instruction_block* connect_recursive(const int tab_counter, struct instruction_block** first_block_address){
	struct instruction_block* first_block = getNextBlock();
	if(first_block == NULL || count_tabs(first_block->line_of_code) < tab_counter) return first_block;
	if(count_tabs(first_block->line_of_code) > tab_counter){
		declare_error(3,first_block); //Indentation error
		return NULL;
	}
	*first_block_address = first_block; 
	struct instruction_block* new_block = first_block;

	while(1){
		if(isConditional(new_block)){
			struct instruction_block* temp = connect_recursive(tab_counter + 1,&(new_block->next)); //recursive call in blocks under conditional blocks
			if(error_occurred())return temp;
			if(temp == NULL || count_tabs(temp->line_of_code) < tab_counter) return temp;
			if(count_tabs(temp->line_of_code) > tab_counter){
				declare_error(3,(void*)temp); //Indentation Error
				return temp;
			}
			new_block->branch = temp; //The block that was returned from the recursive call is the one to be followed if the condition is false.
			new_block = new_block->branch;
		}	
		else{
			struct instruction_block* temp = getNextBlock();
			if(temp == NULL || count_tabs(temp->line_of_code) < tab_counter) return temp;
			if(count_tabs(temp->line_of_code) > tab_counter){
				declare_error(3,(void*)temp);
				return temp;
			}
			new_block->next = temp;
			new_block = new_block->next;
		}
	}
}



static struct instruction_block* connectBlocks(char*  ipli_file){
	loadFile(ipli_file);
	if(main_file==NULL){
		declare_error(4, (void*) ipli_file); //File not Found
		return NULL;
	}
	struct instruction_block* first_block;
	connect_recursive(0,&first_block);
	return first_block;
}





//returns 1 if the return was triggered by a break line within the source file
//0 Otherwise
static int run_recursive(struct instruction_block* block){
	static int break_counter=0;
	while(block != NULL){
		char* code_after_keyword;
		if(code_after_keyword = is__Block("if",block)){ 
			if(evaluateExpression(code_after_keyword)){
				if(error_occurred()){
					declare_error(error_occurred(),(void*) block);
					return 0;
				}
				run_recursive(block->next);
				if(break_counter > 0) return 1;
				block = block->branch;
				while(block != NULL && is__Block("else",block))block=block->branch;
			}
			else{
				if(error_occurred())return 0;
				block = block->branch;
				char* condition = is__Block("else",block); //if it is an else (if) statement
				while(block!=NULL && condition){
					condition = skip_spaces(condition);
					if(condition[0] == '\0'){		//if an else statement is found, execute it.
						run_recursive(block->next);
						if(break_counter > 0) return 1;
						block=block->branch;
						break;
					}
					condition = nextWordIs("if",condition); //If an else if statement is found
					if(error_occurred())return 0;
					if(condition == NULL){
						declare_error(9,(void*)block);	//If after the else keyword, something other than if is found, declare error.
						return 0;
					}
					if(evaluateExpression(skip_spaces(condition))){
						if(error_occurred()){
							declare_error(error_occurred(), (void*)block);
							return 0;
						}
						run_recursive(block->next);
						if(break_counter > 0) return 1;
						block = block->branch;
						while(block != NULL && is__Block("else",block))block=block->branch; //ignore the next else (if) statements.
						break;
					}
					block=block->branch;
					condition = is__Block("else",block);
				}
			}
			continue;
		}
		else if(code_after_keyword = is__Block("while",block)){
			while(evaluateExpression(code_after_keyword)){
				if(error_occurred()){
					declare_error(error_occurred(), (void*)block);
					return 0;
				}
				run_recursive(block->next);
				if(break_counter > 0){
					break_counter--;
					if(break_counter == 0)break;
					if(break_counter > 0)return 1;
				}
			}
			block=block->branch;
			continue;
		}
		else if(code_after_keyword = is__Block("write",block)){
			int value = evaluateExpression(code_after_keyword);
			if(error_occurred()){
				declare_error(error_occurred(), (void*)block);
				return 0;
			}
			printf("%d",value);
		}
		else if(code_after_keyword = is__Block("writeln",block)){
			int value = evaluateExpression(code_after_keyword);
			if(error_occurred()){
				declare_error(error_occurred(), (void*)block);
				return 0;
			}
			printf("%d\n",value);
		}
		else if(code_after_keyword = is__Block("read",block)){
			int index;
			char name[MAX_SIZE];
			int value;
			getVariableFromString(name, &index, code_after_keyword);
			if(error_occurred()){
				declare_error(error_occurred(), (void*)block);
				return 0;
			}
			scanf("%d",&value);
			if(Assign_Value(name,index,value)){ //if variable does not exist, create one and assign the value
				Construct_Array(name,index+1);
				Assign_Value(name,index,value);
			}
		}
		else if(code_after_keyword = is__Block("define",block)){
			int index;
			char name[MAX_SIZE];
			int value;
			getVariableFromString(name, &index, code_after_keyword);
			if(error_occurred()){
				declare_error(error_occurred(), (void*)block);
				return 0;
			}
			Construct_Array(name,index+1);
		}
		else if(code_after_keyword = is__Block("random",block)){
			int index;
			char name[MAX_SIZE];
			int value;
			getVariableFromString(name, &index, code_after_keyword);
			if(error_occurred()){
				declare_error(error_occurred(), (void*)block);
				return 0;
			}
			value = rand();
			if(Assign_Value(name,index,value)){ //if variable does not exist, create one and assign the value
				Construct_Array(name,index+1);
				Assign_Value(name,index,value);
			}
		}
		else if(code_after_keyword = is__Block("break",block)){
			char* number = skip_spaces(code_after_keyword);
			if(number[0]=='\0'){
				break_counter = 1;
				return 1;
			}
			else{
				if(getNumberFromString(&break_counter, code_after_keyword,NULL) == NULL){
					declare_error(8,(void*)block);	//Expected an integer number after break statement
				}
				return 0;
			}
		}
		else if(code_after_keyword = is__Block("else",block)){
			declare_error(5,(void*)block);		//Else without existance of previous if statement
			return 0;
		}
		else{
			if(error_occurred()){
				declare_error(error_occurred(),(void*)block);
				return 0;
			}
			//Assign value to variable
			int index;
			char name[MAX_SIZE];
			int value;
			char* expression = skip_spaces(getVariableFromString(name,&index, block->line_of_code));
			if(error_occurred()){
				declare_error(error_occurred(), (void*)block);
				return 0;
			}
			if(expression[0]!= '='){
				declare_error(9,(void*)block);	
			} //expected assignment operator
			expression = skip_spaces(expression+1);
			value = evaluateExpression(expression);
			if(error_occurred()){
				declare_error(error_occurred(), (void*)block);
				return 0;
			}
			if(Assign_Value(name,index,value)){ //if variable does not exist, create one and assign the value
				Construct_Array(name,index+1);
				Assign_Value(name,index,value);
			}
		}
		block = block->next;
	}
	return 0;
}


static void runBlockChain(struct instruction_block* first_block){
	if(run_recursive(first_block)){
		declare_error(9,(void*)first_block);
	}	

}








int runFile(char* IPLI_source){

	srand((unsigned int)time(NULL));

	struct instruction_block* first_block = connectBlocks(IPLI_source);
	if(error_occurred())return 1;
	runBlockChain(first_block);
	if(error_occurred())return 1;
	
	return 0;
}








