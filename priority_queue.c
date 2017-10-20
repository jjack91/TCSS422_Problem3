#include <stdio.h>
#include <stdlib.h>
#include "pcb.h"
#include "fifoqueue.h"
#include "priority_queue.h"

#define QUEUE_SIZE 16

//This is used for the number of priority levels
fifo_q** priority_queue()
{
	fifo_q **ready_q = malloc(sizeof(fifo_q *) * QUEUE_SIZE);
	int i = 0;
	for(i; i < QUEUE_SIZE; i++)
	{
		ready_q[i] = fifo_queue();
	}
	return ready_q;
}

int priority_queue_destructor(fifo_q** q) 
{
	if (q == NULL) return -1;
	int i;
	for (i = 0; i < QUEUE_SIZE; i++) {
		fifo_destructor(q[i]);
	}
	free(q);
	return 0;
}

int enqueue_ready(fifo_q** theready_q, unsigned char priority, PCB_p the_pcb)
{
  if (theready_q == NULL) return -1;
	fifo_q* q = theready_q[priority];
	q_enqueue(q, the_pcb);
  return 0;
}

PCB_p dequeue_ready(fifo_q** theready_q, unsigned char priority)
{
  if (theready_q == NULL) return -1;
	fifo_q* q = theready_q[priority];
	if(q_is_empty(q)==0)
	{
		printf("This queue is empty!\n");
		return NULL;
	}
	return q_dequeue(q);
}

int empty_ready(fifo_q** theready_q, unsigned char priority)
{
  if (theready_q == NULL) return -1;
  return q_is_empty(theready_q[priority]);
}

int all_empty_ready(fifo_q** theready_q)
{
	return -1;
}
