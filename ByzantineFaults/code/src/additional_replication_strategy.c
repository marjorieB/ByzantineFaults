#include "additional_replication_strategy.h"
#include "group_formation_strategy.h"
#include "simulator.h"
#include "primary.h"
#include <math.h>


int verify (struct p_task * p_t, struct p_worker w) {
	unsigned int cpt;
	struct p_answer_worker p_a_w;

	xbt_dynar_foreach(p_t->w_answers, cpt, p_a_w) {
		unsigned int nb;
		char * name;

		xbt_dynar_foreach(p_a_w.worker_names, nb, name) {
			if (!strcmp(w.mailbox, name)) {
				return -1;
			}
		}
	}
	return 1;
}


void find_workers_fixed_fit (struct p_task * p_t, xbt_dynar_t * array_tmp, int * nb_replications) {
	int nb_rand;
	struct p_worker * tmp = (struct p_worker *) malloc(sizeof(struct p_worker));	

	while ((xbt_dynar_length(workers) > 0) && (*nb_replications > 0)) {
		nb_rand = rand() % (xbt_dynar_length(workers));
		xbt_dynar_remove_at(workers, nb_rand, (void *)tmp);
		// we need to check if the node wasn't already used for that task
		if (verify(p_t, *tmp) == 1) {
			// we need to put the worker in the active group and to send it the task
			xbt_dynar_push(*(p_t->active_workers), tmp);
			*nb_replications = *nb_replications - 1;
			p_t->nb_forwarded++;

			msg_task_t to_send = MSG_task_create(p_t->task_name, p_t->duration, p_t->size, p_t->client);
			MSG_task_send(to_send, tmp->mailbox);
		}
		else {
			// the node can't be used for this task, we will put it again in the array names workers after
			xbt_dynar_push(*array_tmp, tmp);
		}
	}	
}


double valueCond2_replication (struct p_task * p_t) {
	double Psc = 1.0;
	double tmp = 1.0;
	unsigned int cpt;
	struct p_answer_worker p;
	unsigned int nb;
	unsigned char reputation;	


	xbt_dynar_foreach(p_t->w_answers, cpt, p) {
	
		if (p_t->res == xbt_dynar_get_ptr(p_t->w_answers, cpt)) {
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
	xbt_dynar_foreach(p_t->additional_reputations, cpt, reputation) {
		Psc = Psc * ((double)reputation/100.0);
		tmp = tmp * (1.0 - ((double)reputation/100.0));
	}

	return (Psc / (tmp + Psc));
}


void replication_fixed_fit (struct p_task * p_t) {
	int nb_replications;
	int i;
	int nb;

	struct p_worker * tmp = (struct p_worker *) malloc(sizeof(struct p_worker));	
	xbt_dynar_t array_tmp = xbt_dynar_new(sizeof(struct p_worker), NULL);

	// additional_replication_strategy == ITERATIVE_REDUNDANCY
	if ((p_t->to_replicate == 0) && (additional_replication_strategy == ITERATIVE_REDUNDANCY)) {
		nb_replications = additional_replication_value_difference - (p_t->nb_forwarded - xbt_dynar_length(p_t->res->worker_names));
	}
	else if ((p_t->to_replicate == 0) && (additional_replication_strategy == PROGRESSIVE_REDUNDANCY)) {
		nb_replications = floor((double)group_formation_fixed_number / 2.0) + 1 - xbt_dynar_length(p_t->res->worker_names);
	}
	else {
		nb_replications = p_t->to_replicate;
	}

	find_workers_fixed_fit(p_t, &array_tmp, &nb_replications);

	// we go out of the function find_workers without being able to find workers available to do the the execution of the task
	// we will search in the inactive_groups of workers if there are some nodes able to execute the task
	while ((xbt_fifo_size(inactive_groups) > 0) && (nb_replications > 0)) {
		// we will broke the inactive_group and put the workers in the array named workers
		xbt_dynar_t array_w = fifo_supress_head(inactive_groups);
		nb = xbt_dynar_length(array_w);			

		for (i = 0; i < nb; i++) {
			struct p_worker * p_w = NULL;
			xbt_dynar_pop(array_w, p_w);
			xbt_dynar_push(workers, p_w);
		}

		find_workers_fixed_fit(p_t, &array_tmp, &nb_replications);
	}
	
	if (nb_replications > 0) {
		// there aren't workers available left to execute this task, we need to put it in a fifo with some information about the replications that stay
		p_t->to_replicate = nb_replications;
		xbt_fifo_push(additional_replication_tasks, p_t);
	}

	// we have to put the workers in array_tmp back in the array named workers
	nb = xbt_dynar_length(array_tmp);
	for (i = 0; i < nb; i++) {
		xbt_dynar_pop(array_tmp, tmp);
		xbt_dynar_push(workers, tmp);
	}
}


int find_workers_first_fit (struct p_task * p_t, xbt_dynar_t * array_tmp) {
	// sort the array of workers
	xbt_dynar_sort(workers, compare_reputation_workers);

	struct p_worker * tmp = (struct p_worker *) malloc(sizeof(struct p_worker));	

	while (xbt_dynar_length(workers) > 0) {
		xbt_dynar_remove_at(workers, FIRST_ITEM, (void *)tmp);
		// we need to check if the node wasn't already used for that task
		if (verify(p_t, *tmp) == 1) {
			// we need to put the worker in the active group and to send it the task
			xbt_dynar_push(*(p_t->active_workers), tmp);
			p_t->nb_forwarded++;
			xbt_dynar_push(p_t->additional_reputations, &(tmp->reputation));

			msg_task_t to_send = MSG_task_create(p_t->task_name, p_t->duration, p_t->size, p_t->client);
			MSG_task_send(to_send, tmp->mailbox);

			if (valueCond2_replication(p_t) > (1 - group_formation_target_value)) {
				return 1;
			}
		}
		else {
			// the node can't be used for this task, we will put it again in the array names workers after
			xbt_dynar_push(*array_tmp, tmp);
		}
	}	
	return -1;
}


double binary_search_one_replication(struct p_task * p_t, xbt_dynar_t * array_tmp, int index) {
	struct p_worker * toAdd = NULL;
	double ret;

	if (index == FIRST_ITEM) {
		xbt_dynar_remove_at(workers, index, (void *)toAdd);
		if (verify (p_t, *toAdd) == -1) {
			// the node can't be added to execute this task
			xbt_dynar_push (*array_tmp, toAdd);
			return -1;
		}
		else {
			xbt_dynar_push(*(p_t->active_workers), toAdd);
			p_t->nb_forwarded++;
			xbt_dynar_push(p_t->additional_reputations, &(toAdd->reputation));

			msg_task_t to_send = MSG_task_create(p_t->task_name, p_t->duration, p_t->size, p_t->client);
			MSG_task_send(to_send, toAdd->mailbox);

			return valueCond2_replication (p_t);
		}
	}
	else {	
		xbt_dynar_remove_at(workers, index, (void *)toAdd);

		if (verify (p_t, *toAdd) == -1) {
			// the node can't be added to execute this task
			xbt_dynar_push(*array_tmp, toAdd);
			return -1;
		}
		else {
			xbt_dynar_push(p_t->additional_reputations, &(toAdd->reputation));	

			if ((ret = valueCond2_replication (p_t)) > (1 - group_formation_target_value)) {
				xbt_dynar_push(*(p_t->active_workers), toAdd);
				p_t->nb_forwarded++;

				msg_task_t to_send = MSG_task_create(p_t->task_name, p_t->duration, p_t->size, p_t->client);
				MSG_task_send(to_send, toAdd->mailbox);

				return ret;
			}
			else {
				// the added worker doesn't help to achieve the group_formation_target_value. Search for an other worker
				xbt_dynar_pop((xbt_dynar_t const)p_t->additional_reputations, NULL);
				xbt_dynar_insert_at(workers, index, toAdd);
			
				return binary_search_one_replication(p_t, array_tmp, index / 2);
			}
		}
	}
}


int find_workers_tight_fit (struct p_task * p_t, xbt_dynar_t * array_tmp) {
	// sort the array of workers
	xbt_dynar_sort(workers, compare_reputation_workers);

	double value_res;	
	unsigned int cpt;
	struct p_worker p_w;
	int index;

	// we search where are the workers with a reputation above 50
	xbt_dynar_foreach(workers, cpt, p_w) {
		if (p_w.reputation < 50) {
			index = cpt - 1;
			break;
		}
	}

	while (index >= FIRST_ITEM) {
		// at least 1 worker have a reputation above 50, we could do a binary search to add nodes
		value_res = binary_search_one_replication(p_t, array_tmp, index);
		// after the binary_search_one_replication it is sure that we have suppressed a worker from the array named workers
		index--;		
		if (value_res > (1 - group_formation_target_value)) {
			break;
		}
	}
	if (value_res > (1 - group_formation_target_value)) {
		return 1;
	}
	else {
		return -1;
	}
}


int find_workers_random_fit (struct p_task * p_t, xbt_dynar_t * array_tmp) {
	struct p_worker * tmp = (struct p_worker *) malloc(sizeof(struct p_worker));	
	int nb_rand;

	while (xbt_dynar_length(workers) > 0) {
		nb_rand = rand() % (xbt_dynar_length(workers));
		xbt_dynar_remove_at(workers, nb_rand, (void *)tmp);
		// we need to check if the node wasn't already used for that task
		if (verify(p_t, *tmp) == 1) {
			// we need to put the worker in the active group and to send it the task
			xbt_dynar_push(*(p_t->active_workers), tmp);
			p_t->nb_forwarded++;
			xbt_dynar_push(p_t->additional_reputations, &(tmp->reputation));

			msg_task_t to_send = MSG_task_create(p_t->task_name, p_t->duration, p_t->size, p_t->client);
			MSG_task_send(to_send, tmp->mailbox);

			if (valueCond2_replication(p_t) > (1 - group_formation_target_value)) {
				return 1;
			}
		}
		else {
			// the node can't be used for this task, we will put it again in the array names workers after
			xbt_dynar_push(*array_tmp, tmp);
		}
	}	
	return -1;
}


void replication_others_fit (struct p_task * p_t) {
	int ret;
	int nb; 	
	int i;

	struct p_worker * tmp = (struct p_worker *) malloc(sizeof(struct p_worker));	
	xbt_dynar_t array_tmp = xbt_dynar_new(sizeof(struct p_worker), NULL);
	
	if (group_formation_strategy == FIRST_FIT) {
		ret = find_workers_first_fit(p_t, &array_tmp);
	}
	else if (group_formation_strategy == TIGHT_FIT) {
		ret = find_workers_tight_fit(p_t, &array_tmp);
	}
	else {
		ret = find_workers_random_fit(p_t, &array_tmp);
	}

	// we go out of the function find_workers without being able to find workers available to do the the execution of the task
	// we will search in the inactive_groups of workers if there are some nodes able to execute the task
	while ((xbt_fifo_size(inactive_groups) > 0) && (ret == -1)) {
		// we will broke the inactive_group and put the workers in the array named workers
		xbt_dynar_t array_w = fifo_supress_head(inactive_groups);
		nb = xbt_dynar_length(array_w);		

		for (i = 0; i < nb; i++) {
			struct p_worker * p_w = NULL;
			xbt_dynar_pop(array_w, p_w);
			xbt_dynar_push(workers, p_w);
		}

		if (group_formation_strategy == FIRST_FIT) {
			ret = find_workers_first_fit(p_t, &array_tmp);
		}
		else if (group_formation_strategy == TIGHT_FIT) {
			ret = find_workers_tight_fit(p_t, &array_tmp);
		}
		else {
			ret = find_workers_random_fit(p_t, &array_tmp);
		}
	}
	
	if (ret == -1) {
		// there aren't workers available left to execute this task, we need to put it in a fifo with some information about the replications that stay
		xbt_fifo_push(additional_replication_tasks, p_t);
	}

	// we have to put the workers in array_tmp back in the array named workers
	nb = xbt_dynar_length(array_tmp);
	for (i = 0; i < nb; i++) {
		xbt_dynar_pop(array_tmp, tmp);
		xbt_dynar_push(workers, tmp);
	}
}
