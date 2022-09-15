#include <string.h>
#include <stdlib.h>
#include<stdio.h>
#include "line_reader.h"
#include "Memory_Management/memory_management.h"
#include "expressions.h"
#include "error_handling.h"


int isValidVariableName(char* name){
	if(!isLetter(*name, 0))return 0;
	while(*name != '\0'){
		if(!isLetter(*name,1) && !isNumeral(*name))return 0;
		name++;
	}
	return 1;
}

//returns a char* that points right after the closing bracket or the end of the variable name
//returns NULL in case of an error
char* getVariableFromString(char* name_output, int* index_output, char* string){
	//printf("bout to getr a variable\n");
	char* start_of_name = skip_spaces(string);
	char* end_of_name = getNextTerm(start_of_name,0,NULL,NULL);
	if(error_occurred())return string;
	char temp = *end_of_name;
	*end_of_name = '\0';
	if(!isValidVariableName(start_of_name)){
		declare_error(1,NULL);
		return string;
	}
	*end_of_name = temp;
	int index = 0;
	char* return_ptr = end_of_name;

	if(*skip_spaces(end_of_name) == '['){
		char* open_bracket = skip_spaces(end_of_name); 
		char* closing_bracket = getNextTerm(end_of_name,0,NULL,NULL);
		if(error_occurred())return string;
		if(closing_bracket == NULL){
			declare_error(2,NULL);
			return string;
		}
		*(closing_bracket - 1)='\0';
		index = evaluateExpression(open_bracket+1);
		if(error_occurred())return string;
		*(closing_bracket - 1)=']';
		return_ptr = closing_bracket;
	}
	temp = *end_of_name;
	*end_of_name = '\0';
	strcpy(name_output,start_of_name);
	*end_of_name = temp;

	*index_output=index;
	return return_ptr;
}

//Searches for an integer right after char* string. If the next term is not an integer, it returns NULL
//If it is, the value is placed in the output pointer.
//char* end_of_num is optional, if it is already known.
char* getNumberFromString(int* value_output, char* string, char* end_of_num){
	char* start_of_num = skip_spaces(string);
	end_of_num = (end_of_num == NULL ? getNextTerm(start_of_num,0,NULL,NULL) : end_of_num);
	if(error_occurred())return NULL;
	if(end_of_num == NULL)return NULL;
	char temp = *end_of_num;
	*end_of_num = '\0';
	int value = atoi(start_of_num);
	*end_of_num = temp;
	if(value == 0){
		if(start_of_num[0]!='0' || start_of_num + 1 != end_of_num)return NULL;
	}
	*value_output = value;
	return end_of_num;
}


//Used only in evaluateExpression.
//It is called in between operators of expressions in order to obtain the value of each term.
static int getValueFromTerm(int* value_output, char** start_of_term_ptr, char** end_of_term_ptr){
	char* start_of_term = *start_of_term_ptr;
	char* end_of_term = *end_of_term_ptr;

	//Check if the term is a parenthesised expression
	if(*start_of_term == '('){
		if(*(end_of_term - 1) != ')'){
			declare_error(6,NULL); //Incorrectly parenthesised expression
			return 1;
		}
		*(end_of_term-1) = '\0';
		*value_output = evaluateExpression(start_of_term+1);	//evaluate recursively the expression between brackets
		if(error_occurred())return 1;
		*(end_of_term-1) = ')';
	}
	//if getNumberFromString returns a non NULL pointer, then the term was a number that was read successfully.
	else if(getNumberFromString(value_output,start_of_term, end_of_term))return 0;  
	//Otherwise, the term has to be a variable.
	else{
		if(error_occurred())return 1;
		int index;
		char name[MAX_SIZE];
		*end_of_term_ptr = end_of_term = getVariableFromString(name,&index,start_of_term);
		if(error_occurred())return 1;
		Get_Value(name,index,value_output);
	}
	
}




//In order to introduce a precedence of operators, each expression is defined recursively as follows:

//Expression is:
// 1. a Variable or an integer.
// 2. a parenthesised expression.
// 3. a sequence of the expressions, with any of these high-precedence operators in between each expression {*,/,%}.
// 4. an algebraic sum of 1 or more expressions.
// 5. a comparison of two expressions.

int evaluateExpression(char* expression){

	
	//This variable will be returned
	int total_result=0;
	
	//can be either a + or a -
	char low_precedence_op = '+';


	//stores the values of type 3 expressions.
	//each value is summed algebraically into total_result
	int product_value;
	

	int current_term_value;
	
	char* start_of_term = skip_spaces(expression);
	char* end_of_term = getNextTerm(start_of_term,0,NULL,NULL);
	if(error_occurred())return 1;
	char operator[3] = {};
	while(*start_of_term != '\0'){
		int return_value = getValueFromTerm(&current_term_value, &start_of_term, &end_of_term);
		if(error_occurred())return 1;
		

		if(!strcmp(operator,"\0")){ //For the first iteration.
			product_value = current_term_value;
		}
		else if(!strcmp(operator,"*")){
			product_value*=current_term_value;
		}
		else if(!strcmp(operator,"/")){
			product_value/=current_term_value;
		}
		else if(!strcmp(operator,"%")){
			product_value%=current_term_value;
		}
		else if(!strcmp(operator,"+")){
			//Add the product_value into total_result and then set it to the new term.
			if(low_precedence_op == '+')total_result += product_value; 
			else total_result -= product_value;
			product_value = current_term_value;
			low_precedence_op = '+';
		}
		else if(!strcmp(operator,"-")){
			//Similarly
			if(low_precedence_op == '+')total_result += product_value;
			else total_result -= product_value;
			product_value = current_term_value;
			low_precedence_op = '-';
		}
		else{
			//invalid operator
			declare_error(10,NULL);
			return 1;
		}

		//Get new operator
		start_of_term = skip_spaces(end_of_term);
		end_of_term = getNextTerm(start_of_term,0,NULL,NULL);
		if(error_occurred())return 1;

		//Check for end of expression
		if(*start_of_term == '\0'){ 
			if(low_precedence_op == '+')total_result += product_value;
			else total_result -= product_value;
			return total_result;
		}


		//Copy operator
		char temp = *end_of_term;
		*end_of_term = '\0';
		strcpy(operator,start_of_term); 
		*end_of_term = temp;

		//If the operator is a comparison operator
		if(!strcmp(operator,"==")){
			if(low_precedence_op == '+')total_result += product_value;
			else total_result -= product_value;
			return total_result == evaluateExpression(skip_spaces(end_of_term));
		}	
		else if(!strcmp(operator,">")){
			if(low_precedence_op == '+')total_result += product_value;
			else total_result -= product_value;
			//printf("here w are\n");
			return total_result > evaluateExpression(skip_spaces(end_of_term));
		}
		else if(!strcmp(operator,"<")){
			if(low_precedence_op == '+')total_result += product_value;
			else total_result -= product_value;
			return total_result < evaluateExpression(skip_spaces(end_of_term));
		}
		else if(!strcmp(operator,">=")){
			if(low_precedence_op == '+')total_result += product_value;
			else total_result -= product_value;
			return total_result >= evaluateExpression(skip_spaces(end_of_term));
		}	
		else if(!strcmp(operator,"<=")){
			if(low_precedence_op == '+')total_result += product_value;
			else total_result -= product_value;
			return total_result <= evaluateExpression(skip_spaces(end_of_term));
		}
		else if(!strcmp(operator,"!=")){
			if(low_precedence_op == '+')total_result += product_value;
			else total_result -= product_value;
			return total_result != evaluateExpression(skip_spaces(end_of_term));
		}	

		//get next term
		start_of_term = skip_spaces(end_of_term);
		end_of_term = getNextTerm(start_of_term,0,NULL,NULL);
		if(error_occurred())return 1;

	}
}
