#ifndef __LINKSTACK__
#define __LINKSTACK__

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "bombs.h"

typedef struct node{
  bomb data;
  struct node * nxt;
}node;

typedef void (*callback)(node*);

node* create_node(node*, bomb b);
node* push_node(node*, bomb b);
node* pop_node(node*, bomb* b);
void iterate(node*, callback);
void delete_list(node**);
int is_empty(node*);

#endif
