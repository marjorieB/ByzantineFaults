#ifndef _WORKER_H
#define _WORKER_H

#include "task.h"

// structure of information about tasks processing that a worker keep in memory (and transmit to the primary as answer)
struct w_task {
	char client[MAILBOX_SIZE];
	char worker_name[MAILBOX_SIZE];
	char task_name[TASK_NAME_SIZE];
	char bool_answer; // boolean indicated if the answer given by the worker is correct or not
};

struct worker {
	char primary[MAILBOX_SIZE];
	unsigned char reputation;
};

void ask_to_join(char * mailbox, char * myMailbox);

void receive_ack(struct worker * worker, char * myMailbox);

void treat_task_worker(struct worker * me, msg_task_t task, char * myMailbox);

int worker (int argc, char * argv[]);

#endif
