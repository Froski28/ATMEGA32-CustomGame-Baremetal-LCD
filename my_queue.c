/*
 * my_queue.c
 *
 * Created: 6/1/2025 1:33:16 PM
 *  Author: jujuf
 */

#include "my_queue.h"

void Queue_init(Queue* q){
	q->front = 0;
	q->back = 0;
	q->size = 0;
	q->isEmpty = isEmpty;
	q->isFull = isFull;
	q->push = push;
	q->pop = pop;
	q->top = top;
	q->Queue_init = Queue_init;
}

Bool isFull(Queue* q){
	return (q->size == SIZE) ? true: false;
}

Bool isEmpty(Queue* q){
	return (q->size > 0) ? false: true;
}

void pop(Queue* q){
	if (isEmpty(q)) return;
	++q->front;
	q->front %= SIZE;
}

void push(Queue* q, TYPE item){
	if (isFull(q)) return;
	q->data[q->back] = item;
	q->back = (q->back+1) % SIZE;
	++q->size;
}

TYPE top(Queue* q){
	return q->data[q->front];
}



