#include <cstddef>
#include <iostream>

#include "d_node.h"

d_node::d_node() {
	initialized = false;
	left = NULL;
	right = NULL;
}

d_node::d_node(char c, int l, d_node* p) {
	initialized = true;
	byte = c;
	label = l;
	parent = p;
	left = NULL;
	right = NULL;
}

/* Searches for a determined node value in the dictionary tree */
d_node* d_node::find_entry(char c){
	if 	((&right) == NULL) return NULL;
	d_node * cur = right;
	while (cur != NULL) {
		if 	(cur->byte == c) return cur;
		cur = cur->left;
	}
	return NULL;
}

/* Adds a new value to the dictionary tree */
d_node* d_node::insert_entry(char byte, int label){
	if(right == NULL || !right->initialized) {
		right = new d_node(byte,label,this);
		return right;
	} else {
		d_node * cur = right;
		while(cur->left != NULL && cur->left->initialized) {
			cur = cur->left;
		}
		cur->left = new d_node(byte,label,this);
		return cur->left;
	}
}