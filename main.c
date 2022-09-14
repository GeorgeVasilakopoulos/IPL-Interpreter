#include <stdio.h>
#include "memory_management.h"



int main(void){
	Initialize_Memory();
	Construct_Array("bla", 10);
	for(int i=0;i<10;i++){
		Assign_Value("bla", i, i);
	}
	for(int i=0;i<10;i++){
		int k;
		Get_Value("bla", i, &k);
		printf("%d ",k);
	}
	printf("\n");
	Destruct_Array("bla");

	return 0;
}