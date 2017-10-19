fifo_q** priority_queue();
int enqueue_ready(fifo_q** theready_q, unsigned char priority, PCB_p the_pcb);
PCB_p dequeue_ready(fifo_q** theready_q, unsigned char priority);
int empty_ready(fifo_q** theready_q, unsigned char priority);
int all_empty_ready(fifo_q** theready_q);
int priority_queue_destructor(fifo_q** q);
