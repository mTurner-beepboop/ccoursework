#include <stdlib.h>
#include <string.h>
#include "tldlist.h"
/*
Created by James Campbell on 19/10/2020.
 SP Exercise 1a
 2398100c
 This is my own work as defined in the Academic Ethics agreement I have signed.
 The avl implementation was based on the provided java avl implementation.
 */

struct tldlist{
    TLDNode * root;
    Date * begin;
    Date * end;
    int count;
};

struct tldnode{
    int count;
    char * domain;
    int balance;
    int height;

    TLDNode * left;
    TLDNode * right;
    TLDNode * parent;

};
/*
 * struct for the tld iterator that collapses the tree into an array, then iterates over it
 */
//struct tlditerator{
//    int index;
//    int count;
//    TLDNode **nodearr;
//
//};

/*
 * struct for the tlditerator that just works out the next node, given the current one
 */
struct tlditerator{
    TLDNode *current;
};

/*
 * declarations for helper functions
 */
void tree_destroy(TLDNode *node);
TLDNode *nodefinder(TLDList *list, char *tld);
void rebalance(TLDList *tld, TLDNode * n);
TLDNode *rotate_left(TLDNode *a);
TLDNode *rotate_right(TLDNode *a);
TLDNode * rotate_left_right(TLDNode *n);
TLDNode * rotate_right_left(TLDNode *n);
int height(TLDNode * n);
void set_balance(TLDNode * n);
void reheight(TLDNode * n);
// recursive index is for the unused version of tlditerator
//int recursive_index(TLDNode ** narr, TLDNode *node, int index);

/*
 * tldlist_create generates a list structure for storing counts against
 * top level domains (TLDs)
 *
 * creates a TLDList that is constrained to the `begin' and `end' Date's
 * returns a pointer to the list if successful, NULL if not
 */
TLDList *tldlist_create(Date *begin, Date *end){
    TLDList *tlds;
    if ((tlds = (TLDList *)malloc(sizeof(TLDList))) != NULL){
        tlds->begin = begin;
        tlds->end = end;
        tlds->root = NULL;
    }
    return tlds;
}

/*
 * tldlist_destroy destroys the list structure in `tld'
 *
 * all heap allocated storage associated with the list is returned to the heap
 *
 * tree_destroy recursively frees the contents of the tree
 */
void tldlist_destroy(TLDList *tld){
    tree_destroy(tld->root);
    free(tld);
}
void tree_destroy(TLDNode *node){
    if (node != NULL){
        tree_destroy(node->left);
        tree_destroy(node->right);
        free(node->domain);
        free(node);
    }
}

/*
 * tldlist_add adds the TLD contained in `hostname' to the tldlist if
 * `d' falls in the begin and end dates associated with the list;
 * returns 1 if the entry was counted, 0 if not
 */
int tldlist_add(TLDList *tld, char *hostname, Date *d){
    /* check if the date falls in the range */
    if (date_compare(d, tld->begin) <0 || date_compare(tld->end, d) <0){
        return 0;
    }
    /* extract the tld from the hostname */

    char *domain = NULL;
    char * strptr = strrchr(hostname, '.')+1; // looks for the last . in the hostname and adds 1 to it to point to the first letter of the tld
    if (strptr == NULL){
        return 0;
    }
    /*
     * validate that the string is not 0 length, if it is then return 0 (fail)
     */
    if (strlen(strptr) == 0){
        return 0;
    }
    /* copy the tld into the new string */
    if ((domain = (char*)malloc(strlen(strptr) * sizeof(char)+1))!= NULL){
        strcpy(domain, strptr);
    } else {
        return 0;
    }

    /* check if there is already a node in the tree for this hostname and if so then increment it's count and return */

    TLDNode *node = nodefinder(tld, domain);
    if (node != NULL){
        node->count++;
        tld->count++;
        free(domain);
        return 1;
    }

    /* if there isn't one already then create a node, and insert it into the tree */

    if ((node = (TLDNode *)malloc(sizeof(TLDNode))) != NULL){
        node->count = 1;
        node->left = NULL;
        node->right = NULL;
        node->domain = domain;
        node->height = 0;

        /*
         * if the tree is empty then make the new node the root
         * else find out where the node should be, insert it there, and rebalance
         */

        if (tld->root == NULL){
            tld->root = node;
            node->parent = NULL;
        } else {
            TLDNode *current = tld->root;
            TLDNode *parent;
            int tldcmp;
            while (current != NULL){
                tldcmp = strcmp(domain, current->domain);
                parent = current;
                if (tldcmp < 0){
                    current = current->left;
                } else {
                    current = current->right;
                }

                if (current == NULL){ // we have arrived at the place we want to be
                    if (tldcmp < 0){
                        parent->left = node;
                        node->parent = parent;
                    } else {
                        parent->right = node;
                        node->parent = parent;
                    }
                    rebalance(tld, parent);
                    break;
                }
            }
        }

        tld->count++;
        return 1;
    }
    free(domain);
    return 0;
}

/*
 * nodefinder is a helper function that takes a tld, and returns a pointer to the node with that tld, NULL if not found
 */

TLDNode *nodefinder(TLDList *list, char *tld){
    TLDNode * current = list -> root;
    int tldcmp;
    while (current != NULL){
        tldcmp = strcmp(tld, current->domain);
        if (tldcmp == 0){
            break;
        } else if (tldcmp < 0){
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return current;
}

/*
 * Rebalance helper function to restore balance to the tree after a node is added or removed
 */

void rebalance(TLDList *tld, TLDNode * n){
    set_balance(n);
    // check if the balance is off
    if (n->balance == -2){ //the left subtree is too high
        if (height(n->left->left)>=height(n->left->right)){
            n = rotate_right(n);
        } else {
            n = rotate_left_right(n);
        }
    } else if (n->balance == 2){ // the right subtree is too high
        if (height(n->right->right)>=height(n->right->left)){
            n = rotate_left(n);
        } else {
            n = rotate_right_left(n);
        }
    }

    // after this node has been rebalanced, work upwards and continue rebalancing
    if (n->parent != NULL){
        rebalance(tld, n->parent);
    }
    else {
        tld->root = n;
    }
}
/*
 * rotate left and right are functions to reshuffle the tree to put it back into balance
 */
TLDNode *rotate_left(TLDNode *a){
    TLDNode *b = a->right;
    b->parent = a->parent;
    a->right = b->left;
    if (a->right != NULL){
        a->right->parent = a;
    }
    b->left = a;
    a->parent = b;
    if(b->parent != NULL){
        if (b->parent->right == a){
            b->parent->right = b;
        } else {
            b->parent->left = b;
        }
    }
    set_balance(a);
    set_balance(b);
    return b;
}

TLDNode *rotate_right(TLDNode *a){
    TLDNode *b = a->left;
    b->parent = a->parent;
    a->left = b->right;
    if (a->left != NULL){
        a->left->parent = a;
    }
    b->right = a;
    a->parent = b;
    if(b->parent != NULL){
        if (b->parent->right == a){
            b->parent->right = b;
        } else {
            b->parent->left = b;
        }
    }
    set_balance(a);
    set_balance(b);
    return b;
}

/*
 * helper functions rotate_left_right and rotate_right_left apply two rotations in a row
 */

TLDNode * rotate_left_right(TLDNode *n){
    n->left = rotate_left(n->left);
    return rotate_right(n);
}
TLDNode * rotate_right_left(TLDNode *n){
    n->right = rotate_right(n->right);
    return rotate_left(n);
}

/*
 * helper function height returns -1 if node is null, height of node otherwise
 */
int height(TLDNode * n){
    if (n == NULL){
        return -1;
    }
    return n->height;
}

/*
 *  helper function set balance sets the balance parameter of the node it's called on
 */
void set_balance(TLDNode * n){
    reheight(n);
    n->balance = height(n->right) - height(n->left);
}

/*
 * helper function reheight computes the height of the node it is called on
 */
void reheight(TLDNode * n){
    if (n != NULL){}
    n->height = 1 + ((height(n->left) > height(n->right)) ? height(n->left) : height(n->right)); // n->height = 1+ max(height(n->left), height(n->right))
}

/*
 * tldlist_count returns the number of successful tldlist_add() calls since
 * the creation of the TLDList
 */
long tldlist_count(TLDList *tld){
    return tld->count;
}

/*
 * tldlist_iter_create creates an iterator over the TLDList; returns a pointer
 * to the iterator if successful, NULL if not
 */
TLDIterator *tldlist_iter_create(TLDList *tld){
    TLDIterator *iter;
    if ((iter = (TLDIterator *)malloc(sizeof(TLDIterator))) != NULL){
        iter->current = tld->root;
    }
    return iter;
}
// unused version of itercreate for the array version
//TLDIterator *tldlist_iter_create(TLDList *tld){
//    TLDIterator *iter;
//    if ((iter = (TLDIterator *)malloc(sizeof(TLDIterator))) != NULL){
//        TLDNode * * narr;
//        if ((narr = (TLDNode **)malloc(sizeof(struct tldnode) * tld->count)) != NULL){
//            iter->index = 0;
//            iter->count = tld->count;
//            // add the nodes from tld to the array
//            recursive_index(narr, tld->root, 0);
//            iter->nodearr = narr;
//
//        } else {
//            return NULL;
//        }
//    }
//    return iter;
//}



/*
 *  recursive index is a helper function to add the elements of the tree to the iteration array, recursively
 *  unused for the node pointer iterator
 */
//
//int recursive_index(TLDNode ** narr, TLDNode *node, int index){
//    if (node == NULL){
//        return index;
//    }
//    *(narr + index) = node;
//    index++;
//    index = recursive_index(narr, node->left, index);
//    index = recursive_index(narr, node->right, index);
//    return index;
//}

/*
 * tldlist_iter_next returns the next element in the list; returns a pointer
 * to the TLDNode if successful, NULL if no more elements to return
 */

TLDNode *tldlist_iter_next(TLDIterator *iter){
    TLDNode *prev = iter->current;
    if (prev == NULL){
        return prev;
    } else if (prev->left != NULL){
        iter->current = prev->left;
    } else if (prev->right != NULL) {
        iter->current = prev->right;
    } else {
        TLDNode *n = iter->current;
        while (n != NULL){
            if (n->parent != NULL && n == n->parent->left) {
                if (n->parent->right != NULL){
                    iter->current = n->parent->right;
                    break;
                } else {
                    n = n->parent;
                }
            } else {
                n = n->parent;
            }
        }
        if (n == NULL) {
            iter->current = NULL;
        }
    }
    return prev;
}
// unused iter next for the array iter implementaion
//TLDNode *tldlist_iter_next(TLDIterator *iter){
//    TLDNode *n;
//    if (iter->index < iter->count){
//        n = iter->nodearr[iter->index];
//        iter->index++;
//    } else {
//        n = NULL;
//    }
//    return n;
//}

/*
 * tldlist_iter_destroy destroys the iterator specified by `iter'
 */

void tldlist_iter_destroy(TLDIterator *iter){
    free(iter);
}
// unused iter destroy for the array version of the iterator
//void tldlist_iter_destroy(TLDIterator *iter){
//    free(iter->nodearr);
//    free(iter);
//}

/*
 * tldnode_tldname returns the tld associated with the TLDNode
 */
char *tldnode_tldname(TLDNode *node){
    return node->domain;
}

/*
 * tldnode_count returns the number of times that a log entry for the
 * corresponding tld was added to the list
 */
long tldnode_count(TLDNode *node){
    return node->count;
}