/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2025 Jianping Duan <static.integer@hotmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Author nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */
#include "queue.h"

static struct queue_node * make_node(const void *, unsigned short);

/* Adds the key to this queue. */
void
enqueue(struct queue *qp, const void *key)
{
	struct queue_node *current;
	
	current = make_node(key, qp->keysize);
	if (qp->front == NULL)
		qp->front = current;
	else
		qp->rear->next = current;
	qp->rear = current;
	qp->size++;
}

/* 
 * Removes and returns the key on this queue that 
 * was least recently added. 
 */
void
dequeue(struct queue *qp, void **key)
{
	struct queue_node *oldfront;
	
	if (QUEUE_ISEMPTY(qp))
		return;
	
	if (qp->keysize == 0)
		*key = qp->front->key;
	else
		memmove(*key, qp->front->key, qp->keysize);
	oldfront = qp->front;
	qp->front = qp->front->next;
	if (qp->keysize != 0)
		ALGFREE(oldfront->key);
	ALGFREE(oldfront);
	
	/* queue not contains any key */
	if (--qp->size == 0)
		qp->rear = NULL;
}

/* Clears this queue. */
void
queue_clear(struct queue *qp)
{
	void *key = NULL;

	if (qp->keysize != 0)
		key = algmalloc(qp->keysize);

	while (!QUEUE_ISEMPTY(qp))
		dequeue(qp, &key);

	if (qp->keysize != 0)
		ALGFREE(key);
}

/******************** static function boundary ********************/

static struct queue_node * 
make_node(const void *key, unsigned short ksize)
{
	struct queue_node *current;
	
	current = (struct queue_node *)algmalloc(sizeof(struct queue_node));
	
	if (ksize == 0)
		current->key = (void *)key;
	else {
		current->key = algmalloc(ksize);
		memcpy(current->key, key, ksize);
	}
	
	current->next = NULL;
	
	return current;
}
