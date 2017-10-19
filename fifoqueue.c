// Jacob Ackerman and Jenna Hand

#include "fifoqueue.h"
#include "pcb.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Creates a new fifo queue
FIFO_q_p fifo_q_new(void) {
	//Instantiate on heap
    FIFO_q_p this = (FIFO_q_p) malloc(sizeof(FIFO_q_s));
	// Check that malloc was successful
    if (!this)
        return NULL;
	// Initialize members
    this->length = 0;
    this->elementSize = sizeof(Node_p);
    this->front = NULL;
    this->back = NULL;
	
    return this;
}

// Frees a fifo queue
int fifo_q_destroy(FIFO_q_p queue) {
	// If the parameter is null, nothing to destroy
    if (!queue)
        return 0;
	// Iterate through queue
	PCB_p dequeued;
    while (!fifo_q_is_empty(queue)) {
		// Dequeue process / current node
        dequeued = fifo_q_dequeue(queue);
		// Free PCB
        pcb_destroy(dequeued);
    }
	// Free queue
    free(queue);
    return 1;
}

// Creates a string representation of the fifo queue
char * fifo_q_to_string(FIFO_q_p queue, char * string) {
	// If either parameter is null, return
	if (!queue || !string)
		return NULL;
	char tempstring[256] = "";
	char buffer[256];
	sprintf(buffer, "Q:Count=%d: P", queue->length);
	strcat(tempstring, buffer);
	Node_p tempnode = queue->front;
	int i;
	for (i = 0; i < queue->length; i++) {
        char buffer2[256];
		sprintf(buffer2, "%d->", tempnode->process->pid);
		strcat(tempstring, buffer2);
        tempnode = tempnode->next;
    }
    strcat(tempstring, "*");
    strcpy(string, tempstring);
    return string;
}

int fifo_q_is_empty(FIFO_q_p queue) {
    if (!queue)
        return 1;
    if (!queue->front)
        return 1;
    if (queue->length == 0)
        return 1;
    return 0;
}

int fifo_q_enqueue(FIFO_q_p queue, PCB_p process) {
    if (!queue || !process)
        return 0;
    Node_p node = calloc(1, sizeof(Node_s));
    if (!node)
        return 0;
    node->process = process;
    node->next = NULL;
    if (queue->back)
        queue->back->next = node;
    queue->back = node;
    if (!queue->front)
        queue->front = node;
    queue->length++;
    return 1;
}

PCB_p fifo_q_dequeue(FIFO_q_p queue) {
    if (fifo_q_is_empty(queue))
        return NULL;
    Node_p front = queue->front;
    Node_p next = front->next;
    PCB_p dequeued_process = queue->front->process;
    if (queue->back == queue->front) {
        queue->back = NULL;
    }
    free(queue->front);
    queue->front = next;
    queue->length--;
    return dequeued_process;
}
