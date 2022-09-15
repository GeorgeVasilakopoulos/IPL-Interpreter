#include <string.h>
#include <stdio.h>
#include "line_reader.h"
#include "error_handling.h"


int count_tabs(char* line_of_code){
	int counter=0;
	while(*(line_of_code++) == '\t'){
		++counter;
	}
	return counter;
}


char* skip_spaces(char* char_ptr){
	if(char_ptr == NULL)return NULL;
	while(*char_ptr == '\t' || *char_ptr == ' ')char_ptr++;
	return char_ptr;
}


int isLetter(char c, int allow_underscores){
	return (c >='a' && c<= 'z')||(c >='A' && c<= 'Z')||(allow_underscores && c == '_');
}

int isNumeral(char c){
	return (c>='0' && c <='9');
}

int isOperator(char c){
	return (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '='||c == '<'||c == '>'||c =='!');
}



//Used in order to avoid code repetition in getNextTerm.
static void copy_character(char** keyword_output, char** ptr, int* index_difference){
	if(*keyword_output != NULL){
		**keyword_output = **ptr;
		(*keyword_output)++;
	}
	(*ptr)++;
	(*index_difference)++;
}


//Term can be:
// 1. a sequence of letters or numbers or underscores (must not start with underscores)
// 2. a sequence of operators
// 3. a correctly parenthesised string with at least 1 opening parenthesis
// 4. a correctly "bracket-ised" string with at least 1 opening bracket (https://www.urbandictionary.com/define.php?term=bracketise)
char* getNextTerm(const char* line_of_code, const int index_input, char* keyword_output, int* index_output){
	char* ptr = skip_spaces((char*)line_of_code + index_input);
	int index_difference = ptr - line_of_code - index_input; 	
	if(isLetter(*ptr,0) ||isNumeral(*ptr)){
		copy_character(&keyword_output, &ptr, &index_difference);
		while(isLetter(*ptr,1) ||isNumeral(*ptr)){
			copy_character(&keyword_output, &ptr, &index_difference);
		}
	}
	else if(isOperator(*ptr)){
		copy_character(&keyword_output, &ptr, &index_difference);
		while(isOperator(*ptr)){
			copy_character(&keyword_output, &ptr, &index_difference);
		}
	}
	else if(*ptr == '('){
		int parentheses_counter = 1;
		copy_character(&keyword_output, &ptr, &index_difference);
		while(parentheses_counter > 0 && *ptr != '\0'){
			if(*ptr == ' ' || *ptr == '\t'){
				ptr++;
				index_difference++;
				continue;
			}
			if(*ptr == ')'){
				parentheses_counter--;
				copy_character(&keyword_output, &ptr, &index_difference);
			}
			else if(*ptr == '('){
				parentheses_counter++;
				copy_character(&keyword_output, &ptr, &index_difference);
			}
			else if(isLetter(*ptr,1) || isOperator(*ptr)||isNumeral(*ptr)||*ptr == ']' ||*ptr == '['){
				copy_character(&keyword_output, &ptr, &index_difference);
			}
			else{
				//Something else?
				declare_error(8,NULL);
				return NULL;
			}

		}
	}
	else if(*ptr == '['){
		int parentheses_counter = 1;
		copy_character(&keyword_output, &ptr, &index_difference);
		while(parentheses_counter > 0 && *ptr != '\0'){
			if(*ptr == ' ' || *ptr == '\t'){
				ptr++;
				index_difference++;
				continue;
			}
			if(*ptr == ']'){
				parentheses_counter--;
				copy_character(&keyword_output, &ptr, &index_difference);
			}
			else if(*ptr == '['){
				parentheses_counter++;
				copy_character(&keyword_output, &ptr, &index_difference);
			}
			else if(isLetter(*ptr,1) || isOperator(*ptr) || isNumeral(*ptr)||*ptr == ')' ||*ptr == '('){
				copy_character(&keyword_output, &ptr, &index_difference);
			}
			else{
				//Something else?
				declare_error(8,NULL);
				return NULL;
			}
		}
	}
	else if(*ptr == '\0'){
		if(keyword_output!=NULL)*keyword_output = '\0';
		if(index_output!=NULL)*index_output = index_input + index_difference;
		return ptr;	
	}
	else{
		//Something else?
		declare_error(8,NULL); //Unknown Character
		return NULL;
	}
	if(keyword_output!=NULL)*keyword_output = '\0';
	if(index_output!=NULL)*index_output = index_input + index_difference;
	return ptr;
}


//If the next term in the sequence of characters that start in char* ptr is identical to the string keyword,
//then a pointer pointing at the end of the word is returned. NULL otherwise
char* nextWordIs(char* keyword, char* ptr){
	char first_word[MAX_SIZE];
	int index_output;
	getNextTerm(ptr,0,(char*)first_word,&index_output);
	if(error_occurred())return NULL;
	if(strcmp(first_word,keyword))return NULL;
	return ptr + index_output;
}
