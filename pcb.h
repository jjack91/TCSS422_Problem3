//Jacob Ackerman and Josh Lau

#ifndef PCB_H_INCLUDED
#define PCB_H_INCLUDED

typedef struct cpu_context {
	// CPU state for the LC-3 processor
	unsigned int pc;
	unsigned int ir;
	unsigned int psr;
	unsigned int r0;
	unsigned int r1;
	unsigned int r2;
	unsigned int r3;
	unsigned int r4;
	unsigned int r5;
	unsigned int r6;
	unsigned int r7;
} CPU_context_s;	// _s means this is a structure definition

typedef CPU_context_s * CPU_context_p; // _p means that this is a pointer to structure

enum state_type {NEW, READY, RUNNING, INTERRUPTED, WAITING, HALTED};

typedef struct pcb {
	unsigned int pid;	//process identification
	enum state_type state;	//process state (running waiting, etc.)
	unsigned int parent;	// parent process pid
	unsigned char priority;	// 0 is highest - 15 is lowest
	unsigned char * mem;	// start of process in memory
	unsigned int size;	// number of bytes in process
	unsigned char channel_no;	// which I/O device or service Q
	// if process is blocked, which queue it is in
	CPU_context_p context;	// set of cpu registers
	//other items to be added as needed
} PCB_s;

typedef PCB_s * PCB_p;


CPU_context_p context_new(void);

PCB_p pcb_new(void);

void pcb_init(PCB_p);

void pcb_destroy(PCB_p);

unsigned int pcb_get_pid(PCB_p);

void pcb_assign_pid(PCB_p);

enum state_type pcb_get_state(PCB_p);

void pcb_set_state(PCB_p, enum state_type);

unsigned int pcb_get_parent(PCB_p);

void pcb_set_parent(PCB_p, unsigned int);

unsigned char pcb_get_priority(PCB_p);

void pcb_set_priority(PCB_p, unsigned char);

char * pcb_to_string(PCB_p, char *);

unsigned int pcb_get_pc(PCB_p);

#endif
