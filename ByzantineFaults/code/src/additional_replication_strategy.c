#include "additional_replication_strategy.h"
#include "group_formation_strategy.h"
#include "simulator.h"
#include "primary.h"
#include <math.h>


int verify (struct p_task * p_t, struct p_worker w) {
	unsigned int cpt;
	struct p_answer_worker p_a_w;
	double complexity = 0.0;

	printf("in verify\n");
	xbt_dynar_foreach(p_t->w_answers, cpt, p_a_w) {
		unsigned int nb;
		char name[MAILBOX_SIZE];
		
		printf("looking in the p_t: p_a_w.answer = %ld, nb %ld\n", p_a_w.answer, xbt_dynar_length(p_a_w.worker_names));

		xbt_dynar_foreach(p_a_w.worker_names, nb, name) {
			printf("name of the worker %s\n", name); 
			if (!strcmp(w.mailbox, name)) {
				MSG_task_execute(MSG_task_create("task_complexity", complexity + nb, 0, NULL));
				return -1;
			}
		}
		complexity += nb;
	}
	MSG_task_execute(MSG_task_create("task_complexity", complexity, 0, NULL));
	return 1;
}


void find_workers_fixed_fit (struct p_task * p_t, xbt_dynar_t * array_tmp, int * nb_replications, int id) {
	int nb_rand;
	struct p_worker * tmp = (struct p_worker *) malloc(sizeof(struct p_worker));	
	double complexity = 0.0;

	while ((xbt_dynar_length(workers[id]) > 0) && (*nb_replications > 0)) {
		nb_rand = rand() % (xbt_dynar_length(workers[id]));
		xbt_dynar_remove_at(workers[id], nb_rand, (void *)tmp);
		printf("the node chosen was %s\n", tmp->mailbox);
		// we need to check if the node wasn't already used for that task
		if (verify(p_t, *tmp) == 1) {
			printf("%s haven't already execute this request\n", tmp->mailbox);
			// we need to put the worker in the active group and to send it the task
			xbt_dynar_push(*(xbt_dynar_t *)(xbt_fifo_get_item_content(p_t->active_workers)), tmp);
			groups_print(&(active_groups[id]));
			*nb_replications = *nb_replications - 1;
			p_t->nb_forwarded++;

			complexity += 4.0;

			msg_task_t to_send = MSG_task_create(p_t->task_name, p_t->duration, p_t->size, p_t->client);
			MSG_task_send(to_send, tmp->mailbox);
		}
		else {
			printf("the node has already been used for this task we put it in array_tmp\n");
			// the node can't be used for this task, we will put it again in the array names workers after
			xbt_dynar_push(*array_tmp, tmp);
			complexity++;
		}
		complexity += 8.0;
	}	
	complexity += 3.0;
	MSG_task_execute(MSG_task_create("task_complexity", complexity, 0, NULL));
}


double valueCond2_replication (struct p_task * p_t) {
	double Psc = 1.0;
	double tmp = 1.0;
	unsigned int cpt;
	struct p_answer_worker p;
	unsigned char reputation;	
	unsigned int i;
	double complexity = 0.0;

	xbt_dynar_foreach(p_t->w_answers, cpt, p) {
		unsigned int nb;
	
		if (p_t->res == xbt_dynar_get_ptr(p_t->w_answers, cpt)) {
			printf("calculating psc of the workers giving the majoritary answer\n");
			xbt_dynar_foreach(p.worker_reputations, nb, reputation) {
				Psc = Psc * ((double)reputation/100.0);
				tmp = tmp * (1.0 - ((double)reputation/100.0));
			}
			complexity = complexity + 7.0 * nb;
		}
		else {
			printf("calculating psc of the workers that didn't give the majoritaty answer\n");
			xbt_dynar_foreach(p.worker_reputations, nb, reputation) {
				Psc = Psc * (1.0 - ((double)reputation/100.0));
			} 
			complexity = complexity + 4.0 * nb;
		}
		complexity++;
	}	
	printf("calculating psc of the workers added supposed to agree with the majoritary answer %ld\n", xbt_dynar_length(p_t->additional_reputations));
	xbt_dynar_foreach(p_t->additional_reputations, i, reputation) {
		Psc = Psc * ((double)reputation/100.0);
		//printf("new value of PSC = %f\n", Psc);
		tmp = tmp * (1.0 - ((double)reputation/100.0));
		//printf("new value of tmp = %f\n", tmp);
	}
	complexity = complexity + 7.0 * i + 2.0;

	printf("value of cond2 : %f\n", (Psc / (tmp + Psc)));

	MSG_task_execute(MSG_task_create("task_complexity", complexity, 0, NULL));	
	return (Psc / (tmp + Psc));
}


void replication_fixed_fit (struct p_task * p_t, int id) {
	int nb_replications;
	int i;
	int nb;
	double complexity = 0.0;

	workers_print(&(workers[id]));
	printf("active group\n");
	groups_print(&(active_groups[id]));

	struct p_worker * tmp = (struct p_worker *) malloc(sizeof(struct p_worker));	
	xbt_dynar_t array_tmp = xbt_dynar_new(sizeof(struct p_worker), NULL);

	// additional_replication_strategy == ITERATIVE_REDUNDANCY
	if ((p_t->to_replicate == 0) && (additional_replication_strategy == ITERATIVE_REDUNDANCY)) {
		nb_replications = additional_replication_value_difference - (xbt_dynar_length(p_t->res->worker_names) - (p_t->nb_forwarded - xbt_dynar_length(p_t->res->worker_names)));
		complexity += 8.0;
		printf("value of the additional tasks needed = %d\n", nb_replications); 
	}
	else if ((p_t->to_replicate == 0) && (additional_replication_strategy == PROGRESSIVE_REDUNDANCY)) {
		nb_replications = floor((double)group_formation_fixed_number / 2.0) + 1 - xbt_dynar_length(p_t->res->worker_names);
		printf("value of the additional tasks needed = %d\n", nb_replications); 
		complexity += 10.0;
	}
	else {
		nb_replications = p_t->to_replicate;
		complexity += 5.0;
	}

	find_workers_fixed_fit(p_t, &array_tmp, &nb_replications, id);
	printf("after searching in the workers groups\n");
	
	if (nb_replications > 0) {
		// there aren't workers available left to execute this task, we need to put it in a fifo with some information about the replications that stay
		p_t->to_replicate = nb_replications;
		printf("%d \n", p_t->to_replicate);
		printf("we couldn't satisfy all the replications, we put the task on the additional_replication_tasks queue\n");
		xbt_fifo_push(additional_replication_tasks[id], p_t);
		printf("task put in additional_replication_tasks\n");
		complexity += 2.0;
	}
	complexity++;

	// we have to put the workers in array_tmp back in the array named workers
	nb = xbt_dynar_length(array_tmp);
	for (i = 0; i < nb; i++) {
		xbt_dynar_pop(array_tmp, tmp);
		xbt_dynar_push(workers[id], tmp);
	}
	workers_print(&(workers[id]));
	complexity = complexity + 1.0 + 2.0 * nb;
	MSG_task_execute(MSG_task_create("task_complexity", complexity, 0, NULL));
}


int find_workers_first_fit (struct p_task * p_t, xbt_dynar_t * array_tmp, int id) {
	double complexity = 0.0;

	// sort the array of workers
	xbt_dynar_sort(workers[id], compare_reputation_workers);

	struct p_worker * tmp = (struct p_worker *) malloc(sizeof(struct p_worker));	

	while (xbt_dynar_length(workers[id]) > 0) {
		xbt_dynar_remove_at(workers[id], FIRST_ITEM, (void *)tmp);
		printf("the node chosen is %s\n", tmp->mailbox);
		// we need to check if the node wasn't already used for that task
		if (verify(p_t, *tmp) == 1) {
			printf("the node haven't execute the task yet\n");
			// we need to put the worker in the active group and to send it the task
			xbt_dynar_push(*(xbt_dynar_t *)xbt_fifo_get_item_content(p_t->active_workers), tmp);
			p_t->nb_forwarded++;
			xbt_dynar_push(p_t->additional_workers, tmp);
			xbt_dynar_push(p_t->additional_reputations, &(tmp->reputation));

			msg_task_t to_send = MSG_task_create(p_t->task_name, p_t->duration, p_t->size, p_t->client);
			MSG_task_send(to_send, tmp->mailbox);

			complexity += 11.0;
			if (valueCond2_replication(p_t) > (1 - p_t->targetLOC)) {
				MSG_task_execute(MSG_task_create("task_complexity", complexity, 0, NULL));
				printf("replication done: 1 - targetLOC = %f\n", 1 - p_t->targetLOC);
				return 1;
			}
			printf("1 - targetLOC = %f\n", 1 - p_t->targetLOC);
		}
		else {
			printf("the node has already executed this task\n");
			// the node can't be used for this task, we will put it again in the array names workers after
			xbt_dynar_push(*array_tmp, tmp);
			complexity += 5.0;
		}
	}	
	complexity += 2.0;
	MSG_task_execute(MSG_task_create("task_complexity", complexity, 0, NULL));
	return -1;
}


double binary_search_one_replication(struct p_task * p_t, xbt_dynar_t * array_tmp, int index, int id) {
	struct p_worker * toAdd = (struct p_worker *) malloc(sizeof(struct p_worker));
	double ret;
	double complexity = 0.0;

	if (index == FIRST_ITEM) {
		xbt_dynar_remove_at(workers[id], index, (void *)toAdd);
		workers_print(&(workers[id]));
		if (verify (p_t, *toAdd) == -1) {
			printf("the node %s has already execute the task %s\n", toAdd->mailbox, p_t->task_name);
			// the node can't be added to execute this task
			xbt_dynar_push (*array_tmp, toAdd);
			complexity += 4.0;
			MSG_task_execute(MSG_task_create("task_complexity", complexity, 0, NULL));
			return -1;
		}
		else {
			xbt_dynar_push(*(xbt_dynar_t *)xbt_fifo_get_item_content(p_t->active_workers), toAdd);
			p_t->nb_forwarded++;
			xbt_dynar_push(p_t->additional_workers, toAdd);
			xbt_dynar_push(p_t->additional_reputations, &(toAdd->reputation));
			complexity += 8.0;
			MSG_task_execute(MSG_task_create("task_complexity", complexity, 0, NULL));

			msg_task_t to_send = MSG_task_create(p_t->task_name, p_t->duration, p_t->size, p_t->client);
			MSG_task_send(to_send, toAdd->mailbox);

			return valueCond2_replication(p_t);
		}
	}
	else {	
		xbt_dynar_remove_at(workers[id], index, (void *)toAdd);
		workers_print(&(workers[id]));
		if (verify (p_t, *toAdd) == -1) {
			printf("the node %s has already execute the task %s\n", toAdd->mailbox, p_t->task_name);
			// the node can't be added to execute this task
			xbt_dynar_push(*array_tmp, toAdd);
			complexity += 4.0;
			MSG_task_execute(MSG_task_create("task_complexity", complexity, 0, NULL));
			return -1;
		}
		else {
			printf("dans le else\n");
			xbt_dynar_push(p_t->additional_workers, toAdd);
			xbt_dynar_push(p_t->additional_reputations, &(toAdd->reputation));
			printf("after the add of the additional_workers\n");	

			if ((ret = valueCond2_replication (p_t)) > (1 - p_t->targetLOC)) {
				xbt_dynar_push(*(xbt_dynar_t *)xbt_fifo_get_item_content(p_t->active_workers), toAdd);
				p_t->nb_forwarded++;

				complexity += 11.0;
				MSG_task_execute(MSG_task_create("task_complexity", complexity, 0, NULL));

				msg_task_t to_send = MSG_task_create(p_t->task_name, p_t->duration, p_t->size, p_t->client);
				MSG_task_send(to_send, toAdd->mailbox);

				return ret;
			}
			else {
				// the added worker doesn't help to achieve the targetLOC. Search for an other worker
				xbt_dynar_pop(p_t->additional_workers, NULL);
				xbt_dynar_pop(p_t->additional_reputations, NULL);
				xbt_dynar_insert_at(workers[id], index, toAdd);

				complexity += 11.0;
				MSG_task_execute(MSG_task_create("task_complexity", complexity, 0, NULL));

				return binary_search_one_replication(p_t, array_tmp, index / 2, id);
			}
		}
	}
}


int find_workers_tight_fit (struct p_task * p_t, xbt_dynar_t * array_tmp, int id) {
	// sort the array of workers
	xbt_dynar_sort(workers[id], compare_reputation_workers);

	double complexity = 0.0;
	double value_res;	
	unsigned int cpt;
	struct p_worker p_w;
	int index;

	index = xbt_dynar_length(workers[id]) - 1;

	// we search where are the workers with a reputation above 50
	xbt_dynar_foreach(workers[id], cpt, p_w) {
		if (p_w.reputation < 50) {
			index = cpt - 1;
			break;
		}
	}
	complexity = 3.0 + cpt + 2.0;

	workers_print(&(workers[id]));
	printf("value of index %d\n", index);

	while (index >= FIRST_ITEM) {
		// at least 1 worker have a reputation above 50, we could do a binary search to add nodes
		printf("index %d\n", index); 
		value_res = binary_search_one_replication(p_t, array_tmp, index, id);
		// after the binary_search_one_replication it is sure that we have suppressed a worker from the array named workers
		index--;		
		if (value_res > (1 - p_t->targetLOC)) {
			break;
		}
		complexity += 5.0;
	}
	complexity = complexity + 3.0;
	MSG_task_execute(MSG_task_create("task_complexity", complexity, 0, NULL));

	if (value_res > (1 - p_t->targetLOC)) {
		return 1;
	}
	else {
		return -1;
	}
}


int find_workers_random_fit (struct p_task * p_t, xbt_dynar_t * array_tmp, int id) {
	struct p_worker * tmp = (struct p_worker *) malloc(sizeof(struct p_worker));	
	int nb_rand;
	double complexity = 0.0;

	while (xbt_dynar_length(workers[id]) > 0) {
		nb_rand = rand() % (xbt_dynar_length(workers[id]));
		xbt_dynar_remove_at(workers[id], nb_rand, (void *)tmp);
		// we need to check if the node wasn't already used for that task
		if (verify(p_t, *tmp) == 1) {
			// we need to put the worker in the active group and to send it the task
			xbt_dynar_push(*(xbt_dynar_t *)xbt_fifo_get_item_content(p_t->active_workers), tmp);
			p_t->nb_forwarded++;
			xbt_dynar_push(p_t->additional_workers, tmp);
			xbt_dynar_push(p_t->additional_reputations, &(tmp->reputation));
			
			complexity += 14.0;

			msg_task_t to_send = MSG_task_create(p_t->task_name, p_t->duration, p_t->size, p_t->client);
			MSG_task_send(to_send, tmp->mailbox);

			if (valueCond2_replication(p_t) > (1 - p_t->targetLOC)) {
				MSG_task_execute(MSG_task_create("task_complexity", complexity, 0, NULL));
				return 1;
			}
		}
		else {
			// the node can't be used for this task, we will put it again in the array names workers after
			xbt_dynar_push(*array_tmp, tmp);
			complexity += 8.0;
		}
	}	
	complexity += 2.0;
	MSG_task_execute(MSG_task_create("task_complexity", complexity, 0, NULL));

	return -1;
}


void replication_others_fit (struct p_task * p_t, int id) {
	int ret;
	int nb; 	
	int i;
	double complexity = 0.0;

	struct p_worker * tmp = (struct p_worker *) malloc(sizeof(struct p_worker));	
	xbt_dynar_t array_tmp = xbt_dynar_new(sizeof(struct p_worker), NULL);

	if (group_formation_strategy == FIRST_FIT) {
		ret = find_workers_first_fit(p_t, &array_tmp, id);
		complexity += 2.0;
	}
	else if (group_formation_strategy == TIGHT_FIT) {
		ret = find_workers_tight_fit(p_t, &array_tmp, id);
		complexity += 3.0;
	}
	else {
		ret = find_workers_random_fit(p_t, &array_tmp, id);
		complexity += 3.0;
	}
	
	if (ret == -1) {
		// there aren't workers available left to execute this task, we need to put it in a fifo with some information about the replications that stay
		printf("we couldn't satisfy the additional replication. We put the task in the additional replication tasks\n");
		xbt_fifo_push(additional_replication_tasks[id], p_t);
		complexity++;
	}

	// we have to put the workers in array_tmp back in the array named workers
	nb = xbt_dynar_length(array_tmp);
	for (i = 0; i < nb; i++) {
		xbt_dynar_pop(array_tmp, tmp);
		xbt_dynar_push(workers[id], tmp);
	}
	complexity = 3.0 + 2.0 * nb;
	MSG_task_execute(MSG_task_create("task_complexity", complexity, 0, NULL));
}
