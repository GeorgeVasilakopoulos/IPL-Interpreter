#include <string.h>
#include <stdlib.h>
#include "AVL_Item.h"


Key key_of_item(Item a){
	return a->name;
}

void destruct_item(Item* a){
	free((*a)->name);
	free((*a)->memory);
	free(*a);
}

int compare_keys(Key a, Key b){
	return strcmp(a,b);
}


