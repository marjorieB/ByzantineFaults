#include "msg/msg.h"
#include "simulator.h"
#include "task.h"
#include "primary.h"
#include "additional_replication_strategy.h"
#include "reputation_strategy.h"
#include "group_formation_strategy.h"
#include "fifo_private.h"
#include "client.h"
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


void tasks_print(int id) {
	xbt_fifo_item_t i;
	msg_task_t * n;

	printf("content of the list \"tasks\" dans primary-%d:\n", id);
   for(i = xbt_fifo_get_first_item(tasks[id]); ((i) ? (n = (msg_task_t *)(xbt_fifo_get_item_content(i))) : (NULL)); i = xbt_fifo_get_next_item(i)) {
		printf("client: %s, task: %s\n", (char *)MSG_task_get_data(*n), MSG_task_get_name(*n));
	}
}


void processing_tasks_print(int id) {
	xbt_fifo_item_t i;
	struct p_task * n;

	printf("content of the list \"processing_tasks\" dans primary-%d:\n", id);
   for(i = xbt_fifo_get_first_item(processing_tasks[id]); ((i) ? (n = (struct p_task *)(xbt_fifo_get_item_content(i))) : (NULL)); i = xbt_fifo_get_next_item(i)) {
		printf("client: %s, task: %s\n", n->client, n->task_name);
	}
}


void workers_print(xbt_dynar_t * w) {
	struct p_worker p_w;
	unsigned int cpt;

	printf("list of the workers of the system for: %ld\n", xbt_dynar_length(*w));
	xbt_dynar_foreach (*w, cpt, p_w) {
 		printf("p_w.name= %s, p_w.reputation=%d\n", p_w.mailbox, p_w.reputation);
	}
	printf("value of cpt = %u\n", cpt);
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
struct p_worker * dynar_search(const char * name, int id) {
	unsigned int cpt;
	struct p_worker p_w;

	xbt_dynar_foreach (workers[id], cpt, p_w) {
 		if (!strcmp(p_w.mailbox, name)) {
			msg_task_t task_complexity = MSG_task_create("task_complexity", cpt, 0, NULL);
			MSG_task_execute(task_complexity);
			MSG_task_destroy(task_complexity);
			return xbt_dynar_get_ptr(workers[id], cpt);
		}
	}
	msg_task_t task_complexity = MSG_task_create("task_complexity", cpt, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
	return NULL;
}


void send_finalize_to_workers(int id) {
	unsigned int cpt;
	struct p_worker p_w;

	// at the end the workers can be in the workers list. But as their is no more tasks from client, it is impossible to have worker in the active groups
	printf("primary-%d: send finalize to workers\n", id);
	xbt_dynar_foreach (workers[id], cpt, p_w) {
		msg_task_t finalize = MSG_task_create("finalize", 0, 0, NULL);
		MSG_task_send(finalize, p_w.mailbox);
	}	
}


void add_new_worker(const char * name, char * myMailbox, int id) {
	printf("%s add the worker %s\n", myMailbox, name);
	struct p_worker * w_found;// = (struct p_worker *) malloc(sizeof(struct p_worker));
	msg_task_t ack;
	struct p_worker * worker = (struct p_worker *) malloc(sizeof(struct p_worker));

	double complexity = 0.0;

	// we must verify that it isn't a worker we already have, if it is the case, we need to reset it (the node has been shutdown and the primary didn't seen it.
	if ((w_found = dynar_search(name, id)) != NULL) {
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
		complexity += 5.0;
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

		xbt_dynar_push(workers[id], worker);
		complexity += 7.0;
	}
	ack = MSG_task_create("ack", ACK_COMPUTE_DURATION, (strlen("ack") + strlen(myMailbox)) * sizeof(char), myMailbox);
	
	MSG_task_send(ack, name);	

	complexity += 2.0;
	msg_task_t task_complexity = MSG_task_create("task_complexity", complexity, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
	//free(worker);
}


void add_new_worker_change(msg_task_t task, char * myMailbox, int id) {
	msg_task_t ack;
	struct p_worker * worker = (struct p_worker *) malloc(sizeof(struct p_worker));
	worker = (struct p_worker *) MSG_task_get_data(task);
	
	xbt_dynar_push(workers[id], worker);
	
	//ack = MSG_task_create("ackchange", ACK_COMPUTE_DURATION, (strlen("ackchange") + strlen(myMailbox)) * sizeof(char), myMailbox);
	ack = MSG_task_create("ack", ACK_COMPUTE_DURATION, (strlen("ack") + strlen(myMailbox)) * sizeof(char), myMailbox);
	MSG_task_send(ack, worker->mailbox);	

	msg_task_t task_complexity = MSG_task_create("task_complexity", 2.0, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
}


void put_task_fifo(msg_task_t task, int id) {
	msg_task_t * todo = (msg_task_t *) malloc(sizeof(msg_task_t));

	*todo = MSG_task_create(MSG_task_get_name(task), MSG_task_get_compute_duration(task), task_message_size, MSG_task_get_data(task));
	xbt_fifo_push(tasks[id], todo);

	msg_task_t task_complexity = MSG_task_create("task_complexity", 2.0, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
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


void treat_tasks(xbt_dynar_t * w, msg_task_t * task_to_treat, int id) {
	// put the group that have a task to the list of active group and put the tasks in a list of processing task
	int nb = 0;

	struct p_task * t = (struct p_task *) malloc(sizeof(struct p_task));
	struct clientDataTask * data;// = (struct clientDataTask *) malloc(sizeof(struct clientDataTask));

	data = (struct clientDataTask *) MSG_task_get_data(*task_to_treat);
	t->start = data->start_time;
	strcpy(t->client, data->mailbox);
	t->targetLOC = data->target_LOC;
	strcpy(t->task_name, MSG_task_get_name(*task_to_treat));
	t->w_answers = xbt_dynar_new(sizeof(struct p_answer_worker), NULL);
	t->duration = MSG_task_get_compute_duration(*task_to_treat);
	t->size = task_message_size;
	t->final_answer = BAD_ANSWER + 1; // none of the nodes can answer this
	t->nb_forwarded = xbt_dynar_length(*w);
	t->nb_crashed = 0;
	t->nb_answers_received = 0;
	t->nb_false_answers = 0;
	t->nb_correct_answers = 0;
	t->to_replicate = 0;
	t->additional_workers = NULL;
	t->additional_reputations = NULL;
	t->nb_replication = 0;
	//t->additional_workers = xbt_dynar_new((sizeof(char) * MAILBOX_SIZE), NULL);
	//t->additional_reputations = xbt_dynar_new(sizeof(unsigned char), NULL);
	t->active_workers = xbt_fifo_push(active_groups[id], w);

	printf("primary-0: treat_tasks %s %s\n", t->client, t->task_name);
	nb++;
	struct p_worker p_w;
	unsigned int cpt;

	xbt_dynar_foreach (*w, cpt, p_w) {
		printf("send task %s %s to %s\n", t->client, t->task_name, p_w.mailbox);  
		msg_task_t to_send = MSG_task_create(MSG_task_get_name(*task_to_treat), MSG_task_get_compute_duration(*task_to_treat), task_message_size, t->client);
		MSG_task_send(to_send, p_w.mailbox);
	}

	xbt_fifo_push(processing_tasks[id], t);
	xbt_fifo_remove(tasks[id], task_to_treat);	
	//tasks_print(id);


	msg_task_t task_complexity = MSG_task_create("task_complexity", 24.0 + cpt, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
	//free(t);
}


void try_to_treat_tasks(char * mailbox, int id) {
	double complexity = 0.0;
	char ret = 0;

	printf("primary-0: in try_to_treat_tasks: nb_workers=%ld, nb_tasks=%d\n", xbt_dynar_length(workers[id]), xbt_fifo_size(tasks[id]));
	if (xbt_dynar_length(workers[id]) >= group_formation_min_number) {

		xbt_fifo_item_t i;
		msg_task_t * task_to_treat;

		for(i = xbt_fifo_get_first_item(tasks[id]); ((i) ? (task_to_treat = (msg_task_t *)(xbt_fifo_get_item_content(i))) : (NULL)); i = xbt_fifo_get_next_item(i)) {
			complexity += 4.0;
			if (xbt_dynar_length(workers[id]) >= group_formation_min_number) {
				if (group_formation_strategy == FIXED_FIT) {
					complexity += 3.0;
					ret = formGroup_fixed_fit(task_to_treat, id);
				}
				else if (group_formation_strategy == FIRST_FIT) {
					if (simulator == ARANTES) {
						ret = formGroup_first_fit_Arantes(task_to_treat, id);
					}
					else {
						// the simulator chosen is SONNEK
						ret = formGroup_first_fit_Sonnek(task_to_treat, id);
					}
					complexity += 5.0;
				}
				else if (group_formation_strategy == TIGHT_FIT) {
					if (simulator == ARANTES) {
						ret = formGroup_tight_fit_Arantes(task_to_treat, id);
					}
					else {
						// the simulator chosen is SONNEK
						ret = formGroup_tight_fit_Sonnek(task_to_treat, id);
					}
					complexity += 6.0;
				}
				else {
					// the group_formation_strategy chosen is the RANDOM_FIT
					if (simulator == ARANTES) {
						ret = formGroup_random_fit_Arantes(task_to_treat, id);
					}
					else {
						// the simulator chosen is SONNEK
						ret = formGroup_random_fit_Sonnek(task_to_treat, id);
					}
					complexity += 6.0;
				}
				if (random_target_LOC == NOT_RANDOM) {
					if (ret == -1) {
						break;
					}
				}
			}
			else {
				complexity += 2.0;
				break;
			}
		}
		if (centrality == DISTRIBUTED) {
			if((xbt_fifo_size(active_groups[id]) == 0) && (xbt_fifo_size(tasks[id]) != 0)) {
				// we have tried to form group but we have'nt been able to do so
				if (able_to_send_fusion[id] == 1) {
					doing_fusion[id] = 1;
					MSG_comm_destroy(MSG_task_isend(MSG_task_create("fusion", 0, (strlen("fusion") + strlen(mailbox)) * sizeof(char), mailbox), first_primary_name));
				}
			}
		}
	}
	msg_task_t task_complexity = MSG_task_create("task_complexity", complexity + 2.0, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
}


struct p_worker * give_worker_dynar(char * name, int id) {
	unsigned int cpt;
	struct p_worker p_w;

	xbt_dynar_foreach(workers[id], cpt, p_w) {
 		if (!strcmp(p_w.mailbox, name)) {
			msg_task_t task_complexity = MSG_task_create("task_complexity", cpt, 0, NULL);	
			MSG_task_execute(task_complexity);
			MSG_task_destroy(task_complexity);
			return xbt_dynar_get_ptr(workers[id], cpt);
		}
	}
	msg_task_t task_complexity = MSG_task_create("task_complexity", cpt, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
	return NULL;
}


struct p_worker * give_worker_active_groups(char * name, int id) {
	xbt_fifo_item_t i;
	xbt_dynar_t * n;
	double complexity = 0.0;

	for(i = xbt_fifo_get_first_item(active_groups[id]); ((i) ? (n = (xbt_dynar_t *)(xbt_fifo_get_item_content(i))) : (NULL)); i = xbt_fifo_get_next_item(i)) {
	
		struct p_worker p_w;		
		unsigned int cpt;

		xbt_dynar_foreach(*n, cpt, p_w) { 
	 		if (!strcmp(p_w.mailbox, name)) {
				msg_task_t task_complexity = MSG_task_create("task_complexity", complexity + cpt + 4.0, 0, NULL);
				MSG_task_execute(task_complexity);
				MSG_task_destroy(task_complexity);
				return xbt_dynar_get_ptr(*n, cpt);
			}
		}
		complexity += cpt;
	}
	msg_task_t task_complexity = MSG_task_create("task_complexity", complexity + 4.0, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
	return NULL;
}


void updateReputation(struct p_task * t, int id) {
	printf("primary-%d: in updateReputation\n", id); 
	struct p_answer_worker p_a_w;
	struct p_worker * toModify;
	unsigned int cpt;
	char good_or_bad_answer;
	double complexity = 0.0;

	xbt_dynar_foreach (t->w_answers, cpt, p_a_w) {
		char name[MAILBOX_SIZE];
		unsigned int cpt1;

		xbt_dynar_foreach(p_a_w.worker_names, cpt1, name) {
		// search for the worker on the workers list
			if ((toModify = give_worker_dynar(name, id)) == NULL) {
				complexity += 2.0;
				if ((toModify = give_worker_active_groups(name, id)) == NULL) {
					// if we don't find the worker it's because it has crashed and we have suppressed it from the system
					continue;
				} 			
			}
			complexity += 4.0;
			toModify->totR++;
			if (p_a_w.answer == t->final_answer) {
				toModify->totC++;
				good_or_bad_answer = 1;
				complexity += 2.0;
			}
			else {
				good_or_bad_answer = -1;	
				complexity++;
			}
			if (reputation_strategy == SYMMETRICAL) {
				updateReputation_Symmetrical(toModify, good_or_bad_answer);
				complexity++;
			}
			else if (reputation_strategy == ASYMMETRICAL) {
				updateReputation_Asymmetrical(toModify, good_or_bad_answer);
				complexity += 2.0;
			}
			else if (reputation_strategy == BOINC) {
				updateReputation_BOINC(toModify);
				complexity += 3.0;
			}
			else {
				// the reputation_strategy chosen is the reputation used in Sonnek
				updateReputation_Sonnek(toModify);
				complexity += 3.0;
			}
			if ((centrality == DISTRIBUTED) && (distributed_strategies == REPUTATIONS)) {
				printf("value reputation_max primary %d, value reputation_min primary %d, value reputation toModify %d\n", reputations_primary[id].max_reputation, reputations_primary[id].min_reputation, toModify->reputation); 
				if (blacklist == BLACKLIST) {
					if (toModify->reputation < 45) {
						printf("ejected %s beacause of BLACKLIST\n", toModify->mailbox);
						// we just suppress the workers from the system
						unsigned cpt;
						struct p_worker w;
						char found = -1;

						xbt_dynar_foreach(workers[id], cpt, w) {
							if (!(strcmp(w.mailbox, toModify->mailbox))) {
								found = 1;
								break;		
							}
						}
				
						if (found == 1) {
							found = -1;
							xbt_dynar_remove_at(workers[id], cpt, NULL);
						}
						complexity += cpt;

						MSG_task_send(MSG_task_create("ejected", 0, 0, NULL), toModify->mailbox);
					}
				}
				if ((toModify->reputation >= reputations_primary[id].max_reputation) || (toModify->reputation < reputations_primary[id].min_reputation)) {
					// we have to suppress the node
					complexity++;
					printf("the worker %s hasn't the good reputation, WE HAVE TO CHANGE ITS PRIMARY\n", toModify->mailbox);
					xbt_dynar_push(to_change_primary[id], toModify);
				} 
			}
		}
		nb_workers_for_stationary += xbt_dynar_length(p_a_w.worker_names);
	}

	msg_task_t task_complexity = MSG_task_create("task_complexity", complexity + 4.0, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
}


void add_answers(struct p_task * p_t, xbt_dynar_t * w_answers, char * worker_name, unsigned long int answer, int id) {
	unsigned int cpt;
	struct p_answer_worker w_a;
	struct p_answer_worker * toModify = NULL;
	struct p_worker * w;
	double complexity = 0.0;

	printf("primary-%d: I receive an answer of %s for the task %s %s\n", id, worker_name, p_t->client, p_t->task_name);
	
	w = give_worker_active_groups(worker_name, id);

	xbt_dynar_foreach(*w_answers, cpt, w_a) {
 		if (w_a.answer == answer) {
			complexity += 2.0;	
			toModify = xbt_dynar_get_ptr(*w_answers, cpt);
			break;
		}
	}
	complexity = complexity + cpt;

	if (toModify == NULL) {
		// the answer returned by the worker hasn't been returned for that request, we create it
		struct p_answer_worker * toAdd = (struct p_answer_worker *) malloc(sizeof(struct p_answer_worker));
		toAdd->answer = answer;
		toAdd->worker_names = xbt_dynar_new(sizeof(char[MAILBOX_SIZE]), NULL);
		toAdd->worker_reputations = xbt_dynar_new(sizeof(unsigned char), NULL);
		xbt_dynar_push(toAdd->worker_names, worker_name);
		xbt_dynar_push(toAdd->worker_reputations, &(w->reputation));
		xbt_dynar_push(p_t->w_answers, toAdd);
		free(toAdd);
		complexity += 7.0;
	}
	else {
		// the answer returned by the worker has been already returned by other(s) worker(s), we just add the name of the worker to the list of workers having returned this answer
		xbt_dynar_push(toModify->worker_names, worker_name);
		xbt_dynar_push(toModify->worker_reputations, &(w->reputation));
		complexity += 2.0;
	}

	msg_task_t task_complexity = MSG_task_create("task_complexity", complexity + 1.0, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
}


double valueCond2 (struct p_answer_worker * res, struct p_task * p_t) {
	double Psc = 1.0;
	double tmp = 1.0;
	unsigned int cpt;
	struct p_answer_worker p;
	unsigned int nb;
	unsigned char reputation;
	double complexity = 0.0;	

	xbt_dynar_foreach(p_t->w_answers, cpt, p) {
		
		if (res == xbt_dynar_get_ptr(p_t->w_answers, cpt)) {
			xbt_dynar_foreach(p.worker_reputations, nb, reputation) {
				Psc = Psc * ((double)reputation/100.0);
				tmp = tmp * (1.0 - ((double)reputation/100.0));
			}
			complexity = complexity + 7.0 * nb;
		}
		else {
			xbt_dynar_foreach(p.worker_reputations, nb, reputation) {
				Psc = Psc * (1.0 - ((double)reputation/100.0));
			} 
			complexity = complexity + 4.0 * nb;
		}
		complexity += 2.0;
	}	

	msg_task_t task_complexity = MSG_task_create("task_complexity", complexity + 2.0, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);

	return (Psc / (tmp + Psc));
}


// this function permits to know the number of answers that have the same number of workers (nb_majority_answer)
void compute_majoritary_answer(struct p_task * p_t, int * nb_majoritary_answer, double * max_value) {
	unsigned int cpt;
	struct p_answer_worker w_a;
	struct p_answer_worker * res;
	int max = 0;
	//double min = 1000.0;
	double max_double = 0.0;
	double tmp;
	double complexity = 0.0;

	xbt_dynar_foreach(p_t->w_answers, cpt, w_a) {
		if ((simulator == SONNEK) || ((simulator == ARANTES) && (group_formation_strategy == FIXED_FIT))) {
			// if the simulator is SONNEK we consider the number of workers to kown the majoritary answer
	 		if (xbt_dynar_length(w_a.worker_names) > max) {	
				max = xbt_dynar_length(w_a.worker_names);
				*nb_majoritary_answer = 1;
				res = xbt_dynar_get_ptr(p_t->w_answers, cpt);
				complexity += 7.0;
			}
			else if (xbt_dynar_length(w_a.worker_names) == max) {
				*nb_majoritary_answer = *nb_majoritary_answer + 1;
				complexity += 6.0;
			}
		}
		else {
			// if the simulator is ARANTES we consider the minimum treshold obtained by the group
			if ((tmp = valueCond2(xbt_dynar_get_ptr(p_t->w_answers, cpt), p_t)) > max_double) {
				max_double = tmp;
				*max_value = tmp;
				res = xbt_dynar_get_ptr(p_t->w_answers, cpt);
				*nb_majoritary_answer = 1;
				complexity += 8.0;
			} 
			else if (tmp == max) {
				*nb_majoritary_answer = *nb_majoritary_answer + 1;	
				complexity += 6.0;
			}
		}
		complexity += 3.0;
	}
	complexity++;

	msg_task_t task_complexity = MSG_task_create("task_complexity", complexity, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
	p_t->res = res;
}	


void writes_data (char * client_name, char * task_name, double time_start_task, char fail, unsigned int long answer, long int number_workers_used, double targetLOC, int id) {
	char toWrite[BUFFER_SIZE];
	char separator = ';';

	write(data_csv[id], client_name, sizeof(char) * strlen(client_name));
	write(data_csv[id], &separator, sizeof(char));
	write(data_csv[id], task_name, sizeof(char) * strlen(task_name));
	write(data_csv[id], &separator, sizeof(char));
	
	sprintf(toWrite, "%f;", time_start_task);
	write(data_csv[id], toWrite, sizeof(char) * strlen(toWrite));

	sprintf(toWrite, "%f;", MSG_get_clock());
	write(data_csv[id], toWrite, sizeof(char) * strlen(toWrite));

	sprintf(toWrite, "%d;", fail);
	write(data_csv[id], toWrite, sizeof(char) * strlen(toWrite));

	sprintf(toWrite, "%lu;", answer);
	write(data_csv[id], toWrite, sizeof(char) * strlen(toWrite));

	
	xbt_fifo_item_t i;
	xbt_dynar_t * n;
	int nb = 0;
	int to_write = 0;

	 for(i = xbt_fifo_get_first_item(active_groups[id]); ((i) ? (n = (xbt_dynar_t *)(xbt_fifo_get_item_content(i))) : (NULL)); i = xbt_fifo_get_next_item(i)) {
		nb++;
		printf("group number %d\n", nb);
		
		to_write += xbt_dynar_length(*n);
 	}

	sprintf(toWrite, "%ld;%ld;\n", number_workers_used, xbt_dynar_length(workers[id]) + to_write);
	write(data_csv[id], toWrite, sizeof(char) * strlen(toWrite));
	
	nb_answers_written_data_csv++;
	if (nb_answers_written_data_csv >= 300) {
		MSG_process_killall(-1);
		exit(0);
	}
}


void send_answer_Sonnek(struct p_task * n, int nb_majoritary_answer, char * process, int id) {
	double complexity = 0.0;

	if ((nb_majoritary_answer == 1) && (xbt_dynar_length(n->res->worker_names) > floor((double)n->nb_forwarded/2.0))) {
		printf("primary-%d: send the answer %ld to client %s for %s\n", id, n->res->answer, n->client, n->task_name);
		// there is no ambiguity on the final result: the majority doesn't necessarly correspond to the absolute majority
		n->final_answer = n->res->answer;
		if (nb_workers_for_stationary >= stationary_regime) {
			writes_data(n->client, n->task_name, n->start, -1, n->res->answer, n->nb_forwarded, n->targetLOC, id);//xbt_dynar_length(n->res->worker_names), id);
		}
		// send the answer to the client
		msg_task_t answer_client = MSG_task_create("answer", ANSWER_COMPUTE_DURATION, ANSWER_MESSAGE_SIZE, n->task_name);
		MSG_task_send(answer_client, n->client);

		complexity += 6.0;
		//printf("in primary-%d, list of workers\n", id);
		//workers_print(&(workers[id]));
	}	
	else if (nb_majoritary_answer == 1) {
		n->final_answer = n->res->answer;
		if (nb_workers_for_stationary >= stationary_regime) {
			writes_data(n->client, n->task_name, n->start, 0, n->res->answer, n->nb_forwarded, n->targetLOC, id);//xbt_dynar_length(n->res->worker_names), id);
		}	
		printf("primary-%d: send fail to client %s for %s\n", id, n->client, n->task_name);
		msg_task_t fail = MSG_task_create("fail", ANSWER_COMPUTE_DURATION, ANSWER_MESSAGE_SIZE, n->task_name);
		MSG_task_send(fail, n->client);

		complexity += 7.0;
	}				
	else {
		if (nb_workers_for_stationary >= stationary_regime) {
			writes_data(n->client, n->task_name, n->start, 1, 0, n->nb_forwarded, n->targetLOC, id);//xbt_dynar_length(n->res->worker_names), id);
		}
		// replication of the task on additional nodes
		// if we use Sonnek, there isn't replication mechanism. We just send an error to the client indicating him the primary fail obtaining an (absolute or qualify) majority in the answers collected
		printf("primary-%d: send fail to client %s for %s\n", id, n->client, n->task_name);
		msg_task_t fail = MSG_task_create("fail", ANSWER_COMPUTE_DURATION, ANSWER_MESSAGE_SIZE, n->task_name);
		MSG_task_send(fail, n->client);
		complexity += 6.0;
	}
	// in case of Sonnek what ever happen, when all the workers have answer the task is processed (because there isn't any addtional replication strategy)
	*process = 1;
	complexity++;

	msg_task_t task_complexity = MSG_task_create("task_complexity", complexity, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
	// update the reputation of the workers and suppress the processing task
	//updateReputation(n, id);
	//printf("in primary-%d, list of workers\n", id);
	//workers_print(&(workers[id]));
	//printf("acitve groups for primary-%d\n", id);
	//groups_print(&(active_groups[id]));
}


char replication(struct p_task * n, int id) {
	if (n->nb_replication == REPLICATION_MAX) {
		msg_task_t task_complexity = MSG_task_create("task_complexity", 1.0, 0, NULL);
		MSG_task_execute(task_complexity);
		MSG_task_destroy(task_complexity);
		
		return 1;
	}
	n->nb_replication++;
	n->additional_workers = xbt_dynar_new((sizeof(char) * MAILBOX_SIZE), NULL);
	n->additional_reputations = xbt_dynar_new(sizeof(int), NULL);

	if (group_formation_strategy == FIXED_FIT) {
		printf("replication fixed_fit\n");
		replication_fixed_fit(n, id);
	}
	else {
		replication_others_fit(n, id);
	}

	msg_task_t task_complexity = MSG_task_create("task_complexity", 2.0, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
	return -1;
}


void send_answer_Arantes(struct p_task * n, int nb_majoritary_answer, char * process, char * stop_replication, double max_value, int id) {
	double complexity = 0.0;

	if (group_formation_strategy == FIXED_FIT) {
		complexity++;
		if (additional_replication_strategy == ITERATIVE_REDUNDANCY) {
			printf("value of nb_majoritary_answer=%d value of res =%lu, n->nb_forwarded=%d, n->res->worker_names = %ld\n", nb_majoritary_answer, n->res->answer, n->nb_forwarded, xbt_dynar_length(n->res->worker_names));  
			if ((nb_majoritary_answer == 1) && ((xbt_dynar_length(n->res->worker_names) - (n->nb_forwarded - xbt_dynar_length(n->res->worker_names))) >= additional_replication_value_difference)) {
				n->final_answer = n->res->answer;
				if (nb_workers_for_stationary >= stationary_regime) {
					writes_data(n->client, n->task_name, n->start, -1, n->res->answer, n->nb_forwarded, n->targetLOC, id);//xbt_dynar_length(n->res->worker_names), id);
				}			
				printf("final answer = %ld\n", n->res->answer);
				// send the answer to the client
				msg_task_t answer_client = MSG_task_create("answer", ANSWER_COMPUTE_DURATION, ANSWER_MESSAGE_SIZE, n->task_name);
				MSG_comm_destroy(MSG_task_isend(answer_client, n->client));

				*process = 1;
				complexity += 2.0;
				//updateReputation(n, id);
			}
			else {
				printf("additional replication\n");
				// replication
				*stop_replication = replication(n, id);
				printf("after additional replication\n");
			}
			complexity += 7.0;
		}
		else {
			// additionnal_replication_strategy == PROGRESSIVE_REDUNDANCY
			if ((nb_majoritary_answer == 1) && (xbt_dynar_length(n->res->worker_names) >= (floor((double)group_formation_fixed_number/2.0) + 1))) {
				n->final_answer = n->res->answer;
				if (nb_workers_for_stationary >= stationary_regime) {
					writes_data(n->client, n->task_name, n->start, -1, n->res->answer, n->nb_forwarded, n->targetLOC, id);//xbt_dynar_length(n->res->worker_names), id);
				}				
				// send the answer to the client
				msg_task_t answer_client = MSG_task_create("answer", ANSWER_COMPUTE_DURATION, ANSWER_MESSAGE_SIZE, n->task_name);
				MSG_comm_destroy(MSG_task_isend(answer_client, n->client));

				*process = 1;
				complexity += 2.0;
				//updateReputation(n, id);
			}
			else {
				*stop_replication = replication(n, id);
			}
			complexity += 7.0;
		}
		complexity++;
	}
	else if (((nb_majoritary_answer == 1) && (xbt_dynar_length(n->res->worker_names) == n->nb_forwarded)) || ((max_value > (1.0 - n->targetLOC)) && (xbt_dynar_length(n->res->worker_names) > 1))) {
		// there is no ambiguity on the final result
		n->final_answer = n->res->answer;
		if (nb_workers_for_stationary >= stationary_regime) {
			writes_data(n->client, n->task_name, n->start, -1, n->res->answer, n->nb_forwarded, n->targetLOC, id);//xbt_dynar_length(n->res->worker_names), id);
		}
		printf("final answer = %ld\n", n->res->answer);
		// send the answer to the client
		msg_task_t answer_client = MSG_task_create("answer", ANSWER_COMPUTE_DURATION, ANSWER_MESSAGE_SIZE, n->task_name);
		MSG_comm_destroy(MSG_task_isend(answer_client, n->client));

		*process = 1;
		complexity += 10.0;
		//updateReputation(n, id);
	}
	else {
		// replication of the task on additional nodes
		unsigned int nb;
		char name[256];
		xbt_dynar_foreach(n->res->worker_names, nb, name) {
			printf("value of the name that return the good answer %s\n", name);
		}
		complexity += 8.0;
		*stop_replication = replication(n, id);
	}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (*stop_replication == 1) {
		if (nb_majoritary_answer == 1) {
			if (nb_workers_for_stationary >= stationary_regime) {
				writes_data(n->client, n->task_name, n->start, -1, n->res->answer, n->nb_forwarded, n->targetLOC, id);//xbt_dynar_length(n->res->worker_names), id);
			}			
			printf("final answer = %ld\n", n->res->answer);
			// send the answer to the client
			msg_task_t answer_client = MSG_task_create("answer", ANSWER_COMPUTE_DURATION, ANSWER_MESSAGE_SIZE, n->task_name);
			MSG_comm_destroy(MSG_task_isend(answer_client, n->client));
		}
		else {
			n->final_answer = n->res->answer;
			if (nb_workers_for_stationary >= stationary_regime) {
				writes_data(n->client, n->task_name, n->start, 0, n->res->answer, n->nb_forwarded, n->targetLOC, id);//xbt_dynar_length(n->res->worker_names), id);
			}	
			printf("primary-%d: send fail to client %s for %s\n", id, n->client, n->task_name);
			msg_task_t fail = MSG_task_create("fail", ANSWER_COMPUTE_DURATION, ANSWER_MESSAGE_SIZE, n->task_name);
			MSG_task_send(fail, n->client);

			complexity += 1.0;

		}
		complexity += 1.0;
	}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	
	msg_task_t task_complexity = MSG_task_create("task_complexity", complexity + 1.0, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
}


int inAdditional_replication_tasks (struct p_task * p_t, int id) {
	xbt_fifo_item_t i;
	struct p_task * n;
	double complexity = 0.0;

   for(i = xbt_fifo_get_first_item(additional_replication_tasks[id]); ((i) ? (n = (struct p_task *)(xbt_fifo_get_item_content(i))) : (NULL)); i = xbt_fifo_get_next_item(i)) {
		complexity++;
		if (n == p_t) {
			msg_task_t task_complexity = MSG_task_create("task_complexity", complexity + 4.0, 0, NULL);
			MSG_task_execute(task_complexity);
			MSG_task_destroy(task_complexity);
			return 1;
		}
	}

	msg_task_t task_complexity = MSG_task_create("task_complexity", complexity + 4.0, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
	return -1;
}


void suppress_processing_tasks_and_active_group(struct p_task * n, int id) {
	printf("primary-%d: suppress the active group and processing tasks for %s %s\n", id, n->client, n->task_name);
	// suppress the group in active groups
	//if (xbt_fifo_remove(active_groups, *(n->active_workers)) != 1) {
	if (xbt_fifo_remove(active_groups[id], xbt_fifo_get_item_content(n->active_workers)) != 1) {
		printf("problem detected when removing a xbt_dynar_t from active_groups\n");
	}
	// suppress the task (because it has been accomplished) from the processing_tasks
	if(xbt_fifo_remove(processing_tasks[id], n) != 1) {
		printf("problem detected when removing a struct p_task from processing_tasks\n");
	}

	msg_task_t task_complexity= MSG_task_create("task_complexity", 5.0, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
}


void worker_from_active_group_to_workers(char * name, struct p_task * n, int process, int stop_replication, int id) {
	double complexity = 4.0;

	printf("primary-%d: suppression of the %s of the active groups -> put it in workers\n", id, name);
	// move the worker from the active groups from the workers
	xbt_fifo_item_t j;
	xbt_dynar_t * d;
	char found = -1;
	struct p_worker * toAddToWorkers = (struct p_worker *) malloc(sizeof(struct p_worker));

	for(j = xbt_fifo_get_first_item(active_groups[id]); ((j) ? (d = (xbt_dynar_t *)(xbt_fifo_get_item_content(j))) : (NULL)); j = xbt_fifo_get_next_item(j)) {

		struct p_worker p_w;
		unsigned int cpt;

		xbt_dynar_foreach (*d, cpt, p_w) {
	 		if (!strcmp(p_w.mailbox, name)) {
				found = 1;
				break;
			}
		}
		complexity = complexity + cpt + 1.0;

		if (found == 1) {
			xbt_dynar_remove_at(*d, cpt, toAddToWorkers);
			complexity++;
			break;
		}
	}

	complexity += 2.0;
	xbt_dynar_push(workers[id], toAddToWorkers);

	// we check if the name of the worker is in the additional_workers and if yes, we remove it from that array and we remove its reputation from the additional_reputation array too
	if (n->additional_workers != NULL) {
		char worker_name[MAILBOX_SIZE];
		unsigned int nb;
		found = -1;

		xbt_dynar_foreach(n->additional_workers, nb, worker_name) {
			if (!strcmp(worker_name, name)) {
				found = 1;
				break;
			}
		}
		complexity = complexity + nb + 1.0;
		if (found == 1) {
			xbt_dynar_remove_at(n->additional_workers, nb, NULL);
			xbt_dynar_remove_at(n->additional_reputations, nb, NULL);
			complexity += 2.0;
		}
	}

	if ((process == 1) || (stop_replication == 1)) {
		updateReputation(n, id);
		//suppress_processing_tasks_and_active_group(d, n);
		suppress_processing_tasks_and_active_group(n, id);
	}


	msg_task_t task_complexity = MSG_task_create("task_complexity", complexity, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
	if ((centrality == DISTRIBUTED) && (distributed_strategies == REPUTATIONS)) {
		send_change(id);
	}

	workers_print(&workers[id]);
	groups_print(&active_groups[id]);
}


void worker_from_active_group_to_suppression(char * name, struct p_task * n, int process, int stop_replication, int id) {
	printf("primary-%d: suppression of the %s of the active groups -> suppression\n", id, name);
	// remove the worker from the system since it has crashed
	xbt_fifo_item_t j;
	xbt_dynar_t * d;
	struct p_worker * toSuppress = NULL;
	double complexity = 0.0;
	char found = -1;

	for(j = xbt_fifo_get_first_item(active_groups[id]); ((j) ? (d = (xbt_dynar_t *)(xbt_fifo_get_item_content(j))) : (NULL)); j = xbt_fifo_get_next_item(j)) {

		struct p_worker p_w;
		unsigned int cpt;

		xbt_dynar_foreach (*d, cpt, p_w) {
	 		if (!strcmp(p_w.mailbox, name)) {
				found = 1;
				break;
			}
		}
		
		if (found == 1) {
			xbt_dynar_remove_at(*d, cpt, toSuppress);
			free(toSuppress);
		}
		complexity = complexity + cpt + 1.0;
		

		break;
	}

	if ((process == 1) || (stop_replication == 1)) {
		updateReputation(n, id);
		//suppress_processing_tasks_and_active_group(d, n);
		suppress_processing_tasks_and_active_group(n, id);
	}

	complexity += 6.0;

	msg_task_t task_complexity = MSG_task_create("task_complexity", complexity, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
	workers_print(&workers[id]);
	groups_print(&active_groups[id]);
}


void treat_answer(msg_task_t t, int crash, int id) {
	// update the answer of the worker
	// check if all workers give the answer: if yes, look up for the majority solution
	// if all workers answer the same way, give the answer to the client and update the reputation, and put all the workers on the workers dynamic array
	// if not ask the requests to additional nodes (that the primary put in the same active group than the previous answered nodes
	struct w_task * w_t = (struct w_task *) malloc(sizeof(struct w_task));
	xbt_fifo_item_t i;
	struct p_task * n;
	char process = -1;
	char stop_replication = -1;
	int nb_majoritary_answer = 0;
	double max_value = 0.0;

	w_t = (struct w_task *)MSG_task_get_data(t);
	if (crash == -1) {
		printf("primary-%d received an answer from %s to %s %s value=%ld\n", id, w_t->worker_name, w_t->client, w_t->task_name, w_t->answer);
	}
	
	double complexity = 0.0;

	// the primary have to find the element of processing_tasks that correspond to the answer
   for(i = xbt_fifo_get_first_item(processing_tasks[id]); ((i) ? (n = (struct p_task *)(xbt_fifo_get_item_content(i))) : (NULL)); i = xbt_fifo_get_next_item(i)) {
		complexity += 4.0;
		if ((strcmp(n->client, w_t->client) == 0) && (strcmp(n->task_name, w_t->task_name) == 0)) {
			// add the worker to w_answers
			add_answers(n, &(n->w_answers), w_t->worker_name, w_t->answer, id);

			complexity++;
			if (crash == 1) {
				complexity++;
				n->nb_crashed++;
				n->nb_false_answers++;
			}
			else {
				n->nb_answers_received++;
				if (w_t->answer > GOOD_ANSWER) {
					n->nb_false_answers++;
				}
				else {
					n->nb_correct_answers++;
				}
				complexity += 3.0;
			}
			if (((n->nb_answers_received + n->nb_crashed) == n->nb_forwarded) && (inAdditional_replication_tasks(n, id) == -1)) {
				n->to_replicate = 0;
				if (n->additional_workers != NULL) {
					xbt_dynar_free(&(n->additional_workers));
					xbt_dynar_free(&(n->additional_reputations));
				}
				//n->additional_workers = xbt_dynar_new(sizeof(char) * MAILBOX_SIZE, NULL);
				//xbt_dynar_free(&(n->additional_reputations));
				//n->additional_reputations = xbt_dynar_new(sizeof(int), NULL);

				compute_majoritary_answer(n, &nb_majoritary_answer, &max_value); 	

				printf("receive all the answers for %s %s\n", n->client, n->task_name);
				if (simulator == SONNEK) {
					send_answer_Sonnek(n, nb_majoritary_answer, &process, id);
				}
				else {
					// we simulate the algorithm used in ARANTES
					send_answer_Arantes(n, nb_majoritary_answer, &process, &stop_replication, max_value, id);
				}
				//free(n->w_answers);
				complexity += 6.0;
			}
			complexity += 3.0;
			break;
		}
	}
	complexity += 5.0;
	msg_task_t task_complexity = MSG_task_create("task_complexity", complexity, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
	
	if (crash == 1) {
		worker_from_active_group_to_suppression(w_t->worker_name, n, process, stop_replication, id);
	}
	else {
		worker_from_active_group_to_workers(w_t->worker_name, n, process, stop_replication, id);
	}
	//free(w_t);
}


void try_to_treat_additional_replication(int id) {
	int i;
	int size = xbt_fifo_size(additional_replication_tasks[id]);
	printf("start of try_to_treat_additional_replication\n");
	double complexity = 2.0;

	for (i = 0; i < size; i++) {
		complexity += 2.0;
		if (xbt_dynar_length(workers[id]) >= 0) {
			complexity++;
			if (group_formation_strategy == FIXED_FIT) {
				replication_fixed_fit((struct p_task *)fifo_supress_head(additional_replication_tasks[id]), id);
			}
			else {
				replication_others_fit((struct p_task *)fifo_supress_head(additional_replication_tasks[id]), id);
			}
		}
		else {
			break;
		}
	}

	msg_task_t task_complexity = MSG_task_create("task_complexity", complexity, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
}


void compute_name_file (int id) {
	char * ret = (char *) malloc(sizeof(char) * FILE_NAME_SIZE);
	char nb[3];

	if (centrality == CENTRALIZED) {
		sprintf(ret, "../files_res/centralized_primary_%d_res_%dworkers_%dclients", id, nb_workers, nb_clients);
	}
	else {
		if (blacklist == BLACKLIST) {
			if (distributed_strategies == RANDOM) {
				sprintf(ret, "../files_res/distributed_blacklist_random_primary_%d_res_%dworkers_%dclients", id, nb_workers, nb_clients);
			}
			else {
				sprintf(ret, "../files_res/distributed_blacklist_reputations_primary_%d_res_%dworkers_%dclients", id, nb_workers, nb_clients);
			}
		}
		else {
			if (distributed_strategies == RANDOM) {
				sprintf(ret, "../files_res/distributed_no_blacklist_random_primary_%d_res_%dworkers_%dclients", id, nb_workers, nb_clients);
			}
			else {
				sprintf(ret, "../files_res/distributed_no_blacklist_reputations_primary_%d_res_%dworkers_%dclients", id, nb_workers, nb_clients);
			}
		}
	}

	if (random_target_LOC == RANDOM) {
		strcat(ret, "_randomTargetLOC");
	}
	else {
		strcat(ret, "_notRandomTargetLOC");
	}


	if (simulator == ARANTES) {
		strcat(ret, "_arantes");
	}
	else {
		strcat(ret, "_sonnek");
	}

	if (reputation_strategy == SYMMETRICAL) {
		strcat(ret, "_symmetrical");
	}
	else if (reputation_strategy == ASYMMETRICAL) {
		strcat(ret, "_asymmetrical");
	}
	else if (reputation_strategy == BOINC) {
		strcat(ret, "_boinc");
	}
	else {
		strcat(ret, "_sonnek_reputation");
	}
	
	if (group_formation_strategy == FIXED_FIT) {
		strcat(ret, "_");
		sprintf(nb, "%d", group_formation_fixed_number);
		strcat(ret, nb);
		strcat(ret, "fixed_fit");	
	}
	else if (group_formation_strategy == FIRST_FIT) {
		strcat(ret, "_first_fit");
	}
	else if (group_formation_strategy == TIGHT_FIT) {
		strcat(ret, "_tight_fit");
	}
	else {
		strcat(ret, "_random_fit");
	}

	if (simulator == ARANTES) {
		if (additional_replication_strategy == PROGRESSIVE_REDUNDANCY) {
			strcat(ret, "_progressive_redundancy");
		}
		else if (additional_replication_strategy == ITERATIVE_REDUNDANCY) {
			strcat(ret, "_iterative_redundancy");
		}
		else {
			strcat(ret, "_arantes_replication");
		}
	}

	strcat(ret, ".csv");

	if ((data_csv[id] = open(ret, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR)) == -1) {
		printf("error open\n");
		exit(1);
	}

	free(ret);
}


void send_change(int id) {
	unsigned int cpt;
	struct p_worker p_w;
	size_t size;
	double complexity = 0.0;
	
	xbt_dynar_t * toSend = (xbt_dynar_t *) malloc(sizeof(xbt_dynar_t));
	*toSend = xbt_dynar_new(sizeof(struct p_worker), NULL); 

	printf("in send_change\n");
	if (xbt_dynar_length(to_change_primary[id]) == 0) {
		msg_task_t task_complexity = MSG_task_create("task_complexity", 2.0, 0, NULL);
		MSG_task_execute(task_complexity);
		MSG_task_destroy(task_complexity);
		return;
	}

	complexity += 2.0;

	printf("primary-%d: list of workers\n", id);
	//workers_print(&(workers[id]));
	printf("primary-%d: I have some nodes to give to others primaries\n", id);

	xbt_dynar_foreach(to_change_primary[id], cpt, p_w) {
		printf("%s is a worker that must change its primary, we are sending it to the first-primary\n", p_w.mailbox);
		size = size + strlen(p_w.mailbox);
		xbt_dynar_push(*toSend, xbt_dynar_get_ptr(to_change_primary[id], cpt));
	}

	complexity = complexity + cpt;
 
	printf("sending the change message to the first-primary\n");
	MSG_comm_destroy(MSG_task_isend(MSG_task_create("change", 0, sizeof(char) * (strlen("change") + size), toSend), first_primary_name));//&to_change_primary[id]), first_primary_name); 
	unsigned int cpt1;
	struct p_worker p_w1;
	//char found = 0;


	printf("size of to_cahnge_primary[id] %ld\n", xbt_dynar_length(to_change_primary[id]));
	xbt_dynar_foreach(to_change_primary[id], cpt1, p_w1) {
		unsigned int nb;
		struct p_worker worker;
		printf("searching for worker %s\n", p_w1.mailbox);

		xbt_dynar_foreach(workers[id], nb, worker) {
			if (!strcmp(worker.mailbox, p_w1.mailbox)) {
			//	found = 1;
				printf("found %s\n", worker.mailbox);
				break;
			}
		}
	/*	if (found == 1) {
			found = 0;*/
			printf("here value of nb %d\n", nb);
			xbt_dynar_remove_at(workers[id], nb, NULL);
			printf("after here\n");
			complexity = complexity + nb + 1.0;
	//	}
	}
	xbt_dynar_reset(to_change_primary[id]); 
	printf("end of change\n");

	msg_task_t task_complexity = MSG_task_create("task_complexity", complexity + 1.0, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
}


void try_load_balancing_random_overload (int id, char * mailbox) {
	printf("enter try_load_balancing_random_overload\n");
	MSG_comm_destroy(MSG_task_isend(MSG_task_create("division", 0, strlen("division") * sizeof(char), mailbox), first_primary_name));

	int nb;	
	int i;
	double complexity = 6.0;			
	
	if (xbt_dynar_length(workers[id]) >= ((xbt_dynar_length(workers[id]) + xbt_fifo_size(active_groups[id])) / 2)) {	
		nb = ((xbt_dynar_length(workers[id]) + xbt_fifo_size(active_groups[id])) / 2);
		complexity += 5.0;
	}
	else {
		nb = ((floor(number_workers_too_high / 2)) + 1);
		complexity += 4.0;
	}
	printf("value of nb = %d, size of workers[id] = %ld\n", nb, xbt_dynar_length(workers[id]));

	for (i = 0; i < nb; i++) {
		struct p_worker * p = (struct p_worker *) malloc(sizeof(struct p_worker));
		xbt_dynar_remove_at(workers[id], i, p);
		xbt_dynar_push(toSend_loadBalancing[id]->workersToSend, p);
		printf("new worker added to send %s\n", p->mailbox);
	}	

	msg_task_t task_complexity = MSG_task_create("task_complexity", complexity + nb * 2.0, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
}


// the primary keep the workers with the highest reputation and give those with the lowest
void try_load_balancing_reputation_overload (int id, char * mailbox) {
	printf("enter try_load_balancing_reputation_overload\n");
	xbt_dynar_sort(workers[id], compare_reputation_workers);//compare_reputation_workers_inverse);
	struct reputations_loadBalancing * toSend = (struct reputations_loadBalancing *) malloc(sizeof(struct reputations_loadBalancing));

	//unsigned int cpt;
	struct p_worker * worker;
	int nb;	
	int i;
	double complexity = 6.0;			
	
	if (xbt_dynar_length(workers[id]) >= ((xbt_dynar_length(workers[id]) + xbt_fifo_size(active_groups[id])) / 2)) {	
		nb = ((xbt_dynar_length(workers[id]) + xbt_fifo_size(active_groups[id])) / 2);
		complexity += 5.0;
	}
	else {
		nb = ((floor(number_workers_too_high / 2)) + 1);
		complexity += 4.0;
	}

	/*xbt_dynar_foreach(workers[id], cpt, worker) {
		if (cpt == (nb - 1)) {
			toSend->max_reputation = worker.reputation;
			reputations_primary[id].value = worker.reputation;
			complexity += 2.0;
			break;
		}
	}*/
/////////////////////////////////////////////////////////////////////////
	worker = xbt_dynar_get_ptr(workers[id], xbt_dynar_length(workers[id]) - nb);
	toSend->max_reputation = worker->reputation;
	reputations_primary[id].value = worker->reputation;
/////////////////////////////////////////////////////////////////////////




	toSend->min_reputation = reputations_primary[id].min_reputation;				
	strcpy(toSend->mailbox, mailbox);

	complexity = complexity + 5.0 + 2.0;

	MSG_comm_destroy(MSG_task_isend(MSG_task_create("division", 0, strlen("division")*sizeof(char), toSend), first_primary_name));

	for (i = 0; i < nb; i++) {
		struct p_worker * p = (struct p_worker *) malloc(sizeof(struct p_worker));
		xbt_dynar_remove_at(workers[id], xbt_dynar_length(workers[id]) - 1, p);
		xbt_dynar_push(toSend_loadBalancing[id]->workersToSend, p);
	}

	msg_task_t task_complexity = MSG_task_create("task_complexity", complexity + nb * 2.0, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
}


void try_load_balancing_overload(int id, char * mailbox) {
	double complexity = 0.0;

	if (able_to_send_division[id] == 1) {
		if (((xbt_dynar_length(workers[id]) + xbt_fifo_size(active_groups[id])) >= number_workers_too_high) && (xbt_dynar_length(workers[id]) >= (floor((number_workers_too_high / 2)) + 1))) {
			if (distributed_strategies == RANDOM) {				
				 try_load_balancing_random_overload(id, mailbox);
			}
			else {
				try_load_balancing_reputation_overload(id, mailbox);
			}
			complexity++;
		}
		complexity += 8.0;
	}

	msg_task_t task_complexity = MSG_task_create("task_complexity", complexity + 1.0, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
}


void stop_division (int id) {
	int i;
	struct p_worker * p = (struct p_worker *) malloc(sizeof(struct p_worker));
	
	able_to_send_division[id] = -1;
	int until = xbt_dynar_length(toSend_loadBalancing[id]->workersToSend);

	for (i = 0; i < until; i++) {
		xbt_dynar_pop(toSend_loadBalancing[id]->workersToSend, p);
		xbt_dynar_push(toSend_loadBalancing[id]->workersToSend, p);
	}

	msg_task_t task_complexity = MSG_task_create("task_complexity", until * 2.0 + 2.0, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
}


void stop_fusion(int id) {
	able_to_send_fusion[id] = -1;
	doing_fusion[id] = -1;
	
	msg_task_t task_complexity = MSG_task_create("task_complexity", 2.0, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
}


void treat_division_overload(msg_task_t task, int id) {
	double complexity = 0.0;
	struct loadBalancing * lb = (struct loadBalancing *) malloc(sizeof(struct loadBalancing));
	lb->workersToSend =  xbt_dynar_new(sizeof(struct p_worker), NULL);

	if (distributed_strategies == REPUTATIONS) {
		toSend_loadBalancing[id]->min_reputation = reputations_primary[id].min_reputation;
		toSend_loadBalancing[id]->value = reputations_primary[id].value;
		reputations_primary[id].min_reputation = reputations_primary[id].value;
		complexity += 3.0;
	}
	MSG_comm_destroy(MSG_task_isend(MSG_task_create("give_workers", 0, sizeof(toSend_loadBalancing[id]), toSend_loadBalancing[id]), (char *) MSG_task_get_data(task))); 

	toSend_loadBalancing[id] = lb;

	msg_task_t task_complexity = MSG_task_create("task_complexity", complexity + 3.0, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
}


void treat_give_workers(msg_task_t task, int id, char * mailbox) {
	struct loadBalancing * lb;
	lb = (struct loadBalancing *)MSG_task_get_data(task);
	int i;
	int until = xbt_dynar_length(lb->workersToSend);

	reputations_primary[id].max_reputation = (char)lb->value;
	reputations_primary[id].min_reputation = (char)lb->min_reputation;
	
	for (i = 0; i < until; i++) {
		struct p_worker * worker = (struct p_worker *) malloc(sizeof(struct p_worker));
		xbt_dynar_pop(lb->workersToSend, worker);
		xbt_dynar_push(workers[id], worker);
		printf("%s: I receive the worker %s with reputations %d\n", mailbox, worker->mailbox, worker->reputation);
		MSG_comm_destroy(MSG_task_isend(MSG_task_create("ack", 0, (strlen("ack") + strlen(mailbox)) * sizeof(char), mailbox), worker->mailbox));
	}

	msg_task_t task_complexity = MSG_task_create("task_complexity", until * 2.0 + 4.0, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
}


void forward_to_first_primary(msg_task_t task) {
	MSG_comm_destroy(MSG_task_isend(MSG_task_create(MSG_task_get_name(task), 0, MSG_task_get_data_size(task), MSG_task_get_data(task)), first_primary_name)); 

	msg_task_t task_complexity = MSG_task_create("task_complexity", 3.0, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
}


void execute_fusion (int id) {
	struct fusion * toSend = (struct fusion *) malloc(sizeof(struct fusion));
	toSend->workersToSend = xbt_dynar_new(sizeof(struct p_worker), NULL);
	int until = xbt_dynar_length(workers[id]);
	double complexity = 1.0;

	int i;
	for (i = 0; i < until; i++) {
		struct p_worker * worker = (struct p_worker *) malloc(sizeof(struct p_worker));
		xbt_dynar_pop(workers[id], worker);
		xbt_dynar_push(toSend->workersToSend, worker);
	}
	if (distributed_strategies == REPUTATIONS) {
		toSend->min = reputations_primary[id].min_reputation;
		toSend->max = reputations_primary[id].max_reputation;
		complexity += 2.0;
	}
	MSG_comm_destroy(MSG_task_isend(MSG_task_create("workers_to_fuse", 0, strlen("workers_to_fuse") * sizeof(char) + sizeof(toSend), toSend), first_primary_name));

	complexity = complexity + until * 2.0 + 1.0;

	msg_task_t task_complexity = MSG_task_create("task_complexity", complexity, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
}


void update_limit_max (msg_task_t task, int id) {
	char * new_limit = (char *)MSG_task_get_data(task);

	reputations_primary[id].max_reputation = *new_limit;

	msg_task_t task_complexity = MSG_task_create("task_complexity", 2.0, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
}


void update_limit_min(msg_task_t task, int id) {
	char * new_limit = (char *)MSG_task_get_data(task);

	reputations_primary[id].min_reputation = *new_limit;

	msg_task_t task_complexity = MSG_task_create("task_complexity", 2.0, 0, NULL);
	MSG_task_execute(task_complexity);
	MSG_task_destroy(task_complexity);
}


void destroy_content_fifo(xbt_fifo_t * f) {
	int i;
	int until = xbt_fifo_size(*f);

   for(i = 0; i < until; i++) {
		xbt_fifo_free_item(xbt_fifo_get_first_item(*f));
	}
}


int primary (int argc, char * argv[]) {
	printf("primary start\n");
	unsigned long int id;
	char myMailbox[MAILBOX_SIZE];
	msg_task_t task_todo = NULL;
	//int nb_clients;
	int nb_finalize = 0;
	msg_task_t task_complexity;

	if (argc != 3) {
		printf("here in the primary\n");
		exit(1);
	}

	id = atoi(argv[1]);

	/*if (centrality == CENTRALIZED) {
		nb_clients = atoi(argv[2]);
	}*/
	if (centrality == DISTRIBUTED) {
		if (id == 0) {
			strcpy(first_primary_name, argv[2]);
			reputations_primary[id].min_reputation = 0;
			reputations_primary[id].max_reputation = 101;
		}
	}

	srand(time(NULL) * id + MSG_get_clock());
	
	sprintf(myMailbox, "primary-%ld", id);

	compute_name_file(id);


	workers[id] = xbt_dynar_new(sizeof(struct p_worker), NULL);
	tasks[id] = xbt_fifo_new();
	processing_tasks[id] = xbt_fifo_new();
	active_groups[id] = xbt_fifo_new();
	additional_replication_tasks[id] = xbt_fifo_new();
	to_change_primary[id] = xbt_dynar_new(sizeof(struct p_worker), NULL);
	toSend_loadBalancing[id]->workersToSend = xbt_dynar_new(sizeof(struct p_worker), NULL);


	while (1) {
		// reception of a message
		MSG_task_receive(&(task_todo), myMailbox);

		/* the primary can receive 4 types of messages: requests from client, 
			finalization from client, join from workers, or answer to tasks of workers */
		if (!strcmp(MSG_task_get_name(task_todo), "finalize")) {
			printf("%s: I receive finalize\n", myMailbox);
			if (centrality == CENTRALIZED) {
				nb_finalize++;
				if (nb_finalize == nb_clients) {		
					// if all clients have finish to send requests, the primary ask the workers to stop
					send_finalize_to_workers(id); 
					break;
				}
			}
			else {	
				// if all clients have finish to send requests, the primary ask the workers to stop
				send_finalize_to_workers(id); 
				break;
			}
		}
		else if (!strncmp(MSG_task_get_name(task_todo), "task", strlen("task"))) {
			printf("%s: I receive a task\n", myMailbox);
			if (centrality == CENTRALIZED) {
				put_task_fifo(task_todo, id);
				try_to_treat_tasks(myMailbox, id);
			}
			else {
				if (doing_fusion[id] == -1) {
					// the primary put the task to do in a fifo
					put_task_fifo(task_todo, id);
					if ((centrality == DISTRIBUTED) && (distributed_strategies == REPUTATIONS)) {
						send_change(id);
					}
					try_to_treat_tasks(myMailbox, id);
				}
				else {
					forward_to_first_primary(task_todo);
				}
			}
		}
		else if (!strncmp(MSG_task_get_name(task_todo), "join", sizeof(char) * strlen("join"))) {
			printf("%s: I receive a join\n", myMailbox);
			if (centrality == CENTRALIZED) {
				add_new_worker((char *)MSG_task_get_data(task_todo), myMailbox, id);
				if (simulator == ARANTES) {
					try_to_treat_additional_replication(id);
				}
				try_to_treat_tasks(myMailbox, id);
			}
			else {
				if (doing_fusion[id] == -1) {
					// a worker want to join the system			
					add_new_worker((char *)MSG_task_get_data(task_todo), myMailbox, id);
					if ((centrality == DISTRIBUTED) && (distributed_strategies == REPUTATIONS)) {
						send_change(id);
					}
					try_load_balancing_overload(id, myMailbox);
					if (simulator == ARANTES) {
						try_to_treat_additional_replication(id);
					}
					try_to_treat_tasks(myMailbox, id);
				}
				else {
					forward_to_first_primary(task_todo);
				}
			}
		}
		else if (!strncmp(MSG_task_get_name(task_todo), "change", sizeof(char) * strlen("change"))) {
			printf("%s: I receive a change\n", myMailbox);
			if (doing_fusion[id] == -1) {
				// a worker want to join the system			
				add_new_worker_change(task_todo, myMailbox, id);
				if ((centrality == DISTRIBUTED) && (distributed_strategies == REPUTATIONS)) {
					send_change(id);
				}
				try_load_balancing_overload(id, myMailbox);
				if (simulator == ARANTES) {
					try_to_treat_additional_replication(id);
				}
				try_to_treat_tasks(myMailbox, id);
			}
			else {
				forward_to_first_primary(task_todo);
			}
		}
		else if (!strncmp(MSG_task_get_name(task_todo), "answer", sizeof(char) * strlen("answer"))) {
			printf("%s: I receive an answer\n", myMailbox);
			// the primary receive an answer to a request from a worker
			if (centrality == CENTRALIZED) {
				treat_answer(task_todo, -1, id);
				if (simulator == ARANTES) {
					try_to_treat_additional_replication(id);
				}
				try_to_treat_tasks(myMailbox, id);
			}
			else {
				treat_answer(task_todo, -1, id);
				if ((centrality == DISTRIBUTED) && (distributed_strategies == REPUTATIONS)) {
					send_change(id);
				}		
				try_load_balancing_overload(id, myMailbox);	
				if (simulator == ARANTES) {
					try_to_treat_additional_replication(id);
				}
				try_to_treat_tasks(myMailbox, id);
			}
		}
		else if (!strncmp(MSG_task_get_name(task_todo), "crash", sizeof(char) * strlen("crash"))) {
			printf("%s: I receive a message indicating the crash of %s\n", myMailbox, (char *) MSG_task_get_data(task_todo));
			treat_answer(task_todo, 1, id);
		}
		else if (!strncmp(MSG_task_get_name(task_todo), "able_fusion", sizeof(char) * strlen("able_fusion"))) {
			printf("%s: I receive a message able_fusion\n", myMailbox);
			able_to_send_fusion[id] = 1;

			task_complexity = MSG_task_create("task_complexity", 1.0, 0, NULL);
			MSG_task_execute(task_complexity);
			MSG_task_destroy(task_complexity);
			task_complexity = NULL;
		}
		else if (!strncmp(MSG_task_get_name(task_todo), "able_division", sizeof(char) * strlen("able_division"))) {
			printf("%s: I receive a message able_division\n", myMailbox);
			able_to_send_division[id] = 1;

			task_complexity = MSG_task_create("task_complexity", 1.0, 0, NULL);
			MSG_task_execute(task_complexity);
			MSG_task_destroy(task_complexity);
			task_complexity = NULL;
		}
		else if (!strncmp(MSG_task_get_name(task_todo), "unable_fusion", sizeof(char) * strlen("unable_fusion"))) {
			printf("%s: I receive a message unable_fusion\n", myMailbox);
			able_to_send_fusion[id] = -1;
			
			task_complexity = MSG_task_create("task_complexity", 1.0, 0, NULL);
			MSG_task_execute(task_complexity);
			MSG_task_destroy(task_complexity);
			task_complexity = NULL;
		}
		else if (!strncmp(MSG_task_get_name(task_todo), "unable_division", sizeof(char) * strlen("unable_division"))) {
			printf("%s: I receive a message unable_division\n", myMailbox);
			able_to_send_division[id] = -1;
			
			task_complexity = MSG_task_create("task_complexity", 1.0, 0, NULL);
			MSG_task_execute(task_complexity);
			MSG_task_destroy(task_complexity);
			task_complexity = NULL;
		}		
		else if (!strncmp(MSG_task_get_name(task_todo), "unack_division", sizeof(char) * strlen("unack_division"))) {
			printf("%s: I receive a message unack_division\n", myMailbox);
			stop_division(id);
		}
		else if (!strncmp(MSG_task_get_name(task_todo), "unack_fusion", sizeof(char) * strlen("unack_fusion"))) {
			printf("%s: I receive a message unack_fusion\n", myMailbox);
			stop_fusion(id);
		}
		else if (!strncmp(MSG_task_get_name(task_todo), "ack_fusion", sizeof(char) * strlen("ack_fusion"))) {
			printf("%s: I receive a message ack_fusion\n", myMailbox);
			execute_fusion(id);
		}
		else if (!strncmp(MSG_task_get_name(task_todo), "ack_division", sizeof(char) * strlen("ack_division"))) {
			printf("%s: I receive a message ack_division\n", myMailbox);
			treat_division_overload(task_todo, id);
		}
		else if (!strncmp(MSG_task_get_name(task_todo), "give_workers", sizeof(char) * strlen("give_workers"))) {
			printf("%s: I receive a message give_workers\n", myMailbox);
			treat_give_workers(task_todo, id, myMailbox);
		}	
		else if (!strncmp(MSG_task_get_name(task_todo), "limit_max", sizeof(char) * strlen("limit_max"))) {
			printf("%s: I receive a message limit_max\n", myMailbox);
			update_limit_max(task_todo, id);
		}
		else if (!strncmp(MSG_task_get_name(task_todo), "limit_min", sizeof(char) * strlen("limit_min"))) {
			printf("%s: I receive a message limit_min\n", myMailbox);
			update_limit_min(task_todo, id);
		}	
		else {
			// incorrect messages
			printf("%s: I receive an incorrect message\n", myMailbox);
		}
		MSG_task_destroy(task_todo);
		task_todo = NULL;
	}

	MSG_task_destroy(task_todo);
	task_todo = NULL;


	xbt_dynar_free(&(workers[id]));
	destroy_content_fifo(&(tasks[id]));
	xbt_fifo_free(tasks[id]);
	destroy_content_fifo(&(processing_tasks[id]));
	xbt_fifo_free(processing_tasks[id]);
	destroy_content_fifo(&(active_groups[id]));
	xbt_fifo_free(active_groups[id]);
	destroy_content_fifo(&(additional_replication_tasks[id]));
	xbt_fifo_free(additional_replication_tasks[id]);
	xbt_dynar_free(&(to_change_primary[id]));
	xbt_dynar_free(&(toSend_loadBalancing[id]->workersToSend));

	return 0;
}

























