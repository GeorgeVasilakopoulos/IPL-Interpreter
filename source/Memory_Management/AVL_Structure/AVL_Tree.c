#include <stdlib.h>
#include "AVL_Item.h"
#include "AVL_Tree.h"
#define MAX(x, y) (((x) > (y)) ? (x) : (y))



void AVL_Initialize(AVL_Tree* Tree){
	*Tree = NULL;
}


static int balance_factor(struct AVLnode* node){
	int height_of_right;
	int height_of_left;
	if(node->right == NULL)height_of_right = 0;
	else height_of_right = node->right->height + 1;

	if(node->left == NULL)height_of_left = 0;
	else height_of_left = node->left->height + 1;

	return height_of_right - height_of_left;
}



static struct AVLnode* search_recursive(struct AVLnode* node, Key key){
	if(node == NULL)return NULL;
	int comparator = compare_keys(key, key_of_item(node->data));
	if(comparator < 0) return search_recursive(node->left, key);
	if(comparator > 0) return search_recursive(node->right, key);
	return node;
}


Item AVL_Search(AVL_Tree* Tree, Key key){
	struct AVLnode* node = search_recursive(*Tree,key);
	if(node == NULL){
		//Item not found
		return NULLITEM;
	}
	return (node->data);
}

static void fix_height(struct AVLnode* node){
	int height_of_left = (node->left == NULL) ? 0 : node->left->height;
	int height_of_right = (node->right == NULL) ? 0 : node->right->height;
	node->height = MAX(height_of_right,height_of_left) + 1;
}


static struct AVLnode* left_rotation(struct AVLnode* node){
	struct AVLnode* new_root = node->right; 
	node->right = new_root->left;
	new_root->left = node;
	fix_height(node);
	fix_height(new_root);
	return new_root;
}

static struct AVLnode* right_rotation(struct AVLnode* node){
	struct AVLnode* new_root = node->left;
	node->left = new_root->right;
	new_root->right = node;
	fix_height(node);
	fix_height(new_root);
	return new_root;
}


static struct AVLnode* rebalance(struct AVLnode* node){
	if(balance_factor(node) == -2){
		if(balance_factor(node->left) == 1){
			node->left = left_rotation(node->left);
			fix_height(node);
		}
		return right_rotation(node);
	}
	if(balance_factor(node) == 2){
		if(balance_factor(node->right) == -1){
			node->right = right_rotation(node->right);
			fix_height(node);
		}
		return left_rotation(node);
	}
	return node;
}



static struct AVLnode* insert_recursive(struct AVLnode* node, Item data){
	if (node == NULL){
		node = malloc(sizeof(struct AVLnode));
		node->left = NULL;
		node->right = NULL;
		node->data = data;
		node->height = 0;
		return node;
	}
	int comparator = compare_keys(key_of_item(data),key_of_item(node->data));
	if(comparator < 0){
		node->left = insert_recursive(node->left,data);
		fix_height(node);
		return rebalance(node);

	}
	else if(comparator > 0){
		node->right = insert_recursive(node->right,data);
		fix_height(node);
		return rebalance(node);
	}
	else{
		// Here, there already exists an item with the same key...
		return node;
	}

}


void AVL_Insert(AVL_Tree* Tree, Item data){
	*Tree = insert_recursive(*Tree, data);
}



static struct AVLnode* delete_rightmost(struct AVLnode* node, struct AVLnode* deleted_node){
	if(node->right == NULL){
		deleted_node->data = node->data;
		destruct_item((Item*)&(node->data));
		free(node);
		return NULL;
	}
	node->right = delete_rightmost(node->right,deleted_node);
	fix_height(node);
	return rebalance(node);
}

static struct AVLnode* delete_leftmost(struct AVLnode* node, struct AVLnode* deleted_node){
	if(node->left == NULL){
		deleted_node->data = node->data;
		destruct_item((Item*)&(node->data));
		free(node);
		return NULL;
	}
	node->left = delete_leftmost(node->left,deleted_node);
	fix_height(node);
	return rebalance(node);
}



static struct AVLnode* delete_recursive(struct AVLnode* node, Key key){
	if(node == NULL){
		//not found
		return NULL;
	}
	int comparator = compare_keys(key,key_of_item(node->data)); 
	if(comparator == 0){
		if(node->left == NULL){
			if(node->right == NULL){
				destruct_item((Item*)&(node->data));
				free(node);
				return NULL;
			}
			node->right = delete_leftmost(node->right,node);
		}
		else node->left = delete_rightmost(node->left,node);
		fix_height(node);
		return rebalance(node);
	}
	else if(comparator < 0){
		node->left = delete_recursive(node->left,key);
		fix_height(node);
		return rebalance(node);
	}
	else{
		node->right = delete_recursive(node->right,key);
		fix_height(node);
		return rebalance(node);
	}
}



void AVL_Delete(AVL_Tree* Tree, Key key){
	*Tree = delete_recursive(*Tree, key);
}



static void visit_inorder_recursive(struct AVLnode* node,void (*visit)(Item)){
	if(node == NULL){
		return;
	}
	visit_inorder_recursive(node->left, visit);
	visit(node->data);
	visit_inorder_recursive(node->right, visit);
}



void AVL_Visit_Inorder(AVL_Tree* Tree, void (*visit)(Item)){
	visit_inorder_recursive(*Tree, visit);
}

static void destruct_recursive(struct AVLnode* node){
	if(node == NULL)return;
	destruct_recursive(node->left);
	destruct_recursive(node->right);
	destruct_item((Item*)&(node->data));
	free(node);
}


void AVL_Destruct(AVL_Tree* Tree){
	destruct_recursive(*Tree);
}



