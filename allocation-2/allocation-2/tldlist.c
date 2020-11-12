/*
 * DAVID MACNEILL 
 * 2381795m 
 * SP Exercise 1a
 * This is my own work as defined in the Academic 
 * Ethics agreement I have signed.
 *
 */
#include "date.h"
#include "tldlist.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/*
 * Included ctype.h for function "tolower()"
 * which takes a char and returns the lowercase
 * char if uppercase and returns the input if 
 * already lowercase.
 */

struct tldlist{struct tldnode *root; long count;
	Date *begin;
	Date *end;};
struct tldnode{struct tldnode *left;
	struct tldnode *right;
	struct tldnode *parent;
	char *domain;
	long count; 
	int balance,height;};
struct tlditerator{struct tldnode *node;
	int first;
	struct tldnode *root;};

TLDList *tldlist_create(Date *begin,Date *end){
	TLDList * list_ptr = malloc(sizeof(struct tldlist));
	if(list_ptr!=NULL){
		list_ptr->count = 0;
		
		list_ptr->root = NULL;
		list_ptr->begin = date_duplicate(begin);
		list_ptr->end = date_duplicate(end);
		
		return list_ptr;
	}
	else{
		return NULL;
	}
}

/* 
 * destroy_children frees all memory allocated each node and
 * recursively does all subsequent children
 * eventually returning only the root of the tree
 */
TLDNode *destroy_children(TLDNode *node){
	while(node!=NULL){
		if((node->left = NULL) && (node->right = NULL) && (node->parent!=NULL)){
			free(node->domain);
			free(node);
			node = NULL;
			return node;
		}else if (node->left != NULL){
			node->left  = destroy_children(node->left);
			return node;;
		}else if (node->right != NULL){
			node->right = destroy_children(node->right);
			return node;	
		}else{
			free(node->domain);
			free(node);
			node = NULL;
			return node;	
		}
	}
	return NULL;
}

void tldlist_destroy(TLDList *tld){
	while(tld->root !=NULL){tld->root = destroy_children(tld->root);}
	free(tld->begin);
	free(tld->end);
	free(tld);		
}
/*
 * get_tld takes a character string in and returns the 
 * string of the tld in lower case.
 */
char *get_tld(char *hostname){
	int len = strlen(hostname);
	int i= len-1;
	while((hostname[i]!='.')&&(i>=0))
		{i=i-1;};
	char * domain = malloc(sizeof(char *[4]));
	domain[0] = tolower(hostname[i+1]);
	domain[1] = tolower(hostname[i+2]);
	if(i==len-4){
		domain[2] = tolower(hostname[i+3]);
	}
	return domain;	
}

/*
 * new_node take in a domain, height and a parent node
 * and returns a pointer to a new node with the appropriate
 * fields
 */
TLDNode *new_node(char *domain, TLDNode *parent){
	TLDNode *node = malloc(sizeof(TLDNode));
	if(node != NULL){
		node->count = 1;
		node->domain = domain;
		node->parent = parent;
		node->balance = 0;
		node->height = 0;
	}
	return node;	
}
/*
 * reheight takes in a node and updates its height
 */
void reheight(TLDNode *node){
	if(node!=NULL){
		if((node->left==NULL)&&(node->right !=NULL)){
			node->height = node->right->height +1;
		}else if((node->right==NULL)&&(node->left!=NULL)){
			node->height = node->left->height+1;
		}else if((node->right==NULL)&&(node->left==NULL)){
			node->height = 1;
		}else if(node->right->height > node->left->height){
			node->height = node->right->height +1;
		}else{
			node->height = node->left->height +1;	
		
		}
	}
}

int height(TLDNode *n){
	if(n==NULL){
		return -1;
	}
	return n->height;
}

/*
 * set_balance takes in a node and reheights it and sets the nodes balance
 */
void set_balance(TLDNode *node){
	reheight(node);
	node->balance =height(node->right) - height(node->left);
}

TLDNode *rotate_right(TLDNode *a){
	TLDNode *b = a->left;
	b->parent = a->parent;
	a->left = b->right;
	
	if(a->left!=NULL){
		a->left->parent=a;
	}
	
	b->right = a;
	a->parent = b;
	
	if(b->parent!=NULL){
		if(b->parent->right==a){
			b->parent->right = b;
		}else{
			b->parent->left = b;
		}
	}
	set_balance(a);
	set_balance(b);
	
	return b;
}

TLDNode *rotate_left(TLDNode *a){
	TLDNode *b = a->right;
	b->parent = a->parent;
	
	a->right = b->left;
	
	if(a->right!=NULL){
		a->right->parent=a;
	}
	
	b->left = a;
	a->parent = b;
	
	if(b->parent!=NULL){
		if(b->parent->right==a){
			b->parent->right = b;
		}else{
			b->parent->left = b;
		}
	}
	set_balance(a);
	set_balance(b);
	
	return b;
}

TLDNode *rotate_left_then_right(TLDNode *n){
	n->left = rotate_left(n->left);
	return rotate_right(n);
}

TLDNode *rotate_right_then_left(TLDNode *n){
	n->right = rotate_right(n->right);
	return rotate_left(n);
}
/*
 * rebalance takes in a parent node and rebalances it
 */

void rebalance(TLDNode *n, TLDList *tld){
	set_balance(n);
	
	if(n->balance ==-2){
		if(height(n->left->left)>=height(n->left->right)){
			n = rotate_right(n);
		}else{
			n = rotate_left_then_right(n);
		}
		
	}else if(n->balance == 2){
		if(height(n->right->right)>=height(n->right->left)){
			n = rotate_left(n);
		}else{
			n = rotate_right_then_left(n);
		}
	}	
	
	if(n->parent!=NULL){
		rebalance(n->parent,tld);
	}else{
		tld->root = n; 
	}

}
/*
 * tldlist_add has been impleneted following the avl tree java code
 * provided generally with a few changes.
 * the changes include the tldlist attribute count is increased
 * whenever a new log is succesfully added to the list
 * also if the specific tld already exists in the AVL
 * its count is increased. 
 */
int tldlist_add(TLDList *tld, char *hostname, Date *d){	
	if(((date_compare(d,tld->begin))>=0)&&(date_compare(d,tld->end)<=0)){
		char *domain = get_tld(hostname);
		if(tld->root==NULL){
			tld->root = new_node(domain, NULL);
			tld->count++;
			return 1;
		}
		TLDNode *n = tld->root;
		
		while(1==1){
			int comp = strcmp(domain,n->domain);
			if(comp == 0){
				n->count++;
				tld->count++;
				return 1;
			}
			TLDNode *parent = n;
			if(comp >0){
				n = n->right;	
			}else{
				n = n->left;
			}
			if(n==NULL){
				if(comp<0){
					parent->left=new_node(domain,parent);
					tld->count++;
				}else{
					parent->right=new_node(domain,parent);
					tld->count++;
				}
				rebalance(parent, tld);
				return 1;
			}
		}
		
	}
	return 0;
}


long tldlist_count(TLDList *tld){
	return tld->count;
}
 
TLDIterator *tldlist_iter_create(TLDList *tld){
	TLDIterator *start = malloc(sizeof(struct tlditerator));
	if(start!=NULL){
		start->node = tld->root;
		start->first =1;
		while(start->node->left!=NULL){
			start->node = start->node->left;	
		}
		start->root = tld->root;
	}
	return start;
}
TLDNode *min_node(TLDNode* n){
	TLDNode *curr = n;
	while(curr->left!=NULL){
		curr = curr->left;
	}
	return curr;	
}	

TLDNode *tldlist_iter_next(TLDIterator *iter){
	if(iter!=NULL){
		if(iter->node!=NULL){
			if(iter->first==1){
				iter->first=0;
				return iter->node;
			}
			if(iter->node->right!=NULL){
				iter->node = min_node(iter->node->right);
				return iter->node;
			}
			TLDNode *p = iter->node->parent;
			while((p!=NULL)&&(iter->node==p->right)){
				iter->node = p;
				p = p->parent;
			}
			iter->node=p;
			return p;
		}
	}
	return NULL;
}
void tldlist_iter_destroy(TLDIterator *iter){
	free(iter);
}

char *tldnode_tldname(TLDNode *node){
	return node->domain;
}

long tldnode_count(TLDNode *node){
	return node->count;
}
