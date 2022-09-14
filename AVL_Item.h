struct variable{
	char* name;
	int* memory;
	int size;
};

#define NULLITEM ((struct variable*)NULL)
#define Item struct variable*
#define Key char*

Key key_of_item(Item a);
int compare_keys(Key a, Key b);
void destruct_item(Item* a);
