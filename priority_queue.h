//Jacob Ackerman and Josh Lau

#include "pcb.h"
#include "fifoqueue.h"

FIFO_q_p* priority_queue();
int enqueue_ready(FIFO_q_p* theready_q, unsigned char priority, PCB_p the_pcb);
PCB_p dequeue_ready(FIFO_q_p* theready_q, unsigned char priority);
int empty_ready(FIFO_q_p* theready_q, unsigned char priority);
int all_empty_ready(FIFO_q_p* theready_q);
int priority_queue_destructor(FIFO_q_p* q);
