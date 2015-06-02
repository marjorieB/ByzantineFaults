#ifndef _PRIMARY_H
#define _PRIMARY_H

#include "task.h"
#include "worker.h"


// structure of information about tasks (already distributed on workers) processing that a primary keep in memory
struct p_task {
	char client[MAILBOX_SIZE]; 
	char task_name[TASK_NAME_SIZE];
	xbt_dynar_t w_answers; // list of struct p_answer_worker
	int duration;
	int size;
	int final_answer;
	int nb_forwarded;
	int nb_answers_received;
	int nb_false_answers;
	int nb_correct_answers;
};

struct p_worker {
	char mailbox[MAILBOX_SIZE];
	unsigned char reputation; // correspond to the reputation of the worker seen by the primary
	unsigned int totR; // correspond to the number of requests submitted to the worker
	unsigned int totC; // correspond to the number of assumed correct answers that the worker give
};

struct p_answer_worker {
	char worker_name[MAILBOX_SIZE];
	char bool_answer;
};

/*struct group {
	xbt_dynar_t workers;
};*/

struct pimary {
	xbt_dynar_t active_workers;
	xbt_dynar_t passive_workers;
};

xbt_dynar_t workers;

// list of tasks the primary hasn't distributed yet 
xbt_fifo_t tasks;

xbt_fifo_t processing_tasks;

xbt_fifo_t active_groups;

xbt_fifo_t inactive_groups;


void tasks_print(void);

void processing_tasks_print(void);

void workers_print(void);

void groups_print(xbt_fifo_t * f); 

struct p_worker * dynar_search(const char * name);

void send_finalize_to_workers(void);

xbt_fifo_item_t fifo_supress_item_head(xbt_fifo_t l);

void * fifo_supress_head(xbt_fifo_t l);

struct p_worker * give_worker_dynar(char * name);

struct p_worker * give_worker_active_groups(char * name);

struct p_worker * give_worker_inactive_groups(char * name);

void updateReputation(struct p_task * t);

void add_new_worker(const char * name, char * myMailbox);

void put_task_fifo(msg_task_t task);

void treat_tasks(void);

void try_to_treat_tasks(void);

void treat_answer(msg_task_t);

int primary (int argc, char * argv[]);

#endif
