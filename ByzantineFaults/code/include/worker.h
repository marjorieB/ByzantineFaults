#ifndef _WORKER_H
#define _WORKER_H

#include "task.h"

struct worker {
	char primary[MAILBOX_SIZE];
	unsigned char reputation;
};

void ask_to_join(char * mailbox, char * myMailbox);

void receive_ack(struct worker * worker, char * myMailbox);

void treat_task(struct worker * me, msg_task_t task, struct task * data_task, char * myMailbox);

int worker (int argc, char * argv[]);

#endif
