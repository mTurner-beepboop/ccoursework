#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tldlist.h"

//
//Structure definitions
//

struct tldlist{
	long count; //add if successful add call, initially 0
	Date *begin; //begin date
	Date *end; //end date
	TLDNode *root;	
};

struct tldnode{
	char *tldname; //top level domain associated with node
	long count; //Amount of times top level domain has appeared
	int balance; //balance is calculated from height of children
	int height;
	TLDNode *left;
	TLDNode *right;
	TLDNode *parent;
};

struct tlditerator{
	TLDNode *current;
};

//function definitions for those not in header
TLDNode *tldnode_create();
void tldlist_destroy_recursive();
void tldnode_destroy();
TLDNode *search_tldlist();
void rebalance();
int get_max();
void setBalance();
TLDNode *rotateRight();
TLDNode *rotateLeft();
TLDNode *rotateRightThenLeft();
TLDNode *rotateLeftThenRight();

void print_node(TLDNode *node){
	if (node == NULL){
		printf("Node is null\n");
		return;
	}
	printf("tldname: %5s, count: %ld, balance: %d, height: %d\n",node->tldname, node->count, node->balance, node->height);
}

//
//TLDList main methods
//

//DONE
TLDList *tldlist_create(Date *begin, Date *end){
	TLDList *list_ptr = malloc(sizeof(TLDList)); //Allocate memory
	list_ptr->begin = begin; list_ptr->end = end; //Set up bounding dates
	list_ptr->count = 0; list_ptr->root = NULL; //Initialise empty list

	return list_ptr; //if malloc fails, will return null
}

//Debug this, otherwise prog done I think
int tldlist_add(TLDList *tld, char *hostname, Date *d){
	//first check that the date is within bounds
	if(date_compare(d, tld->begin) == -1){
		return 0; //In this case, the date is before the start date
	}
	if(date_compare(d, tld->end) == 1){
		return 0; //In this case, the date is past the end date
	}

	//parse the hostname to get the tld
	int len = strlen(hostname);
	char tld_name[4];
	if (hostname[len-3]=='.'){//If the tld has 2 letters
		tld_name[0] = hostname[len-2];
		tld_name[1] = hostname[len-1];
		tld_name[2] = '\0';
	}
	else{
		tld_name[0] = hostname[len-3];
		tld_name[1] = hostname[len-2];
		tld_name[2] = hostname[len-1];
		tld_name[3] = '\0';
	}
	
	//check if root empty
	if (tld->root==NULL){	
		TLDNode *new = tldnode_create(NULL, tld_name); //Root empty so this becomes new root
		tld->root = new;
		print_node(tld->root);
		printf("Root made\n");
		return 1; //Root was filled with new node
	}

	//Check passes, check if tld is already a node, if so, add count and return 1
	printf("Searching with: %10s, %10s\n",tld->root->tldname, tld_name);
	TLDNode *search = search_tldlist(tld->root, tld_name);
	print_node(search);
	if (search != NULL){
		printf("Existing node found, adding to count\n");
		search->count = search->count + 1;
		tld->count = tld->count+1;
		return 1; //In this case, an existing node was found and the count was increased
	}
	printf("Existing node does not match, creating new node\n");

	//tld not a node currently, create one and link to the tree
	//Traverse the tree for a parent for the child
	TLDNode *iter = tld->root;//Since I need to change the parent node, reference the node directly instead of using iterator
	while(iter!=NULL){
		//If node belongs on left og the current iterated node
		if (strcmp(iter->tldname,tld_name)<0){
			if (iter->left!=NULL){
				iter = iter->left;
				continue;
			}
			//Will only reach here if the left node of the iterator is empty and this node belongs on the left
			TLDNode *new = tldnode_create(iter,tld_name);//Create new node with parent in tree
			iter->left = new;//Link to tree
			rebalance(tld,iter);//Rebalance the parent of new node
			tld->count = tld->count+1;
			return 1;
		}
		//If node belongs on right of the current iterated node
		if (iter->right!=NULL){
			iter = iter->right;
			continue;
		}
		TLDNode *new = tldnode_create(iter,tld_name);//Create new node with parent in tree
		iter->right = new;//Link to tree
		rebalance(tld,iter);//rebalance on parent of new node
		tld->count = tld->count+1;
		return 1;
	}
	//This should never be called
	return 0;
}

//DONE
long tldlist_count(TLDList *tld){
	return tld->count;
}

//DONE
long tldnode_count(TLDNode *node){
	return node->count;
}

//Will perform an inorder traversal, hence will start at leftmost leaf - DONE
TLDIterator *tldlist_iter_create(TLDList *tld){
	TLDIterator *iter = malloc(sizeof(TLDIterator));
	TLDNode *temp = tld->root;
	while(temp->left!=NULL){
		temp = temp->left;
	}
	iter->current = temp;//Set the current node in the iterator as the bottom left one
	return iter;
}

//Inorder traversal, find the next node in the sequence, return the current and switch to the next - DONE
TLDNode *tldlist_iter_next(TLDIterator *iter){
	//Log node to be returned
	TLDNode *ret = iter->current;

	//Set null case
	if (iter->current == NULL){
		return NULL;
	}
	
	while(iter->current != NULL){
		if (iter->current->left == NULL){
			iter->current = iter->current->right;
			return ret;
		}
		else{
			//Find predecessor of the current node
			TLDNode *predecessor = iter->current->left;
			while(predecessor->right != NULL && predecessor->right != iter->current){
				predecessor = predecessor->right;
			}
			//Make the current iterated node the right child of the predecessor
			if (predecessor->right == NULL){
				predecessor->right = iter->current;
				iter->current = iter->current->left;
			}
			//revert changes to restore original connections to node
			else{
				predecessor->right = NULL;
				iter->current = iter->current->right;
				return ret;
			}
		}
	}
	return NULL;//If here is reached, the current node of the iterator will be null, hence the end of the tree has been reached
}

//DONE
char *tldnode_tldname(TLDNode *node){
	return node->tldname;
}

//
//Helper methods
//

//This exclusively creates a new node, will need to add as child, calc balance etc seperately - DONE
TLDNode *tldnode_create(TLDNode *parent, char *tldname){
	TLDNode *node_ptr = malloc(sizeof(TLDNode));
	node_ptr->count = 1;
	memcpy(node_ptr->tldname, tldname, 4); //must explicitly copy the tldname otherwise it will be the pointed created in add
	if (parent != NULL){
		node_ptr->height = parent->height+1;
		node_ptr->parent = parent;
	}
	else {
		node_ptr->height = 1;
		node_ptr->parent = NULL;
	}
	node_ptr->balance = 0; //Since this is a new node, it will have no children
	return node_ptr;
}

//Recursively find the height of the node - DONE
int tldnode_height(TLDNode *node){
	if (node != NULL){
		return 1 + get_max(tldnode_height(node->left),tldnode_height(node->right)); 	
	}
	return 0;
}

//Helper to finx max int values - DONE
int get_max(int val1, int val2){
	if (val1 > val2){
		return val1;
	}
	return val2;
}

//Search list for a tld name, returns either a node with the matching search name, or NULL - DONE
TLDNode *search_tldlist(TLDNode *node, char *tldname){
	while(node != NULL){
		int compare = strcmp(node->tldname,tldname);
		printf("Compare strings %10s, %10s \n",node->tldname, tldname);
		printf("Compare val: %d\n",compare);
		if (compare == 0){
			return node;
		}else if (compare > 0){
			node = node->right;
		}else if (compare < 0){
			node = node->left;
		}
	}
	return NULL;
}

//
//Balance operations
//

//Recalculate the height of a node - DONE
void reheight(TLDNode *n){
	if(n!=NULL){
		n->height = tldnode_height(n);
	}
}

//Balance the tree from node n - DONE
void rebalance(TLDList *tld, TLDNode *n){
	//Update the balance of node n
	setBalance(n);
	
	//Check if too heavy on left
	if (n->balance == -2) {
		//Find which side of the left subtree is too heavy and rotate to fix
		if (n->left->left->height >= n->left->right->height){
			n = rotateRight(n);
		}
		else{
			n = rotateLeftThenRight(n);
		}
	}
	//Check if too heavy on right
	else if (n->balance == 2){
		//Find which side of the right subtree is too heavy and rotate to fix
		if (n->right->right->height >= n->right->left->height){
			n = rotateLeft(n);
		}
		else{
			n = rotateRightThenLeft(n);
		}
	}
	
	//Check parent of node n	
	if (n->parent != NULL){
		rebalance(tld,n->parent);//If n has a parent, rebalance it
	}
	else {
		tld->root = n;//n has no parent, so must be the tree root	
	}
}

//Recalculate the balance of a node - DONE
void setBalance(TLDNode *n){
	reheight(n);
	n->balance = n->right->height - n->left->height; 
}

//Below are rotation funcs - ALL DONE
TLDNode *rotateLeft(TLDNode *node1){
	TLDNode *node2 = node1->right;
	node2->parent = node1->parent;

	node1->right = node2->left;

	if (node1->right != NULL){
		node1->right->parent = node1;
	}
	
	node2->left = node1;
	node1->parent = node2;

	if (node2->parent != NULL){
		if (node2->parent->right == node1){
			node2->parent->right = node2;
		}
		else{
			node2->parent->left = node2;
		}
	}
	
	setBalance(node1);
	setBalance(node2);

	return node2;
}

TLDNode *rotateRight(TLDNode *node1){
	TLDNode *node2 = node1->left;
	node2->parent = node1->parent;

	node1->left = node2->right;

	if (node1->left != NULL){
		node1->left->parent = node1;
	}

	node2->right = node1;
	node1->parent = node2;

	if (node2->parent != NULL){
		if (node2->parent->right == node1){
			node2->parent->right = node2;
		}
		else{
			node2->parent->left = node2;
		}
	}

	setBalance(node1);
	setBalance(node2);
	
	return node2;
}

TLDNode *rotateLeftThenRight(TLDNode *n){
	n->left = rotateLeft(n);
	return rotateRight(n);
}

TLDNode *rotateRightThenLeft(TLDNode *n){
	n->right = rotateRight(n->left);
	return rotateLeft(n);
}
//
//Memory free methods
//

//DONE
void tldlist_destroy(TLDList *tld){
	tldlist_destroy_recursive(tld->root);
	free(tld);//Destroy the tree itself
}

//destroy the tree using inorder traversal - DONE
void tldlist_destroy_recursive(TLDNode *node){
	if (node != NULL){
		tldlist_destroy_recursive(node->left);
		tldlist_destroy_recursive(node->right);
		tldnode_destroy(node);
	}
}

//DONE
void tldlist_iter_destroy(TLDIterator *iter){	
	free(iter);
}

//DONE
void tldnode_destroy(TLDNode *node){
	free(node);
}
