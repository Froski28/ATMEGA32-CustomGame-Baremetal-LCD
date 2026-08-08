/*
 * my_queue.h
 *
 * Created: 6/1/2025 1:33:01 PM
 *  Author: jujuf
 */ 


#ifndef MY_QUEUE_H_
#define MY_QUEUE_H_

#include "type_config.h"

#ifndef TYPE
#error "Type must be defined"
#define TYPE int
#endif

#ifndef SIZE
#define SIZE 100
#endif

typedef enum {false=0, true=1} Bool;

typedef struct queue
{
	int front, back, size;
	TYPE data[SIZE+1];
	void (*Queue_init)(struct queue*);
	void (*push)(struct queue*, TYPE item);
	void (*pop)(struct queue*);
	TYPE (*top)(struct queue*);
	Bool (*isFull)(struct queue*);
	Bool (*isEmpty)(struct queue*);
} Queue;

void Queue_init(Queue* q);

void pop(Queue* q);

void push(Queue* q, TYPE item);

TYPE top(Queue* q);

Bool isFull(Queue* q);

Bool isEmpty(Queue* q);



#endif /* MY_QUEUE_H_ */