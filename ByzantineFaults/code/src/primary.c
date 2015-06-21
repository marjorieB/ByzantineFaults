#include "msg/msg.h"
#include "simulator.h"
#include "task.h"
#include "primary.h"
#include "additional_replication_strategy.h"
#include "reputation_strategy.h"
#include "group_formation_strategy.h"
#include "fifo_private.h"
#include <string.h>
#include <math.h>


void tasks_print() {
	xbt_fifo_item_t i;
	msg_task_t * n;

	printf("content of the list \"tasks\":\n");
   for(i = xbt_fifo_get_first_item(tasks); ((i) ? (n = (msg_task_t *)(xbt_fifo_get_item_content(i))) : (NULL)); i = xbt_fifo_get_next_item(i)) {
		printf("client: %s, task: %s\n", (char *)MSG_task_get_data(*n), MSG_task_get_name(*n));
	}
}


void processing_tasks_print() {
	xbt_fifo_item_t i;
	struct p_task * n;

	printf("content of the list \"processing_tasks\":\n");
   for(i = xbt_fifo_get_first_item(processing_tasks); ((i) ? (n = (struct p_task *)(xbt_fifo_get_item_content(i))) : (NULL)); i = xbt_fifo_get_next_item(i)) {
		printf("client: %s, task: %s\n", n->client, n->task_name);
	}
}


void workers_print() {
	struct p_worker p_w;
	unsigned int cpt;

	printf("list of the workers of the system:\n");
	xbt_dynar_foreach (workers, cpt, p_w) {
 		printf("p_w.name= %s, p_w.reputation=%d\n", p_w.mailbox, p_w.reputation);
	}
}


void groups_print(xbt_fifo_t * f) {
	xbt_fifo_item_t i;
	xbt_dynar_t * n;
	int nb = 0;

   for(i = xbt_fifo_get_first_item(*f); ((i) ? (n = (xbt_dynar_t *)(xbt_fifo_get_item_content(i))) : (NULL)); i = xbt_fifo_get_next_item(i)) {
		nb++;
		printf("group number %d\n", nb);
		
		struct p_worker p_w;
		unsigned int cpt;

		xbt_dynar_foreach (*n, cpt, p_w) {
 			printf("p_w.name= %s, p_w.reputation=%d\n", p_w.mailbox, p_w.reputation);
		}
	}
}


// this function permits to find if an element of the dynamic array (passed in argument) has as identifier the name in argument
struct p_worker * dynar_search(const char * name) {
	unsigned int cpt;
	struct p_worker p_w;

	xbt_dynar_foreach (workers, cpt, p_w) {
 		if (!strcmp(p_w.mailbox, name)) {
			return xbt_dynar_get_ptr(workers, cpt);
		}
	}
	return NULL;
}


void send_finalize_to_workers() {
	unsigned int cpt;
	struct p_worker p_w;
	xbt_fifo_item_t i;
	xbt_dynar_t * n;

	// at the end the workers can be in the workers list or in the inactive_groups. But as their is no more tasks from client, it is impossible to have worker in the active groups
	xbt_dynar_foreach (workers, cpt, p_w) {
		msg_task_t finalize = MSG_task_create("finalize", 0, 0, NULL);
		MSG_task_send(finalize, p_w.mailbox);
	}	

   for(i = xbt_fifo_get_first_item(inactive_groups); ((i) ? (n = (xbt_dynar_t *)(xbt_fifo_get_item_content(i))) : (NULL)); i = xbt_fifo_get_next_item(i)) {		
		struct p_worker p_w;
		unsigned int cpt;

		xbt_dynar_foreach (*n, cpt, p_w) {
 			msg_task_t finalize = MSG_task_create("finalize", 0, 0, NULL);
			MSG_task_send(finalize, p_w.mailbox);
		}
	}
}


void add_new_worker(const char * name, char * myMailbox) {
	struct p_worker * w_found;// = (struct p_worker *) malloc(sizeof(struct p_worker));
	msg_task_t ack;
	struct p_worker * worker = (struct p_worker *) malloc(sizeof(struct p_worker));

	// we must verify that it isn't a worker we already have, if it is the case, we need to reset it (the node has been shutdown and the primary didn't seen it.
	if ((w_found = dynar_search(name)) != NULL) {
		w_found->totR = 0;
		w_found->totC = 0;
		if ((reputation_strategy == SYMMETRICAL) || (reputation_strategy == ASYMMETRICAL)) {
			w_found->reputation = INITIAL_VALUE;
		}
		else if (reputation_strategy == BOINC) {
			updateReputation_BOINC(w_found);
		}
		else {
			// the reputation_strategy chosen is the reputation used in Sonnek
			updateReputation_Sonnek(w_found);
		}
	}
	else {
		// creation of a new worker
		strcpy(worker->mailbox, name);
		worker->totR = 0;
		worker->totC = 0;
		if ((reputation_strategy == SYMMETRICAL) || (reputation_strategy == ASYMMETRICAL)) {
			worker->reputation = INITIAL_VALUE;
		}
		else if (reputation_strategy == BOINC) {
			updateReputation_BOINC(worker);
		}
		else {
			// the reputation_strategy chosen is the reputation used in Sonnek
			updateReputation_Sonnek(worker);
		}

		xbt_dynar_push(workers, worker);
	}
	printf("sending acknowledgement to the worker\n");
	ack = MSG_task_create("ack", ACK_COMPUTE_DURATION, ACK_MESSAGE_SIZE, myMailbox);
	MSG_task_send(ack, name);	
}


void put_task_fifo(msg_task_t task) {
	msg_task_t * todo = (msg_task_t *) malloc(sizeof(msg_task_t));

	*todo = MSG_task_create(MSG_task_get_name(task), MSG_task_get_compute_duration(task), MSG_task_get_data_size(task), MSG_task_get_data(task));
	xbt_fifo_push(tasks, todo);
}


xbt_fifo_item_t fifo_supress_item_head(xbt_fifo_t l) {
	xbt_fifo_item_t item;

	if (l->head == NULL) {
		return NULL;
	}
	item = l->head;

	l->head = item->next;
	if (l->head == NULL) {
		l->tail = NULL;
	}
	else {
		l->head->prev = NULL;
	}

	(l->count)--;
	item->next = NULL;

	return item;
}


void * fifo_supress_head(xbt_fifo_t l) {
	xbt_fifo_item_t item;
	void * content;

	if (l == NULL) {
		return NULL;
	}
	if (!(item = fifo_supress_item_head(l))) {
		return NULL;
	}
	
	content = item->content;
	xbt_fifo_free_item(item);
	return content;
}


void treat_tasks() {
	// travel the list of inactive groups and distribute them the tasks in the list tasks
	// put the group that have a task to the list of active group and put the tasks in a list of processing task
	xbt_fifo_item_t i;
	xbt_dynar_t * n;
	int k;
	struct p_worker p_w;
	unsigned int cpt;
	int nb = 0;

   for(i = xbt_fifo_get_first_item(inactive_groups); ((i) ? (n = (xbt_dynar_t *)(xbt_fifo_get_item_content(i))) : (NULL)); i = xbt_fifo_get_next_item(i)) {
		if (xbt_fifo_size(tasks) != 0) {
			struct p_task * t = (struct p_task *) malloc(sizeof(struct p_task));
			msg_task_t * to_process = (msg_task_t *) malloc(sizeof(msg_task_t));

			to_process = fifo_supress_head(tasks);		
			strcpy(t->client, (char *)MSG_task_get_data(*to_process));
			strcpy(t->task_name, MSG_task_get_name(*to_process));
			t->w_answers = xbt_dynar_new(sizeof(struct p_answer_worker), NULL);
			t->duration = MSG_task_get_compute_duration(*to_process);
			t->size = MSG_task_get_data_size(*to_process);
			t->final_answer = BAD_ANSWER + 1; // none of the nodes can answer this
			t->nb_forwarded = xbt_dynar_length(*n);
			t->nb_crashed = 0;
			t->nb_answers_received = 0;
			t->nb_false_answers = 0;
			t->nb_correct_answers = 0;
			t->to_replicate = 0;
			t->additional_reputations = xbt_dynar_new(sizeof(int), NULL);
			t->active_workers = (xbt_dynar_t *)xbt_fifo_get_item_content(xbt_fifo_get_first_item(inactive_groups));

			nb++;

			xbt_dynar_foreach (*n, cpt, p_w) {
				printf("send task %s %s to %s\n", t->client, t->task_name, p_w.mailbox);  
	 			msg_task_t to_send = MSG_task_create(MSG_task_get_name(*to_process), MSG_task_get_compute_duration(*to_process), MSG_task_get_data_size(*to_process), MSG_task_get_data(*to_process));
				MSG_task_send(to_send, p_w.mailbox);
			}

			xbt_fifo_push(processing_tasks, t);
			cpt++;
		}
		else {
			break;
		}
	}
	for (k = 0; k < nb; k++) {
		xbt_fifo_push(active_groups, (xbt_dynar_t *)xbt_fifo_get_item_content(xbt_fifo_get_first_item(inactive_groups)));
		xbt_fifo_remove(inactive_groups, (xbt_dynar_t *)xbt_fifo_get_item_content(xbt_fifo_get_first_item(inactive_groups)));
	}
}


void try_to_treat_tasks() {
	if ((xbt_fifo_size(inactive_groups) != 0) || (xbt_dynar_length(workers) >= group_formation_min_number)) {
		if (xbt_dynar_length(workers) >= group_formation_min_number) {
			if (group_formation_strategy == FIXED_FIT) {
				formGroup_fixed_fit();
			}
			else if (group_formation_strategy == FIRST_FIT) {
				if (simulator == ARANTES) {
					formGroup_first_fit_Arantes();
				}
				else {
					// the simulator chosen is SONNEK
					formGroup_first_fit_Sonnek();
				}
			}
			else if (group_formation_strategy == TIGHT_FIT) {
				if (simulator == ARANTES) {
					formGroup_tight_fit_Arantes();
				}
				else {
					// the simulator chosen is SONNEK
					formGroup_tight_fit_Sonnek();
				}
			}
			else {
				// the group_formation_strategy chosen is the RANDOM_FIT
				if (simulator == ARANTES) {
					formGroup_random_fit_Arantes();
				}
				else {
					// the simulator chosen is SONNEK
					formGroup_random_fit_Sonnek();
				}
			}
			printf("content of the inactive groups\n");
			groups_print(&inactive_groups);
			printf("content of the active groups\n");
			groups_print(&active_groups);
			workers_print();
			printf("value of the number of workers %ld\n", xbt_dynar_length(workers)); 
		}
		treat_tasks();
	}	
}


struct p_worker * give_worker_dynar(char * name) {
	unsigned int cpt;
	struct p_worker p_w;

	xbt_dynar_foreach(workers, cpt, p_w) {
 		if (!strcmp(p_w.mailbox, name)) {	
			return xbt_dynar_get_ptr(workers, cpt);
		}
	}
	return NULL;
}


struct p_worker * give_worker_active_groups(char * name) {
	xbt_fifo_item_t i;
	xbt_dynar_t * n;

	for(i = xbt_fifo_get_first_item(active_groups); ((i) ? (n = (xbt_dynar_t *)(xbt_fifo_get_item_content(i))) : (NULL)); i = xbt_fifo_get_next_item(i)) {
	
		struct p_worker p_w;		
		unsigned int cpt;

		xbt_dynar_foreach(*n, cpt, p_w) { 
	 		if (!strcmp(p_w.mailbox, name)) {
				return xbt_dynar_get_ptr(*n, cpt);
			}
		}
	}
	return NULL;
}


struct p_worker * give_worker_inactive_groups(char * name) {
	xbt_fifo_item_t i;
	xbt_dynar_t * n;

	for(i = xbt_fifo_get_first_item(inactive_groups); ((i) ? (n = (xbt_dynar_t *)(xbt_fifo_get_item_content(i))) : (NULL)); i = xbt_fifo_get_next_item(i)) {
	
		struct p_worker p_w;		
		unsigned int cpt;

		xbt_dynar_foreach(*n, cpt, p_w) {		 	
			if (!strcmp(p_w.mailbox, name)) {
				return xbt_dynar_get_ptr(*n, cpt);					
			}
		}
	}
	return NULL;
}


void updateReputation(struct p_task * t) {
	struct p_answer_worker p_a_w;
	struct p_worker * toModify;
	unsigned int cpt;
	char good_or_bad_answer;

	xbt_dynar_foreach (t->w_answers, cpt, p_a_w) {
		char name[MAILBOX_SIZE];
		unsigned int cpt1;

		xbt_dynar_foreach(p_a_w.worker_names, cpt1, name) {
		// search for the worker on the workers list
			if ((toModify = give_worker_dynar(name)) == NULL) {
				if ((toModify = give_worker_inactive_groups(name)) == NULL) {
					if ((toModify = give_worker_active_groups(name)) == NULL) {
						printf("unknown worker: impossible\n");
						exit(1);
					}
				} 			
			}
			toModify->totR++;
			if (p_a_w.answer == t->final_answer) {
				toModify->totC++;
				good_or_bad_answer = 1;
			}
			else {
				good_or_bad_answer = -1;
			}
			if (reputation_strategy == SYMMETRICAL) {
				updateReputation_Symmetrical(toModify, good_or_bad_answer);
			}
			else if (reputation_strategy == ASYMMETRICAL) {
				updateReputation_Asymmetrical(toModify, good_or_bad_answer);
			}
			else if (reputation_strategy == BOINC) {
				updateReputation_BOINC(toModify);
			}
			else {
				// the reputation_strategy chosen is the reputation used in Sonnek
				updateReputation_Sonnek(toModify);
			}
		}
	}
}


void add_answers(struct p_task * p_t, xbt_dynar_t * w_answers, char * worker_name, unsigned long int answer) {
	unsigned int cpt;
	struct p_answer_worker w_a;
	struct p_answer_worker * toModify = NULL;
	struct p_answer_worker * toAdd = (struct p_answer_worker *) malloc(sizeof(struct p_answer_worker));
	struct p_worker * w;
	
	w = give_worker_active_groups(worker_name);

	xbt_dynar_foreach(*w_answers, cpt, w_a) {
 		if (w_a.answer == answer) {	
			toModify = xbt_dynar_get_ptr(*w_answers, cpt);
		}
	}
	if (toModify == NULL) {
		// the answer returned by the worker hasn't been returned for that request, we create it
		toAdd->answer = answer;
		toAdd->worker_names = xbt_dynar_new(sizeof(char[MAILBOX_SIZE]), NULL);
		toAdd->worker_reputations = xbt_dynar_new(sizeof(unsigned char), NULL);
		xbt_dynar_push(toAdd->worker_names, worker_name);
		xbt_dynar_push(toAdd->worker_reputations, &(w->reputation));
		xbt_dynar_push(p_t->w_answers, toAdd);
	}
	else {
		// the answer returned by the worker has been already returned by other(s) worker(s), we just add the name of the worker to the list of workers having returned this answer
		xbt_dynar_push(toModify->worker_names, worker_name);
		xbt_dynar_push(toModify->worker_reputations, &(w->reputation));
	}
}


double valueCond2 (struct p_answer_worker * res, struct p_task * p_t) {
	double Psc = 1.0;
	double tmp = 1.0;
	unsigned int cpt;
	struct p_answer_worker p;
	unsigned int nb;
	unsigned char reputation;	

	xbt_dynar_foreach(p_t->w_answers, cpt, p) {
		
		if (res == xbt_dynar_get_ptr(p_t->w_answers, cpt)) {
			xbt_dynar_foreach(p.worker_reputations, nb, reputation) {
				Psc = Psc * ((double)reputation/100.0);
				tmp = tmp * (1.0 - ((double)reputation/100.0));
			}
		}
		else {
			xbt_dynar_foreach(p.worker_reputations, nb, reputation) {
				Psc = Psc * (1.0 - ((double)reputation/100.0));
			} 
		}
	}	
	return (Psc / (tmp + Psc));
}


// this function permits to know the number of answers that have the same number of workers (nb_majority_answer)
void compute_majoritary_answer(struct p_task * p_t, int * nb_majoritary_answer, double * min_value) {
	unsigned int cpt;
	struct p_answer_worker w_a;
	struct p_answer_worker * res;
	int max = 0;
	double min = 1000.0;
	double tmp;

	xbt_dynar_foreach(p_t->w_answers, cpt, w_a) {
		if ((simulator == SONNEK) || ((simulator == ARANTES) && (group_formation_strategy == FIXED_FIT))) {
			// if the simulator is SONNEK we considere the number of workers to konw the majoritary answer
	 		if (xbt_dynar_length(w_a.worker_names) > max) {	
				max = xbt_dynar_length(w_a.worker_names);
				*nb_majoritary_answer = 1;
				res = xbt_dynar_get_ptr(p_t->w_answers, cpt);
			}
			else if (xbt_dynar_length(w_a.worker_names) == max) {
				*nb_majoritary_answer = *nb_majoritary_answer + 1;
			}
		}
		else {
			// if the simulator is ARANTES we considere the minimum treshold obtained by the group
			if ((tmp = valueCond2(xbt_dynar_get_ptr(p_t->w_answers, cpt), p_t)) < min) {
				min = tmp;
				*min_value = tmp;
				res = xbt_dynar_get_ptr(p_t->w_answers, cpt);
				*nb_majoritary_answer = 1;
			} 
			else if (tmp == min) {
				*nb_majoritary_answer = *nb_majoritary_answer + 1;	
			}
		}
	}
	p_t->res = res;
}	


void send_answer_Sonnek(struct p_task * n, int nb_majoritary_answer, char * process) {
	if (nb_majoritary_answer == 1 && xbt_dynar_length(n->res->worker_names) > floor((double)n->nb_forwarded/2.0)) {
		printf("send the answer %ld to client %s\n", n->res->answer, n->client);
		// there is no ambiguity on the final result: the majority doesn't necessarly correspond to the absolute majority
		n->final_answer = n->res->answer;
		// send the answer to the client
		msg_task_t answer_client = MSG_task_create("answer", ANSWER_COMPUTE_DURATION, ANSWER_MESSAGE_SIZE, n->task_name);
		MSG_task_send(answer_client, n->client);

		workers_print();
	}	
	else if (nb_majoritary_answer == 1) {
		n->final_answer = n->res->answer;
		printf("send fail to client %s\n", n->client);
		msg_task_t fail = MSG_task_create("fail", ANSWER_COMPUTE_DURATION, ANSWER_MESSAGE_SIZE, n->task_name);
		MSG_task_send(fail, n->client);
	}				
	else {
		// replication of the task on additional nodes
		// if we use Sonnek, there isn't replication mechanism. We just send an error to the client indicating him the primary fail obtaining an (absolute or qualify) majority in the answers collected
		printf("send fail to client %s\n", n->client);
		msg_task_t fail = MSG_task_create("fail", ANSWER_COMPUTE_DURATION, ANSWER_MESSAGE_SIZE, n->task_name);
		MSG_task_send(fail, n->client);
	}
	// in case of Sonnek what ever happen, when all the workers have answer the task is processed (because there isn't any addtional replication strategy)
	*process = 1;
	// update the reputation of the workers and suppress the processing task
	updateReputation(n);
}


void replication(struct p_task * n) {
	n->nb_replication++;
	if (n->nb_replication == REPLICATION_MAX) {
		msg_task_t fail = MSG_task_create("fail", ANSWER_COMPUTE_DURATION, ANSWER_MESSAGE_SIZE, n->task_name);
		MSG_task_send(fail, n->client);
	}

	if (group_formation_strategy == FIXED_FIT) {
		replication_fixed_fit(n);
	}
	else {
		replication_others_fit(n);
	}
}


void send_answer_Arantes(struct p_task * n, int nb_majoritary_answer, char * process, double min_value) {
	if (group_formation_strategy == FIXED_FIT) {
		if (additional_replication_strategy == ITERATIVE_REDUNDANCY) {
			if ((nb_majoritary_answer == 1) && ((n->nb_forwarded - xbt_dynar_length(n->res->worker_names)) == additional_replication_value_difference)) {
				n->final_answer = n->res->answer;
				// send the answer to the client
				msg_task_t answer_client = MSG_task_create("answer", ANSWER_COMPUTE_DURATION, ANSWER_MESSAGE_SIZE, n->task_name);
				MSG_task_send(answer_client, n->client);

				*process = 1;
				updateReputation(n);
			}
			else {
				// replication
				replication(n);
			}
		}
		else {
			// additionnal_replication_strategy == PROGRESSIVE_REDUNDANCY
			if ((nb_majoritary_answer == 1) && (xbt_dynar_length(n->res->worker_names) >= (floor((double)group_formation_fixed_number/2.0) + 1))) {
				n->final_answer = n->res->answer;
				// send the answer to the client
				msg_task_t answer_client = MSG_task_create("answer", ANSWER_COMPUTE_DURATION, ANSWER_MESSAGE_SIZE, n->task_name);
				MSG_task_send(answer_client, n->client);

				*process = 1;
				updateReputation(n);
			}
			else {
				replication(n);
			}
		}
	}
	else if (((nb_majoritary_answer == 1) && (xbt_dynar_length(n->res->worker_names) == n->nb_forwarded)) || (min_value > (1 - group_formation_target_value))) {
		// there is no ambiguity on the final result
		n->final_answer = n->res->answer;
		// send the answer to the client
		msg_task_t answer_client = MSG_task_create("answer", ANSWER_COMPUTE_DURATION, ANSWER_MESSAGE_SIZE, n->task_name);
		MSG_task_send(answer_client, n->client);

		*process = 1;
		updateReputation(n);
	}
	else {
		// replication of the task on additional nodes
		replication(n);
	}
}


int inAdditional_replication_tasks (struct p_task * p_t) {
	xbt_fifo_item_t i;
	struct p_task * n;

   for(i = xbt_fifo_get_first_item(processing_tasks); ((i) ? (n = (struct p_task *)(xbt_fifo_get_item_content(i))) : (NULL)); i = xbt_fifo_get_next_item(i)) {
		if (n == p_t) {
			return 1;
		}
	}
	return -1;
}


void suppress_processing_tasks_and_active_group(xbt_dynar_t * d, struct p_task * n) {
	// suppress the group in active groups
	if (xbt_fifo_remove(active_groups, d) != 1) {
		printf("problem detected when removing a xbt_dynar_t from active_groups\n");
	}
	else {
		printf("supression of the active group\n");
	}
	// suppress the task (because it has been accomplished) from the processing_tasks
	if(xbt_fifo_remove(processing_tasks, n) != 1) {
		printf("problem detected when removing a struct p_task from processing_tasks\n");
	}
	else {
		printf("task suppressed from the processing_tasks\n");
	}
}


void worker_from_active_group_to_workers(char * name, struct p_task * n, int process) {

	// move the worker from the active groups from the workers
	xbt_fifo_item_t j;
	xbt_dynar_t * d;
	struct p_worker * toAddToWorkers = (struct p_worker *) malloc(sizeof(struct p_worker));

	for(j = xbt_fifo_get_first_item(active_groups); ((j) ? (d = (xbt_dynar_t *)(xbt_fifo_get_item_content(j))) : (NULL)); j = xbt_fifo_get_next_item(j)) {

		struct p_worker p_w;
		unsigned int cpt;

		xbt_dynar_foreach (*d, cpt, p_w) {
	 		if (!strcmp(p_w.mailbox, name)) {
				break;
			}
		}
		xbt_dynar_remove_at(*d, cpt, toAddToWorkers);
		break;
	}

	xbt_dynar_push(workers, toAddToWorkers);
	if (process == 1) {
		suppress_processing_tasks_and_active_group(d, n);
	}
}


void worker_from_active_group_to_suppression(char * name, struct p_task * n, int process) {
	// remove the worker from the system since it has crashed
	xbt_fifo_item_t j;
	xbt_dynar_t * d;
	struct p_worker * toSuppress = NULL;

	for(j = xbt_fifo_get_first_item(active_groups); ((j) ? (d = (xbt_dynar_t *)(xbt_fifo_get_item_content(j))) : (NULL)); j = xbt_fifo_get_next_item(j)) {

		struct p_worker p_w;
		unsigned int cpt;

		xbt_dynar_foreach (*d, cpt, p_w) {
	 		if (!strcmp(p_w.mailbox, name)) {
				printf("reputation dans dynar %d\n", p_w.reputation);
				break;
			}
		}
		xbt_dynar_remove_at(*d, cpt, toSuppress);
		break;
	}

	free(toSuppress);

	if (process == 1) {
		suppress_processing_tasks_and_active_group(d, n);
	}
}


void treat_answer(msg_task_t t, int crash) {
	// update the answer of the worker
	// check if all workers give the answer: if yes, look up for the majority solution
	// if all workers answer the same way, give the answer to the client and update the reputation, and put all the workers on the workers dynamic array
	// if not ask the requests to additional nodes (that the primary put in the same active group than the previous answered nodes
	struct w_task * w_t = (struct w_task *) malloc(sizeof(struct w_task));
	xbt_fifo_item_t i;
	struct p_task * n;
	char process = -1;
	int nb_majoritary_answer = 0;
	double min_value;

	w_t = (struct w_task *)MSG_task_get_data(t);
	printf("received %s %s %s %ld\n", w_t->client, w_t->worker_name, w_t->task_name, w_t->answer);


	// the primary have to find the element of processing_tasks that correspond to the answer
   for(i = xbt_fifo_get_first_item(processing_tasks); ((i) ? (n = (struct p_task *)(xbt_fifo_get_item_content(i))) : (NULL)); i = xbt_fifo_get_next_item(i)) {
		printf("processing task found\n");
		if ((strcmp(n->client, w_t->client) == 0) && (strcmp(n->task_name, w_t->task_name) == 0)) {
			printf("primary: add of a new answer\n");
			// add the worker to w_answers
			add_answers(n, &(n->w_answers), w_t->worker_name, w_t->answer);

			if (crash == 1) {
				n->nb_crashed++;
			}
			else {
				n->nb_answers_received++;
				if (w_t->answer > GOOD_ANSWER) {
					n->nb_false_answers++;
				}
				else {
					n->nb_correct_answers++;
				}
			}
			if (((n->nb_answers_received + n->nb_crashed) == n->nb_forwarded) && (inAdditional_replication_tasks(n) == 1)) {
				n->to_replicate = 0;
				xbt_dynar_free(&(n->additional_reputations));
				n->additional_reputations = xbt_dynar_new(sizeof(int), NULL);

				printf("primary: I have receive all the answers for that task\n");
				compute_majoritary_answer(n, &nb_majoritary_answer, &min_value); 		

				printf("receive all the answers for %s %s\n", n->client, n->task_name);
				if (simulator == SONNEK) {
					send_answer_Sonnek(n, nb_majoritary_answer, &process);
				}
				else {
					// we simulate the algorithm used in ARANTES
					send_answer_Arantes(n, nb_majoritary_answer, &process, min_value);
				}
			}
			break;
		}
	}
	
	if (crash == 1) {
		worker_from_active_group_to_suppression(w_t->worker_name, n, process);
	}
	else {
		worker_from_active_group_to_workers(w_t->worker_name, n, process);
	}
}


void try_to_treat_additional_replication() {
	int i;
	int size = xbt_fifo_size(additional_replication_tasks);

	for (i = 0; i < size; i++) {
		if ((xbt_fifo_size(inactive_groups) != 0) || (xbt_dynar_length(workers) >= 0)) {
			if (group_formation_strategy == FIXED_FIT) {
				replication_fixed_fit((struct p_task *)fifo_supress_head(additional_replication_tasks));
			}
			else {
				replication_others_fit((struct p_task *)fifo_supress_head(additional_replication_tasks));
			}
		}
		else {
			break;
		}
	}
}


int primary (int argc, char * argv[]) {
	unsigned long int id;
	int nb_clients;
	char myMailbox[MAILBOX_SIZE];
	int nb_finalize = 0;
	msg_task_t task_todo = NULL;

	if (argc != 3) {
		exit(1);
	}

	workers = xbt_dynar_new(sizeof(struct p_worker), NULL);
	tasks = xbt_fifo_new();
	processing_tasks = xbt_fifo_new();
	active_groups = xbt_fifo_new();
	inactive_groups = xbt_fifo_new();
	additional_replication_tasks = xbt_fifo_new();

	id = atoi(argv[1]);
	sprintf(myMailbox, "primary-%ld", id);
	nb_clients = atoi(argv[2]);

	while (1) {
		// reception of a message
		MSG_task_receive(&(task_todo), myMailbox);

		/* the primary can receive 4 types of messages: requests from client, 
			finalization from client, join from workers, or answer to tasks of workers */
		if (!strcmp(MSG_task_get_name(task_todo), "finalize")) {
			printf("%s: I receive finalize\n", myMailbox);
			MSG_task_destroy(task_todo);
			task_todo = NULL;
			nb_finalize++;
			if (nb_finalize == nb_clients) {
				// if all clients have finish to send requests, the primary ask the workers to stop
				send_finalize_to_workers(); 
				break;
			}
		}
		else if (!strncmp(MSG_task_get_name(task_todo), "task", sizeof(char) * strlen("task"))) {
			// the primary put the task to do in a fifo
			printf("%s: I receive a task\n", myMailbox);
			put_task_fifo(task_todo);
			MSG_task_destroy(task_todo);
			task_todo = NULL;
			try_to_treat_tasks();
		}
		else if (!strncmp(MSG_task_get_name(task_todo), "join", sizeof(char) * strlen("join"))) {
			// a worker want to join the system		
			printf("%s: I receive a join\n", myMailbox);	
			add_new_worker((char *)MSG_task_get_data(task_todo), myMailbox);
			MSG_task_destroy(task_todo);
			task_todo = NULL;
			if (simulator == ARANTES) {
				try_to_treat_additional_replication();
			}
			try_to_treat_tasks();
		}
		else if (!strncmp(MSG_task_get_name(task_todo), "answer", sizeof(char) * strlen("answer"))) {
			// the primary receive an answer to a request from a worker
			printf("%s: I receive an answer\n", myMailbox);
			treat_answer(task_todo, -1);
			if (simulator == ARANTES) {
				try_to_treat_additional_replication();
			}
			try_to_treat_tasks();
			MSG_task_destroy(task_todo);
			task_todo = NULL;
		}
		else if (!strncmp(MSG_task_get_name(task_todo), "crash", sizeof(char) * strlen("crash"))) {
			printf("%s: I receive a message indicating the crash of %s\n", myMailbox, (char *) MSG_task_get_data(task_todo));
			treat_answer(task_todo, 1);
			MSG_task_destroy(task_todo);
			task_todo = NULL;
		}
		else {
			// messages incorrect
			printf("%s: I receive an incorrect message\n", myMailbox);
			MSG_task_destroy(task_todo);
			task_todo = NULL;
		}
	}

	return 0;
}

























