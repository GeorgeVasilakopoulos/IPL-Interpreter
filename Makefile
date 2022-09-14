ipli : AVL_Item.o AVL_Tree.o error_handling.o expressions.o instruction_block.o line_reader.o main.o memory_management.o
	gcc AVL_Item.o AVL_Tree.o error_handling.o expressions.o instruction_block.o line_reader.o main.o memory_management.o -o ipli

AVL_Item.o: AVL_Item.c AVL_Item.h
	gcc -c AVL_Item.c

AVL_Tree.o: AVL_Tree.c AVL_Tree.h AVL_Item.h
	gcc -c AVL_Tree.c

error_handling.o: error_handling.c error_handling.h
	gcc -c error_handling.c

expressions.o: expressions.c line_reader.h memory_management.h expressions.h error_handling.h
	gcc -c expressions.c

instruction_block.o: instruction_block.c line_reader.h memory_management.h expressions.h error_handling.h
	gcc -c instruction_block.c

line_reader.o: line_reader.c line_reader.h error_handling.h
	gcc -c line_reader.c 

main.o: main.c instruction_block.h
	gcc -c main.c 

memory_management.o: memory_management.c AVL_Item.h AVL_Tree.h
	gcc -c memory_management.c 


clean: 
	rm ipli AVL_Item.o AVL_Tree.o error_handling.o expressions.o instruction_block.o line_reader.o main.o memory_management.o

