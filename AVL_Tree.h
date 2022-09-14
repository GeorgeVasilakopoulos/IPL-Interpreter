
struct AVLnode{
	Item data;
	int height;
	struct AVLnode* left;
	struct AVLnode* right;
};

typedef struct AVLnode* AVL_Tree;

void AVL_Initialize(AVL_Tree* Tree);
Item AVL_Search(AVL_Tree* Tree, Key key);
void AVL_Insert(AVL_Tree* Tree, Item data);
void AVL_Delete(AVL_Tree* Tree, Key data);
void AVL_Visit_Inorder(AVL_Tree* Tree, void (*visit)(Item));
void AVL_Destruct(AVL_Tree* Tree);


