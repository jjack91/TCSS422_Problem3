// Jacob Ackerman and Jenna Hand

#include "pcb.h"
#include "fifoqueue.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

int sys_stack = 0;
int output_tick = 0;

enum interrupt_type {TIMER, IO, EXCEPTION}; // IO and Exception states won't be used (yet),
											// but includded for completeness

void isr(FIFO_q_p jobs, PCB_p current, unsigned int * pc);
void scheduler(enum interrupt_type interrupt, FIFO_q_p jobs, PCB_p current);
void dispatcher(FIFO_q_p jobs, PCB_p current);

/* 	OS Main simulates timer interrupts and scheduling of processes 
	in a round robin algorithm */
int main(void) {
	
	// List for processes
	printf("Begin program\n");
	FIFO_q_p jobs = fifo_q_new();
	
	// Pointer for currently running process
	PCB_p current = NULL;
	
	// PC register local to main loop
	unsigned int pc = 0;

	
	// Main loop in which each cycle/iteration represents a timer quantum
	while (jobs->length < 30) {
		
		// Function to create a random number of processes (0 - 5)
		// Processes are then put in the ready queue
		create_processes(jobs);
		
		// Loop will simulate running of current process
		// Add a PC value (unsigned int) to current PCB (random between 3000 and 4000)
		pc += rand() % (4000 + 1 - 3000) + 3000;
		
		if(current == NULL)
		{
			printf("Assigning first job\n");
			current = fifo_q_dequeue(jobs);
			current->context->pc = pc;
			printf("Job assigned\ncurrent = %d\n", current);
		}
		
		
		// Pseudo-push of PC to system stack
		sys_stack = pc;
		
		// Simulate timer interrupt / call a pseudo-ISR
		isr(jobs, current, &pc);
		printf("-------------------\n"); //For loop tracking
	}
	
	
	
}

void create_processes(FIFO_q_p jobs) {
	srand(time(NULL));
	int n = rand()%5; // get random number of jobs, 0-5
	printf("Creating jobs...\n");
	/* Start creating and adding the jobs to the queue */
	int i;
	PCB_p newjob;
	enum interrupt_type status = READY;
	for(i=0; i<n; i++)
	{
		newjob = pcb_new();
		pcb_set_state(newjob, status);
		fifo_q_enqueue(jobs, newjob);
	}
	printf("Jobs created\n");
}

void isr(FIFO_q_p jobs, PCB_p current, unsigned int * pc) {
	// ISR will change the state of the running process to interrupted,
	// save the CPU state (PC) to the PCB for that process, and do an up-call (scheduler)
	if(current != NULL)
	{
		current->state = INTERRUPTED;
		current->context->pc = *pc;
	}
	scheduler(TIMER, jobs, current);
	// Performs pseudo-IRET/returns to main loop
}

/*  TODO: Decide what parameter should be passed to scheduler to denote
	a tiimer interrupt. */
void scheduler(enum interrupt_type interrupt, FIFO_q_p jobs, PCB_p current) {
	// Determines that the scheduler only needs to put the process back
	// in the ready queue and change its state from interrupted to ready.
	if (interrupt == TIMER)
	{
	//	dispatcher(jobs, current);
	
	// Each time a process is scheduled, print a message with its contents
		if(current == NULL)
		{
			printf("Assigning first job!\n");
		}
		else
		{
			printf("Outputting...\n");
			char sys_message[256] = "";
			//sys_message = pcb_to_string(current, sys_message);
			printf("Scheduling process: %s\n", pcb_to_string(current, sys_message));
		}
	// Calls dispatcher
	// At every fourth context switch/call to dispatcher, print contents of running PCB
	// and "Switching to: " contents of ready queue head
		if(output_tick==4)
		{
			char sys_message[256] = "";
		
			printf("===============\n");
			printf("Currently running: %s\n", pcb_to_string(current, sys_message));
			printf("Switching to: %s\n", pcb_to_string(jobs->front->process, sys_message));
		}
		
		dispatcher(jobs, current);
		
		if(output_tick==4)
		{
			char sys_message[256] = "";
		
			printf("Now running: %s\n", pcb_to_string(current, sys_message));
			printf("Switched from: %s\n", pcb_to_string(jobs->back->process, sys_message));
			printf("Contents of queue: %s\n", fifo_q_to_string(jobs, sys_message));
			printf("===============\n");
			output_tick = 0;
		}
		output_tick++;
	// After context switch, print the same two PCBs to show different contents
	// Also print ready queue
	// Additional housekeeping
	// Returns to isr
	}
}

void dispatcher(FIFO_q_p jobs, PCB_p current) {
	// Will dequeue next waiting process, change to RUNNING
	// Will copy new pcb's pc value to system stack
	// Returns to scheduler
	if(current != NULL)
	{
		//printf("current is non-null\n");
		current->state = READY;
		fifo_q_enqueue(jobs, current);
	}
		current = fifo_q_dequeue(jobs);
		current->state = RUNNING;
		//printf("Switched state to running\n");
		/* Pull data from PCB and hand off to system */
		sys_stack = current->context->pc;
		/* return to scheduler */ 
}
