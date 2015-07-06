#ifndef _WORKER_H
#define _WORKER_H

#include "task.h"
#include "msg/msg.h"


#define REQUEST_SIZE 256
#define PEAK_FLOP_S 2000000000


// structure of information about tasks processing that a worker keep in memory (and transmit to the primary as answer)
struct w_task {
	char client[MAILBOX_SIZE];
	char worker_name[MAILBOX_SIZE];
	char task_name[TASK_NAME_SIZE];
	unsigned int long answer; 
};

struct worker {
	char primary[MAILBOX_SIZE];
	unsigned long int id;
	char reputation;
	unsigned long int index; // correspond to the index in which the worker has to take the information about it
};

struct present_or_failed {
	double time;
	unsigned char type;
};

xbt_dynar_t * presence;


void ask_to_join(char * mailbox, char * myMailbox);

void receive_ack(struct worker * worker, char * myMailbox);

//this function returns the time the process has to wait before entering in the system
double enter_the_system (struct worker * me);

// this function returns 0 if the node is present in the system, otherwise this function returns the time where the node have to recover (ask the primary to enter in the system)
double present(struct worker * me, double duration_task);

double treat_task_worker(struct worker * me, msg_task_t task, char * myMailbox);

int worker (int argc, char * argv[]);

#endif
