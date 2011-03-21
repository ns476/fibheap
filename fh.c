/* fh.c - Fibonacci heap implementation
   Copyright (c) 2011 Nicholas Skehin <ns476@cam.ac.uk>
   
   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:
   
   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
   LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
   OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
   WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#include "fh.h"
#include <stdlib.h>
#include <string.h>

#ifndef RUN_FH_TEST
#define RUN_FH_TEST 1
#endif

fh *
fh_insert(fh *head,
	  int val,
	  fh **newNode)
{
	fh *new = (fh *) malloc(sizeof (fh));
	memset(new, 0, sizeof (fh));
	if (head == NULL) {
		new->prev = new;
		new->next = new;
		new->val = val;
		new->parent = NULL;
		new->children = NULL;
		new->degree = 0;
		return new;
	} else {
		new->next = head->next;
		new->prev = head;
		head->next = new;
		new->next->prev = new;
		new->val = val;
		new->parent = head->parent;
		new->children = NULL;
		new->degree = 0;
		if (newNode != NULL) {
			*newNode = new;
		}
		if (head->val < val) {
			return head;
		} else {
			return new;
		}
	}
}

static fh *
fh_find_min_sibling(fh *node)
{
	int min;
	fh *minNode;
	fh *cur = node->next;

	if (node->next == node)
		return NULL;
	
	min = node->next->val;
	minNode = node->next;

	while (cur != node) {
		if (cur->val < min) {
			min = cur->val;
			minNode = cur;
		}
		cur = node->next;
	}
	return minNode;
}

static fh *
fh_merge_lists (fh *head1,
		fh *head2)
{
	fh *left_splice = head1;
	fh *right_splice = head1->next;

	fh *left_in = head2;
	fh *right_in = head2->prev;

	left_splice->next = left_in;
	left_in->prev = left_splice;

	right_splice->prev = right_in;
	right_in->next = right_splice;

	if (head1->val < head2->val) {
		return head1;
	} else {
		return head2;
	}
}

static void
fh_splice_node (fh *node)
{
	node->prev->next = node->next;
	node->next->prev = node->prev;
}

static int
fh_count_trees(fh *head)
{
	int i = 1;
	fh *cur = head->next;
	if (head == NULL)
		return 0;
	while (cur != head) {
		cur = cur->next;
		i++;
	}
	return i;
}

static int
fh_max_degree (fh *head)
{
	if (head == NULL)
		return 0;
	fh *max = head;
	fh *cur = head->next;
	while (cur != head) {
		if (cur->degree > max->degree)
			max = cur;
		cur = cur->next;
	}

	return max->degree;
}

static fh *
fh_merge_trees (fh *t1,
		fh *t2)
{
	fh *parent;
	fh *child;
	
	if (t1->val < t2->val) {
		parent = t1;
		child = t2;
	} else {
		parent = t2;
		child = t1;
	}
	child->parent = parent;
	if (parent->children) {
		parent->children->next->prev = child;
		child->next = parent->children->next;
	} else {
		parent->children = child;
	}
	parent->children->next = child;
	child->prev = parent->children;

	if (parent->children->next->val < parent->children->val) {
		parent->children = parent->children->next;
	}

	parent->degree++;
	return parent;
}

static void 
fh_merge_into_array(fh **degrees,
		    fh *node)
{
	node->next = node;
	node->prev = node;
	node->parent = NULL;
	if (degrees[node->degree] == NULL) {
		degrees[node->degree] = node;
	} else {
		int degree = node->degree;
		node = fh_merge_trees (node, degrees[node->degree]);
		degrees[degree] = NULL;
		fh_merge_into_array(degrees, node);
	}
}

fh *
fh_extractMin (fh *head)
{
	fh *nextNode;
	fh *cur;
	fh *tmp;

	int i, arrayLen;
	
	int len, maxDeg;

	nextNode = head->next;
	fh_splice_node(head);


	if (nextNode == head) {
		if (head->children == NULL)
			return NULL;
		else
			nextNode = head->children;
	} else {
		nextNode->prev = head->prev;
		nextNode->next = head->next->next;
		if (head->children != NULL) {
			nextNode = fh_merge_lists (head->children, nextNode);
		}
	}
	
	free(head);

	head = nextNode;

	if (head->next == head)
		return head;
	
	len = fh_count_trees(head);
	maxDeg = fh_max_degree(head);
	arrayLen = len + maxDeg;

	fh *degrees[arrayLen];
	memset(degrees, 0, sizeof(fh *) * (arrayLen)); 

	cur = head->next;

	do {
		tmp = cur;
		cur = cur->next;
		fh_merge_into_array (degrees, tmp);
	} while (cur != head);

	fh_merge_into_array(degrees, head);

	head = NULL;
	
	for (i = 0; i < arrayLen; i++) {
		if (degrees[i] != NULL) {
			if (head == NULL) {
				head = degrees[i];
			} else {
				head = fh_merge_lists(head, degrees[i]);
			}
		}
	}
	
	return head;
}

static fh *
fh_cut (fh *head,
	fh *node)
{
	if (node->parent != NULL) {
		if (node->parent->marked == 0) {
			node->parent->marked = 1;
		} else {
			head = fh_cut(head, node->parent);
		}
	}
	if (node->next == node) {
		node->parent->children = NULL;
	} else {
		node->parent->children = fh_find_min_sibling (node);
	}

	node->parent = NULL;
	fh_splice_node(node);
	node->marked = 0;
	node->next = node;
	node->prev = node;

        return fh_merge_lists (head, node);
}

fh *
fh_decreaseKey(fh *head,
	       fh *node,
	       int new)
{
	node->val = new;
	if (node->parent && node->val < node->parent->val) {
		fh_cut(head, node);
	}
	if (node->val < head->val) {
		return node;
	}
	return head;
}

void
fh_cll_foreach (fh *list,
		void (*fun) (int))
{
	fh *new;

	if (list == NULL)
		return;

	fun(list->val);
	
	new = list->next;
	
	while (new != list) {
		fun(new->val);
		new = new->next;
	}
}

#if RUN_FH_TEST
#include <stdio.h>
#include <time.h>

int
main (int argc,
      char *argv[])
{
	fh *final;
	fh *fh = NULL;
	int i;
	
	srand ((unsigned int) time (NULL));

	for (i = 0; i < 10000; i++) {
		fh = fh_insert (fh, rand(), &final);
	}

	fh = fh_decreaseKey(fh, final, 1);
	while (1) {
		if (fh != NULL) {
			printf("%d\n", fh_findMin(fh)->val);
			fh = fh_extractMin(fh);
		} else break;
	}
	return 0;
}
#endif
