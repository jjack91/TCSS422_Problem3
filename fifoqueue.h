// Jacob Ackerman and Jenna Hand

#ifndef FIFOQUEUE_H_INCLUDED
#define FIFOQUEUE_H_INCLUDED

#include "pcb.h"

typedef struct node {
	PCB_p process;
	struct node * next;
} Node_s;

typedef Node_s * Node_p;

typedef struct fifo_q {
    unsigned int length;
    int elementSize;
    Node_p front;
    Node_p back;
} FIFO_q_s;

typedef FIFO_q_s * FIFO_q_p;

FIFO_q_p fifo_q_new(void);

int fifo_q_destroy(FIFO_q_p);

char * fifo_q_to_string(FIFO_q_p, char *);

int fifo_q_is_empty(FIFO_q_p);

int fifo_q_enqueue(FIFO_q_p, PCB_p);

PCB_p fifo_q_dequeue(FIFO_q_p);

#endif
