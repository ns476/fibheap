/* fh.h - Fibonacci heap implementation
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

#ifndef FH_H
#define FH_H

/* Circular doubly-linked list data structure */

#define max(a,b) ( ( (a) > (b) ) ? (a) : (b) )

typedef struct fh {
	struct fh *prev;
	struct fh *next;
	struct fh *parent;
	struct fh *children;
	int marked : 1;
	int degree;
	int val;
} fh;

/* pointers returned by these functions are to the new fibonacci heap */

fh *fh_insert(fh *head, int val, fh **newNode);
fh *fh_merge(fh *head1, fh *head2);
#define fh_findMin(h) ((h))
fh *fh_extractMin(fh *head);
fh *fh_decreaseKey(fh *head,
		   fh *node,
		   int new);
#endif
