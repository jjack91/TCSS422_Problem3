//Jacob Ackerman and Josh Lau


#include "pcb.h"
#include "fifoqueue.h"
#include "priority_queue.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_LOOPS 64
#define MAX_PROCESSES 32

int sys_stack = 0;
int output_tick = 0;
PCB_p idle;
FIFO_q_p special_pcbs;
/*
 * This is used to set the current quantum length.
 * Quantum lengths are the current priority level,
 * and all previous priority levels added together.
 */
int curr_quantum = 1;
int process_count = 0;

//int g_pid = 0;

enum interrupt_type {TIMER, IO, EXCEPTION}; // IO and Exception states won't be used (yet),
											// but includded for completeness
void create_processes(FIFO_q_p, FIFO_q_p, int[]);
PCB_p nextjob(FIFO_q_p*);
void choose_special_ids(int[]);
PCB_p isr(FIFO_q_p, PCB_p, unsigned int * pc, FIFO_q_p*, FIFO_q_p);
PCB_p scheduler(enum interrupt_type, FIFO_q_p, PCB_p current, FIFO_q_p*, FIFO_q_p);
PCB_p dispatcher(FIFO_q_p*, PCB_p);

/* 	OS Main simulates timer interrupts and scheduling of processes 
	in a round robin algorithm */
int main(void) {
	int special[4];
	choose_special_ids(special);
	// List for processes
	printf("Begin program\n");
	FIFO_q_p new_jobs = fifo_q_new();
	special_pcbs = fifo_q_new();
	FIFO_q_p zombie_queue = fifo_q_new();
	//the ready queue
	FIFO_q_p* the_mlfq = priority_queue();
	
	idle = pcb_new();
	idle->pid = -1;
	
	// Pointer for currently running process
	PCB_p current = NULL;
	
	// PC register local to main loop
	unsigned int pc = 0;

	srand(time(NULL));
	// Main loop in which each cycle/iteration represents a timer quantum
	for (;;) {
		
		// Function to create a random number of processes (0 - 5)
		// Processes are then put in the ready queue
		if(/*process_count < MAX_PROCESSES*/ rand()%100 >= 60 || output_tick == 0)
		{
			create_processes(new_jobs, special_pcbs, special);
		}
		
		// Loop will simulate running of current process
		// Add a PC value (unsigned int) to current PCB (random between 3000 and 4000)
		//pc += rand() % (1001) + 3000;
		
		if(current == NULL && output_tick == 0)
		{
			char *tostring;
			char output[256] = "";
		//	printf("new_jobs contents: %s\n", fifo_q_to_string(new_jobs, output));
			
			printf("Assigning first job\n");
			current = fifo_q_dequeue(new_jobs);
			if(current == NULL)
			{
				current = idle;
			}
			else
			{
				current->context->pc = pc;
			}
		//	printf("addr of first job: %p\n", current);

			//printf("%s", pcb_to_string(current, tostring));
		}
		
		int i;
		for (i = 0; i < curr_quantum; i++)
		{
			if (current->pid != -1 || process_count == 0)
			{
			pc += rand()%(1001) + 3000;
			// Pseudo-push of PC to system stack
			sys_stack = pc;
			//printf("Job assigned\ncurrent = ");
			int is_terminate = rand()%100;
			if(is_terminate <= 15 && (current->pid != special[0]
				|| current->pid != special[1]
				|| current->pid != special[2]
				|| current->pid != special[3]))
			{
				enum state_type status = HALTED;
				pcb_set_state(current, status);
				printf("Job terminated.\n");
				process_count--;
			}
			}
			//printf("Job assigned\ncurrent = ");
		
		}
		
		// Simulate timer interrupt / call a pseudo-ISR
		/*	Call this when job uses up alloted quantum for its priority level	*/
		//printf("Job assigned\ncurrent = ");
	//	printf("addr of current, main loop: %p\n", current);
		current = isr(zombie_queue, current, &pc, the_mlfq, new_jobs); // TODO: change to accept MLFQ
		printf("-------------------\n"); //For loop tracking
		if (output_tick >= MAX_LOOPS)
		{
			return 0;
		}
	}
	
	
	
}
void choose_special_ids(int arr[]) {
	int i;
	srand(time(NULL));
	for(i = 0; i <4; i++) {
		arr[i] = rand()%100;
		//arr[i] = i;
	}
}

void create_processes(FIFO_q_p new_jobs_q, FIFO_q_p privileged, int special[]) {
	
	int n = rand()%6; // get random number of jobs, 0-5
	//int p = rand()%15;
	printf("Creating jobs...\n");
	/* Start creating and adding the jobs to the queue */
	int i,j;
	PCB_p newjob;
	enum state_type status = NEW;
	for(i=0; i<n; i++)
	{
		newjob = pcb_new();
		pcb_set_state(newjob, status);
		//pcb_assign_pid(newjob);
		printf("New job addr: %p\n", newjob);
		//pcb_set_priority(newjob, p);

		for(j = 0; j<3; j++) {
			int val = special[j];
			if (val == newjob->pid) {
				fifo_q_enqueue(special_pcbs, newjob);
				break;

			}

		}
		fifo_q_enqueue(new_jobs_q, newjob);
		process_count++;
	}
	printf("Jobs created\n");
}

PCB_p isr(FIFO_q_p terminate_q, PCB_p current, unsigned int * pc, FIFO_q_p* mlfq, FIFO_q_p new_jobs_q) {
	// ISR will change the state of the running process to interrupted,
	// save the CPU state (PC) to the PCB for that process, and do an up-call (scheduler)

	if(current != NULL && current->state != HALTED)
	{
		current->state = INTERRUPTED;
		current->context->pc = *pc;
	}
	//printf("addr of current, isr: %p, pid=%d\n", current, current->pid);
	current = scheduler(TIMER, terminate_q, current, mlfq, new_jobs_q);
	
	return current;
	// Performs pseudo-IRET/returns to main loop
}

/*  TODO: Alter scheduler to accept a priority queue object and implement rules
	for checking priority acording to MLFQ rules */
PCB_p scheduler(enum interrupt_type interrupt, FIFO_q_p special_q, PCB_p current, FIFO_q_p* mlfq, FIFO_q_p new_jobs_q) {
	// Determines that the scheduler only needs to put the process back
	// in the ready queue and change its state from interrupted to ready.
	/*	Check for and schedule new jobs	*/
	
	if (new_jobs_q->length > 0)
	{
		PCB_p tmp = NULL;
		while (new_jobs_q->length > 0)
		{
			tmp = fifo_q_dequeue(new_jobs_q);
			//printf("New job addr: %p\n", tmp);
			enqueue_ready(mlfq, tmp->priority, tmp);
		}
	}
	
	printf("\n\nIteration: %d\n", output_tick+1);
	int c;
	for (c = 0; c < 16; c++)
	{
		printf("Q%d: %d\n", c, mlfq[c]->length);
		
	}
	PCB_p spc_tmp = NULL;
	int length = special_pcbs->length;
	for(c = 0; c < length; c++)
	{
		spc_tmp = fifo_q_dequeue(special_pcbs);
		if(spc_tmp != NULL)
		{
			printf("PCB: PID %d, PRIORITY %d, PC %d\n", spc_tmp->pid, spc_tmp->priority, spc_tmp->context->pc);
		}
		fifo_q_enqueue(special_pcbs, spc_tmp);
	}
	

	if (current->state == HALTED)
	{
		fifo_q_enqueue(special_q, current); 
		current = NULL;
	}
	
	if (interrupt == TIMER)
	{
	//	dispatcher(jobs, current);
	
	// Calls dispatcher
	//	printf("addr of current, scheduler: %p, pid=%d\n", current, current->pid);
		current = dispatcher(mlfq, current);

		output_tick++;

	// Additional housekeeping
	if(current->pid == -1)
	{
		curr_quantum=2;
	}
	else
	{
		int i;
		curr_quantum=0;
		for (i = 0; i < current->priority; i++)
		{
			curr_quantum+=current->priority;
		}
	}
	}
	
	/*	Kill off zombie jobs if more than 10 exist	*/
	if (special_q->length >= 10)
	{
		PCB_p tmp = NULL;
		while (special_q->length > 0)
		{
			tmp = fifo_q_dequeue(special_q);
			pcb_destroy(tmp);
		}
	}
	return current;
	// Returns to isr
}

PCB_p dispatcher(FIFO_q_p* mlfq, PCB_p current) {
	// Will dequeue next waiting process, change to RUNNING
	// Will copy new pcb's pc value to system stack
	// Returns to scheduler

	
	
	if(current != NULL)
	{
		//printf("current is non-null\n");
		//increment priority unless at max.
		//printf("addr of current, dispatcher: %p, pid=%d\n", current, current->pid);
		current->state = READY;
		//printf("Current priority: %d, priority addr: %p\nCurrently assigned job: %p\n"
		//	, current->priority, &current->priority, current);
		if(current->priority < 15) {
			current->priority += 1;
		}
		if(current->priority == 15)
		{
			current->priority=0;
		}
		//printf("Job assigned\nairplane = ");
		enqueue_ready(mlfq, current->priority, current);
		//printf("Job assigned\nfairplane = ");
	}
	//intf("Job assigned\nairplane = ");
		//current = fifo_q_dequeue(jobs);
		//printf("Job assigned\nairplane = ");
		current = nextjob(mlfq);
		if(current != NULL) {

			current->state = RUNNING;
			//printf("Switched state to running\n");
			/* Pull data from PCB and hand off to system */
			sys_stack = current->context->pc;
			//printf("Job assigned\nairplane = %p\n", (void*)current);
		}
		else
		{
			current = idle;
		}
		
		return current;
		/* return to scheduler */ 
}

PCB_p nextjob(FIFO_q_p* mlfq) {
	PCB_p nj = NULL;
	int i;
	for(i = 0; i < 15; i++) {
		nj = dequeue_ready(mlfq, i);
		
		if(nj!= NULL) break;
	}
		//printf("addr of current, newjob selection: %p, pid=%d\n", nj, nj->pid);
		return nj;
	
}
