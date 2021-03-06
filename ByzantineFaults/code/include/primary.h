#ifndef _PRIMARY_H
#define _PRIMARY_H

#include "task.h"
#include "worker.h"
#include "simulator.h"


#define NB_MAX_ACTIVE_PRIMARIES 500


struct reputations_primary {
	char max_reputation;
	char min_reputation;
	char value;
};

struct reputations_loadBalancing {
	char max_reputation;
	char min_reputation;
	char mailbox[MAILBOX_SIZE];
};

struct loadBalancing {
	xbt_dynar_t workersToSend;
	char min_reputation;
	char value;
};

struct fusion {
	xbt_dynar_t workersToSend;
	xbt_fifo_t tasks;
	xbt_fifo_t processing_tasks;
	char max;
	char min;
};


// structure of information about tasks (already distributed on workers) processing that a primary keep in memory
struct p_task {
	double start;
	char client[MAILBOX_SIZE]; 
	char task_name[TASK_NAME_SIZE];
	xbt_dynar_t w_answers; // array of struct p_answer_worker
	int duration; // time in Flop/s taken to compute the task
	int size; // size of the answer (in bytes)
	long int final_answer;
	int nb_forwarded; // number of workers on which we send the task
	int nb_crashed; // number of workers that don't answer
	int nb_answers_received;
	int nb_false_answers;
	int nb_correct_answers;
	double targetLOC;
	int to_replicate; // when the additional replication strategy corresponds to a fixed_fit strategy, we need say how many replicas we need, this variable is used when the task is in the additional_replication_tasks
	int nb_replication; // number of times the task had been replicated additionally
	xbt_dynar_t additional_workers; // contain the names of the workers added to compute the task
	xbt_dynar_t additional_reputations; // contain the reputation of the workers added to compute the task
	//xbt_dynar_t * active_workers; // pointer towards the group of active_workers containing the workers executing the task
	xbt_fifo_item_t active_workers;
	struct p_answer_worker * res; // current majoritary result
};

struct p_worker {
	char mailbox[MAILBOX_SIZE];
	unsigned char reputation; // correspond to the reputation of the worker seen by the primary
	unsigned int totR; // correspond to the number of requests submitted to the worker
	unsigned int totC; // correspond to the number of assumed correct answers that the worker give
};

struct p_answer_worker {
	xbt_dynar_t worker_names;
	xbt_dynar_t worker_reputations;
	unsigned int long answer;
};

char first_primary_name[MAILBOX_SIZE];

struct reputations_primary * reputations_primary;

xbt_dynar_t * workers;

// list of tasks the primary hasn't distributed yet. Each case of the table corresponds to a fifo of one primary 
xbt_fifo_t * tasks;

xbt_fifo_t * processing_tasks;

xbt_fifo_t * active_groups;

xbt_fifo_t * additional_replication_tasks; // list of structure struct p_task. A processing task will be put in that list when we need to replicate it but we haven't been able to replicate entirely the task because there aren't enough workers able to execute this task

xbt_dynar_t * to_change_primary;

int * data_csv;

char * able_to_send_division;

char * able_to_send_fusion;

struct loadBalancing ** toSend_loadBalancing;

char * doing_fusion;

int nb_workers_for_stationary;

int nb_answers_written_data_csv;




void tasks_print(int id);

void processing_tasks_print(int id);

void workers_print(xbt_dynar_t * w);

void groups_print(xbt_fifo_t * f);

// this function permits to find if an element of the dynamic array (passed in argument) has as identifier the name in argument
struct p_worker * dynar_search(const char * name, int id);

void send_finalize_to_workers(int id);

void add_new_worker(const char * name, char * myMailbox, int id);

void add_new_worker_change(msg_task_t task, char * myMailbox, int id);

void put_task_fifo(msg_task_t task, int id);

xbt_fifo_item_t fifo_supress_item_head(xbt_fifo_t l);

void * fifo_supress_head(xbt_fifo_t l);

void treat_tasks(xbt_dynar_t * w, msg_task_t * task_to_treat, int id);

void try_to_treat_tasks(char * myMailbox, int id);

struct p_worker * give_worker_dynar(char * name, int id);

struct p_worker * give_worker_active_groups(char * name, int id);

struct p_worker * give_worker_inactive_groups(char * name, int id);

void updateReputation(struct p_task * t, int id);

void add_answers(struct p_task * p_t, xbt_dynar_t * w_answers, char * worker_name, unsigned long int answer, int id);

double valueCond2 (struct p_answer_worker * res, struct p_task * p_t);

// this function permits to know the number of answers that have the same number of workers (nb_majority_answer)
void compute_majoritary_answer(struct p_task * p_t, int * nb_majoritary_answer, double * min_value);

void writes_data (char * client_name, char * task_name, double time_start_task, char fail, unsigned int long answer, long int number_workers_used, double targetLOC, int id);

void send_answer_Sonnek(struct p_task * n, int nb_majoritary_answer, char * process, int id);

char replication(struct p_task * n, int id);

void send_answer_Arantes(struct p_task * n, int nb_majoritary_answer, char * process, char * stop_replication, double max_value, int id);

int inAdditional_replication_tasks (struct p_task * p_t, int id);

void suppress_processing_tasks_and_active_group(struct p_task * n, int id);

void worker_from_active_group_to_workers(char * name, struct p_task * n, int process, int stop_replication, int id);

void worker_from_active_group_to_suppression(char * name, struct p_task * n, int process, int stop_replication, int id);

void treat_answer(msg_task_t t, int crash, int id);

void try_to_treat_additional_replication(int id);

void treat_crash(msg_task_t task_todo);

void compute_name_file (int id);

void send_change(int id);

void try_load_balancing_random_overload (int id, char * mailbox);

void try_load_balancing_reputation_overload (int id, char * mailbox);

void try_load_balancing_overload(int id, char * mailbox);

void stop_division (int id);

void treat_division_overload(msg_task_t task, int id);

void stop_division (int id);

void stop_fusion(int id);

void treat_division_overload(msg_task_t task, int id);

void treat_give_workers(msg_task_t task, int id, char * mailbox);

void forward_to_first_primary(msg_task_t task);

void execute_fusion (int id);

void update_limit_max (msg_task_t task, int id);

void update_limit_min(msg_task_t task, int id);

void destroy_content_fifo(xbt_fifo_t * f);

int primary (int argc, char * argv[]);

#endif
