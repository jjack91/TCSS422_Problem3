//Jacob Ackerman and Josh Lau

#include "pcb.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUM_PRIORITY_LEVEL 16

static unsigned int next_pid = 0;

PCB_p pcb_new(void) {
	PCB_p this = (PCB_p) malloc(sizeof(PCB_s));
	// initialize new PCB
	pcb_init(this);
	//this->context = (CPU_context_p) malloc(sizeof(CPU_context_s));
	return this;
}

void pcb_init(PCB_p this) {
	this->context = (CPU_context_p) malloc(sizeof(CPU_context_s));
	this->context->pc = 0;
	this->context->ir = 0;
	this->context->psr = 0;
	this->context->r0 = 0;
	this->context->r1 = 0;
	this->context->r1 = 0;
	this->context->r1 = 0;
	this->context->r1 = 0;
	this->context->r1 = 0;
	this->context->r6 = 0;
	this->context->r7 = 0;

	pcb_assign_pid(this);
	this->state = NEW;
	this->parent = 0;
	this->priority = 0;
	this->mem = 0;
	this->size = 0;
	this->channel_no = 0;
}

void pcb_destroy(PCB_p this) {
	free(this->context);
	free(this);
}

void pcb_assign_pid(PCB_p this) {
	this->pid = next_pid++;
}

void pcb_set_state(PCB_p this, enum state_type state) {
	this->state = state;
}

void pcb_set_parent(PCB_p this, unsigned int parent_pid) {
	this->parent = parent_pid;
}

void pcb_set_priority(PCB_p this, unsigned char priority) {
	this->priority = (unsigned char) (priority % NUM_PRIORITY_LEVEL);
}

char * pcb_to_string(PCB_p this, char * s) {
	char buffer[256];
	//sprintf prints formatted data into a buffer/char array (does not allocate space, can overwrite data)
	sprintf(buffer, "PID: 0x%X, Priority: 0x%X, state: %d, PC: 0x%04X, Mem: 0x%04X, Size: 0x%X, Channel No.: 0x%X",
			this->pid, this->priority, this->state, this->context->pc, 42, this->size, this->channel_no);
	strcpy(s, buffer);
	return s;
}
