//Jacob Ackerman and Josh Lau

#include <stdio.h>
#include <stdlib.h>
#include "pcb.h"
#include "fifoqueue.h"
#include "priority_queue.h"

#define QUEUE_SIZE 16

//This is used for the number of priority levels
FIFO_q_p* priority_queue()
{
	FIFO_q_p* ready_q = malloc(sizeof(FIFO_q_p) * QUEUE_SIZE);
	int i = 0;
	for(i; i < QUEUE_SIZE; i++)
	{
		ready_q[i] = fifo_q_new();
	}
	return ready_q;
}

int priority_queue_destructor(FIFO_q_p* q) 
{
	if (q == NULL) return -1;
	int i;
	for (i = 0; i < QUEUE_SIZE; i++) {
		fifo_q_destroy(q[i]);
	}
	free(q);
	return 0;
}

int enqueue_ready(FIFO_q_p* theready_q, unsigned char priority, PCB_p the_pcb)
{
	if (theready_q == NULL) 
		return -1;
	FIFO_q_p q = theready_q[priority];
	fifo_q_enqueue(q, the_pcb);
	return 0;
}

PCB_p dequeue_ready(FIFO_q_p* theready_q, unsigned char priority)
{
  if (theready_q == NULL) return NULL;
	FIFO_q_p q = theready_q[priority];
	if(fifo_q_is_empty(q)==1)
	{
		printf("This queue is empty!\n");
		return NULL;
	}
	return fifo_q_dequeue(q);
}

int empty_ready(FIFO_q_p* theready_q, unsigned char priority)
{
  if (theready_q == NULL) return -1;
  return fifo_q_is_empty(theready_q[priority]);
}

int all_empty_ready(FIFO_q_p* theready_q)
{
	return -1;
}
