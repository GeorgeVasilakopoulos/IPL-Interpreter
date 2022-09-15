#include <string.h>
#include <stdlib.h> 
#include "AVL_Item.h"
#include "AVL_Tree.h"
#include "memory_management.h"

AVL_Tree Memory_Tree;

void Initialize_Memory(){
	AVL_Initialize(&Memory_Tree);
}


void Construct_Array(char* name, int size){
	struct variable* new_variable = malloc(sizeof(struct variable));
	new_variable->size = size;
	new_variable->memory = malloc(sizeof(int) * size); 
	new_variable->name = malloc(sizeof(char)*(strlen(name)+1));
	strcpy(new_variable->name,name);
	AVL_Insert(&Memory_Tree,new_variable);
}


int Assign_Value(char* name, int index, int value){
	struct variable* var = AVL_Search(&Memory_Tree,name);
	if(var == NULL || index<0 || index > var->size - 1) return 1; //Variable not found or out of bounds
	var->memory[index] = value;
	return 0;
}

int Get_Value(char* name, int index, int* destination_ptr){
	struct variable* var = AVL_Search(&Memory_Tree,name);
	if(var == NULL || index<0 || index > var->size - 1){
		//variable not found or index out of bounds
		return 1;	
	}
	*destination_ptr = var->memory[index];
	return 0;
}

int Destruct_Array(char* name){
	struct variable* var = AVL_Search(&Memory_Tree,name);
	if(var == NULL){
		//Variable does not exist
		return 1;
	}
	AVL_Delete(&Memory_Tree,name);
	return 0;
}


int Destruct_Memory(){
	AVL_Destruct(&Memory_Tree);
	return 0;
}








