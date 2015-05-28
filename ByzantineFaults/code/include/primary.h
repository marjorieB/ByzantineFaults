#ifndef _PRIMARY_H
#define _PRIMARY_H

#include "task.h"

#define MIN_WORKERS_GROUP 5

struct p_worker {
	char mailbox[MAILBOX_SIZE];
	unsigned char answer; // the answer is symbolised by a boolean. If the answer is correct the boolean is true, otherwise it is false
	unsigned char reputation; // correspond to the reputation of the worker seen by the primary
	unsigned int totR; // correspond to the number of requests submitted to the worker
	unsigned int totC; // correspond to the number of assumed correct answers that the worker give
};

struct group {
	xbt_dynar_t workers;
};

struct pimary {
	xbt_dynar_t active_workers;
	xbt_dynar_t passive_workers;
};

xbt_dynar_t workers;

xbt_fifo_t tasks;



void dynar_print(void);

int dynar_search(const char * name) ;

void send_finalize_to_workers(void);

void add_new_worker(const char * name, char * myMailbox);

void put_task_fifo(msg_task_t task);

void treat_answer(void);

int primary (int argc, char * argv[]);

#endif
