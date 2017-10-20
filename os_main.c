// Jacob Ackerman and Jenna Hand

#include "pcb.h"
#include "fifoqueue.h"
#include "priority_queue.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

int sys_stack = 0;
int output_tick = 0;
//int g_pid = 0;

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
	FIFO_q_p new_jobs = fifo_q_new();
	FIFO_q_p zombie_queue = fifo_q_new();
	//the ready queue
	FIFO_q_p* the_mlfq = priority_queue();
	
	// Pointer for currently running process
	PCB_p current = NULL;
	
	// PC register local to main loop
	unsigned int pc = 0;

	
	// Main loop in which each cycle/iteration represents a timer quantum
	for (;;) {
		
		// Function to create a random number of processes (0 - 5)
		// Processes are then put in the ready queue
		create_processes(the_mlfq);
		
		// Loop will simulate running of current process
		// Add a PC value (unsigned int) to current PCB (random between 3000 and 4000)
		pc += rand() % (1001) + 3000;
		
		if(current == NULL)
		{
			printf("Assigning first job\n");
			current = fifo_q_dequeue(new_jobs);
			current->context->pc = pc;
			printf("Job assigned\ncurrent = %d\n", current);
		}
		
		
		
		
		// Pseudo-push of PC to system stack
		sys_stack = pc;
		
		int is_terminate = rand()%100;
		if(is_terminate <= 15 /* && !privileged_few*/)
		{
			enum state_type status = HALTED;
			pcb_set_state(current, status);
		}
		
		// Simulate timer interrupt / call a pseudo-ISR
		/*	Call this when job uses up alloted quantum for its priority level	*/
		isr(jobs, current, &pc); // TODO: change to accept MLFQ
		printf("-------------------\n"); //For loop tracking
	}
	
	
	
}

void create_processes(FIFO_q_p* jobs) {
	srand(time(NULL));
	int n = rand()%6; // get random number of jobs, 0-5
	printf("Creating jobs...\n");
	/* Start creating and adding the jobs to the queue */
	int i;
	PCB_p newjob;
	enum state_type status = NEW;
	for(i=0; i<n; i++)
	{
		newjob = pcb_new();
		pcb_set_state(newjob, status);
		pcb_assign_pid(newjob);
		enqueue_ready(jobs, newjob->priority, newjob);
	}
	printf("Jobs created\n");
}

void isr(FIFO_q_p jobs, PCB_p current, unsigned int * pc, FIFO_q_p* mlfq) {
	// ISR will change the state of the running process to interrupted,
	// save the CPU state (PC) to the PCB for that process, and do an up-call (scheduler)
	if(current != NULL)
	{
		current->state = INTERRUPTED;
		current->context->pc = *pc;
	}
	scheduler(TIMER, jobs, current, mlfq);
	// Performs pseudo-IRET/returns to main loop
}

/*  TODO: Alter scheduler to accept a priority queue object and implement rules
	for checking priority acording to MLFQ rules */
void scheduler(enum interrupt_type interrupt, FIFO_q_p special_q, PCB_p current, FIFO_q_p* mlfq) {
	// Determines that the scheduler only needs to put the process back
	// in the ready queue and change its state from interrupted to ready.
	 
	if (current->state == HALTED)
	{
		fifo_q_enqueue(special_q, current); 
		current = NULL;
	}
	
	if (interrupt == TIMER)
	{
	//	dispatcher(jobs, current);
	
	// Each time a process is scheduled, print a message with its contents
		if(current == NULL)
		{
			printf("Assigning new job!\n");
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
		/*if(output_tick==4)
		{
			char sys_message[256] = "";
		
			printf("===============\n");
			printf("Currently running: %s\n", pcb_to_string(current, sys_message));
			printf("Switching to: %s\n", pcb_to_string(jobs->front->process, sys_message));
		}*/
		
		dispatcher(jobs, current);
		
		/*if(output_tick==4)
		{
			char sys_message[256] = "";
		
			printf("Now running: %s\n", pcb_to_string(current, sys_message));
			printf("Switched from: %s\n", pcb_to_string(jobs->back->process, sys_message));
			printf("Contents of queue: %s\n", fifo_q_to_string(jobs, sys_message));
			printf("===============\n");
			output_tick = 0;
		}*/
		output_tick++;
	// After context switch, print the same two PCBs to show different contents
	// Also print ready queue
	// Additional housekeeping
	// Returns to isr
	}
}

void dispatcher(FIFO_q_p* mlfq, PCB_p current) {
	// Will dequeue next waiting process, change to RUNNING
	// Will copy new pcb's pc value to system stack
	// Returns to scheduler
	if(current != NULL)
	{
		//printf("current is non-null\n");
		//increment priority unless at max.
		current->state = READY;
		if(current->priority < 15) {
			current->priority += 1;
		}
		enqueue_ready(jobs, current);
	}
		//current = fifo_q_dequeue(jobs);
		current = nextjob(mlfq);
		current->state = RUNNING;
		//printf("Switched state to running\n");
		/* Pull data from PCB and hand off to system */
		sys_stack = current->context->pc;
		/* return to scheduler */ 
}

PCB_p nextjob(FIFO_q_p* mlfq) {
	PCB_p nj;
	int i;
	for(i = 0; i < 15; i++) {
		nj = dequeue_ready(mlfq, i);
		if(nj!= NULL)
			break;
	}
}