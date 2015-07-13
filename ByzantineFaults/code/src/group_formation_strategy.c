#include "msg/msg.h"
#include "simulator.h"
#include "group_formation_strategy.h"
#include "primary.h"
#include "client.h"
#include <time.h>
#include <math.h>


int compare_reputation_workers (const void * w_a, const void * w_b) {
	struct p_worker p_w_a = *(struct p_worker *) w_a;
	struct p_worker p_w_b = *(struct p_worker *) w_b;

	if (p_w_a.reputation < p_w_b.reputation) {
		MSG_task_execute(MSG_task_create("task_complexity", 1.0, 0, NULL));
		return 1;
	}
	else if (p_w_a.reputation > p_w_b.reputation) {
		MSG_task_execute(MSG_task_create("task_complexity", 2.0, 0, NULL));
		return -1;
	}
	else {
		MSG_task_execute(MSG_task_create("task_complexity", 2.0, 0, NULL));
		return 0;
	}
}


long int factorial (int n) {
	int i;	
	long int res = 1;

	for (i = 1; i <= n; i++) {
		res = res * i;
	}

	MSG_task_execute(MSG_task_create("task_complexity", n * 2.0 + 1.0, 0, NULL));
	return res;
}


double combination (int a, int b) {
	long int fact = factorial(b);
	long int tmp = 1;
	int i;

	for (i = 0; i <= b - 1; i++) {
		tmp = tmp * (a - i);
	}	

	MSG_task_execute(MSG_task_create("task_complexity", b * 3.0 + 1.0, 0, NULL));
	return (double)tmp / (double)fact;
}


double compute_LOC(xbt_dynar_t * w) {
	int m;
	double res = 0.0;
	double reputation;
	double tmp;
	double alpha = 0.0;
	double comb = 0.0;
	unsigned int cpt;
	struct p_worker p_w; 
	double complexity = 0.0;

	m = ((int)((double)xbt_dynar_length(*w) / 2.0) + 1);
	complexity = complexity + 8.0;

	for (m = ((int)((double)xbt_dynar_length(*w) / 2.0) + 1); m <= xbt_dynar_length(*w); m++) {
		comb = combination(xbt_dynar_length(*w), m);
		tmp = 1.0;
		alpha = combination(xbt_dynar_length(*w) - 1, m - 1) / combination(xbt_dynar_length(*w), m);
		complexity = complexity + 9.0;

		xbt_dynar_foreach (*w, cpt, p_w) {
			reputation = (double)p_w.reputation / 100.0;
			tmp = tmp * pow(reputation, alpha) * pow((1 - reputation), (1 - alpha));
		}
		res = res + tmp * comb;
		complexity = complexity + 9.0 * cpt + 3.0;
	}

	MSG_task_execute(MSG_task_create("task_complexity", complexity, 0, NULL));
	return res;
}


void formGroup_fixed_fit(msg_task_t * task_to_treat, int id) {
	int i;
	int nb_rand;
	int until = group_formation_fixed_number;
	struct p_worker * toAdd = (struct p_worker *) malloc(sizeof(struct p_worker));
	double complexity = 0.0;


	if ((simulator == ARANTES) && (additional_replication_strategy == PROGRESSIVE_REDUNDANCY)) {
		until = floor((double)group_formation_fixed_number / 2.0) + 1;
		complexity += 4.0;
	}
	complexity += 2.0;

	xbt_dynar_t * w = (xbt_dynar_t *)malloc(sizeof(xbt_dynar_t));
	*w = xbt_dynar_new(sizeof(struct p_worker), NULL);	

	for(i = 0; i < until; i++) {
		nb_rand = rand() % (xbt_dynar_length(workers[id]));
		//printf("value nb_rand=%d\n", nb_rand);
		xbt_dynar_remove_at(workers[id], nb_rand, (void *)toAdd);
			
		//printf("remove toAdd.name= %s\ntoAdd.reputation=%d\n", toAdd->mailbox, toAdd->reputation);
		xbt_dynar_push(*w, toAdd);
	}
	printf("workers on the group formed\n");
	workers_print(w);
	complexity = complexity + until * 5.0;
	MSG_task_execute(MSG_task_create("task_complexity", complexity, 0, NULL));

	treat_tasks(w, task_to_treat, id);
}


// this function do the same thing that formGroup_first_Arantes but the groups formed are odd.
void formGroup_first_fit_Sonnek(msg_task_t * task_to_treat, int id) {
	double LOC;
	double complexity = 0.0;
	xbt_dynar_sort(workers[id], compare_reputation_workers);
	struct p_worker * toAdd = (struct p_worker *) malloc(sizeof(struct p_worker));

	xbt_dynar_t * w = (xbt_dynar_t *)malloc(sizeof(xbt_dynar_t));			
	*w = xbt_dynar_new(sizeof(struct p_worker), NULL);	
	LOC = 0.0;
	struct clientDataTask * data = MSG_task_get_data(*task_to_treat);
	double target_LOC = data->target_LOC;

	do {
		xbt_dynar_remove_at(workers[id], FIRST_ITEM, (void *)toAdd);
		xbt_dynar_push(*w, toAdd);
	
		if (xbt_dynar_length(*w) >= group_formation_min_number) {
			LOC = compute_LOC(w);
			complexity++;
		}
		complexity += 14.0;

	} while(((LOC < target_LOC) || (xbt_dynar_length(*w) < group_formation_min_number) || (xbt_dynar_length(*w) % 2 == 0)) && (xbt_dynar_length(*w) != group_formation_max_number) && (xbt_dynar_length(workers[id]) != 0));
	if (xbt_dynar_length(workers[id]) == 0) {
		if(((LOC >= target_LOC) && (xbt_dynar_length(*w) >= group_formation_min_number) && (xbt_dynar_length(*w) % 2 != 0)) || (xbt_dynar_length(*w) == group_formation_max_number)) {
			treat_tasks(w, task_to_treat, id);
			printf("a group of %ld workers is formed with target LOC %f\n", xbt_dynar_length(*w), LOC);
		}
		else {
			printf("tried to form a group but failed\n");
			// we have to put the workers back to the workers list: we have try to create a group but there isn't quiet workers left permitting to have a good LOC
			int i;
			long int until = xbt_dynar_length(*w);
			for (i = 0; i < until; i++) {
				xbt_dynar_remove_at(*w, FIRST_ITEM, (void *)toAdd);
				xbt_dynar_push(workers[id], toAdd);
			}		
			complexity += 2.0 + until * 2.0; 
		}
		complexity += 8.0;
	}
	else {
		printf("a group of %ld workers is formed with target LOC %f\n", xbt_dynar_length(*w), LOC);
		treat_tasks(w, task_to_treat, id);
	}
	complexity += 2.0;
	MSG_task_execute(MSG_task_create("task_complexity", complexity, 0, NULL));
}


void binary_search_pairs(xbt_dynar_t * w, double * LOC, int index, double target_LOC, int id) {
	struct p_worker * toAdd = (struct p_worker *) malloc(sizeof(struct p_worker));

	if (index <= FIRST_ITEM + 1) {
		xbt_dynar_remove_at(workers[id], index, (void *)toAdd);
		xbt_dynar_push(*w, toAdd);			
		xbt_dynar_remove_at(workers[id], index - 1, (void *)toAdd);
		xbt_dynar_push(*w, toAdd);	

		*LOC = compute_LOC(w);
		MSG_task_execute(MSG_task_create("task_complexity", 8.0, 0, NULL));
		return;
	}
	else {
		xbt_dynar_remove_at(workers[id], index, (void *)toAdd);
		xbt_dynar_push(*w, toAdd);			
		xbt_dynar_remove_at(workers[id], index - 1, (void *)toAdd);
		xbt_dynar_push(*w, toAdd);	

		*LOC = compute_LOC(w);
		MSG_task_execute(MSG_task_create("task_complexity", 9.0, 0, NULL));
		if (*LOC >= target_LOC) {
			return;
		}
		else {
			// the two workers add doesn't help to achieve the target_LOC. Search for two better workers
			xbt_dynar_pop(*w, toAdd);
			xbt_dynar_insert_at(workers[id], index - 1, toAdd);
			xbt_dynar_pop(*w, toAdd);
			xbt_dynar_insert_at(workers[id], index - 1, toAdd);

			MSG_task_execute(MSG_task_create("task_complexity", 16.0, 0, NULL));
			return binary_search_pairs(w, LOC, index / 2, target_LOC, id);
		}
	}	
}


void formGroup_tight_fit_Sonnek(msg_task_t * task_to_treat, int id) {
	unsigned int cpt;
	struct p_worker p_w;
	double LOC;
	double complexity = 0.0;

	struct clientDataTask * data = MSG_task_get_data(*task_to_treat);
	double target_LOC = data->target_LOC;

	xbt_dynar_sort(workers[id], compare_reputation_workers);
	struct p_worker * toAdd = (struct p_worker *) malloc(sizeof(struct p_worker));

	int index = xbt_dynar_length(workers[id]) - 1;

	// we search where are the workers with a reputation above 50
	xbt_dynar_foreach(workers[id], cpt, p_w) {
		if (p_w.reputation < 50) {
			index = cpt - 1;
			complexity += 2.0;
			break;
		}
	}
	complexity = complexity + 3.0 + cpt;
	
	printf("value of index %d, length of workers %ld\n", index, xbt_dynar_length(workers[id]));
	if (index >= group_formation_min_number - 1) {
		LOC = 0.0;
		xbt_dynar_t * w = (xbt_dynar_t *)malloc(sizeof(xbt_dynar_t));			
		*w = xbt_dynar_new(sizeof(struct p_worker), NULL);	

		// put the first node in the list and then use binary search to find others pairs of nodes
		xbt_dynar_remove_at(workers[id], FIRST_ITEM, (void *)toAdd);
		xbt_dynar_push(*w, toAdd);	
		index--;	
		printf("value of index %d\n", index);

		complexity += 6.0;

		while ((index >= FIRST_ITEM + 1) && (xbt_dynar_length(*w) < group_formation_max_number)) {
			// at least 2 workers have a reputation above 50, we could do a binary search to add nodes
			binary_search_pairs(w, &LOC, index, target_LOC, id);
			printf("out of binary_search_pairs\n");
			// after the binary_search_pairs it is sure that we have put two workers on the w array
			index = index - 2;
			printf("value of index %d\n", index);		
			if (LOC >= target_LOC) {
				break;
			}
			complexity += 7.0;
		}
		complexity += 4.0;
		if (LOC >= target_LOC) {
			complexity++;
			// we go out of the while boucle because we find a group whom LOC achieve target_LOC
			printf("a group of %ld workers is formed with target LOC %f\n", xbt_dynar_length(*w), LOC);
			treat_tasks(w, task_to_treat, id);
		}
		else if (xbt_dynar_length(*w) >= group_formation_max_number) {
			printf("a group of %ld workers is formed with target LOC %f, we don't want to be in that case\n", xbt_dynar_length(*w), LOC);
			complexity += 3.0;
			treat_tasks(w, task_to_treat, id);
		}
		else if (index < FIRST_ITEM + 1) {
			// we can't form other group of workers. As the algorithm indicate that if it isn't possible to reach group_formation_target_value we search for the group of workers that permit to be near group_formation_target_value, we add the group to the inactive group too. But we decided to put the workers back to the list named workers		
			printf("tried to form a group but failed\n");
			int i;
			long int until = xbt_dynar_length(*w);
			for (i = 0; i < until; i++) {
				xbt_dynar_remove_at(*w, FIRST_ITEM, (void *)toAdd);
				xbt_dynar_push(workers[id], toAdd);
			}	
			complexity = complexity + 7.0 + until * 2.0;
		}
	}
	MSG_task_execute(MSG_task_create("task_complexity", complexity, 0, NULL));
}


void formGroup_random_fit_Sonnek(msg_task_t * task_to_treat, int id) {
	int nb_rand;
	struct p_worker * toAdd = (struct p_worker *) malloc(sizeof(struct p_worker));

	struct clientDataTask * data = MSG_task_get_data(*task_to_treat);
	double target_LOC = data->target_LOC;

	xbt_dynar_t * w = (xbt_dynar_t *)malloc(sizeof(xbt_dynar_t));			
	*w = xbt_dynar_new(sizeof(struct p_worker), NULL);	
	double LOC = 0.0;
	double complexity = 1.0;

	do {
		nb_rand = rand() % (xbt_dynar_length(workers[id]));
		xbt_dynar_remove_at(workers[id], nb_rand, (void *)toAdd);
		xbt_dynar_push(*w, toAdd);

		if (xbt_dynar_length(*w) >= group_formation_min_number) {
			LOC = compute_LOC(w);
			complexity++;
		}

		complexity += 17.0;
	} while(((LOC < target_LOC) || (xbt_dynar_length(*w) < group_formation_min_number) || (xbt_dynar_length(*w) % 2 == 0)) && (xbt_dynar_length(*w) != group_formation_max_number) && (xbt_dynar_length(workers[id]) != 0));
	if (xbt_dynar_length(workers[id]) == 0) {
		if(((LOC >= target_LOC) && (xbt_dynar_length(*w) >= group_formation_min_number) && (xbt_dynar_length(*w) % 2 != 0)) || ((xbt_dynar_length(*w) == group_formation_max_number) && (xbt_dynar_length(*w) % 2 != 0))) {
			printf("a group of %ld workers is formed with target LOC %f\n", xbt_dynar_length(*w), LOC);
			treat_tasks(w, task_to_treat, id);
		}
		else {
			printf("tried to form a group but failed\n");
			// we have to put the workers back to the workers list: we have try to create a group but there isn't quiet workers left permitting to have a good LOC
			int i;
			long int until = xbt_dynar_length(*w);
			for (i = 0; i < until; i++) {
				xbt_dynar_remove_at(*w, FIRST_ITEM, (void *)toAdd);
				xbt_dynar_push(workers[id], toAdd);
			}	
			complexity = complexity + 2.0 + 2.0 * until;			
		}
		complexity += 13.0;
	}
	else {
		complexity += 2.0;
		printf("a group of %ld workers is formed with target LOC %f\n", xbt_dynar_length(*w), LOC);
		treat_tasks(w, task_to_treat, id);
	}
	MSG_task_execute(MSG_task_create("task_complexity", complexity, 0, NULL));
}


double compute_Arantes_PC(xbt_dynar_t * w) {
	unsigned int cpt;
	struct p_worker p_w;
	double res = 1.0;

	printf("nombre de workers %ld\n", xbt_dynar_length(*w));
	xbt_dynar_foreach (*w, cpt, p_w) {
		printf("reputation %u et res %f\n", p_w.reputation, res);
		res = res * ((double)p_w.reputation / 100.0);
	}

	MSG_task_execute(MSG_task_create("task_complexity", 3.0 * cpt + 1.0, 0, NULL));
	printf("value Arantes_PC %f\n", res);
	return res;
}


double compute_Arantes_PB(xbt_dynar_t *w) {
	unsigned int cpt;
	struct p_worker p_w;
	double res = 1.0;

	xbt_dynar_foreach(*w, cpt, p_w) {
		printf("reputation %u et res %f\n", p_w.reputation, res);
		res = res * (1 - ((double)p_w.reputation / 100.0));
	}

	MSG_task_execute(MSG_task_create("task_complexity", 4.0 * cpt + 1.0, 0, NULL));
	printf("value Arantes_PB %f\n", res);
	return res;
}


void formGroup_first_fit_Arantes(msg_task_t * task_to_treat, int id) {
	xbt_dynar_sort(workers[id], compare_reputation_workers);
	struct p_worker * toAdd = (struct p_worker *) malloc(sizeof(struct p_worker));

	struct clientDataTask * data = MSG_task_get_data(*task_to_treat);
	double target_LOC = data->target_LOC;

	xbt_dynar_t * w = (xbt_dynar_t *)malloc(sizeof(xbt_dynar_t));			
	*w = xbt_dynar_new(sizeof(struct p_worker), NULL);	
	double Pc;
	double Pb;
	double res;
	double complexity = 1.0;

	do {
		xbt_dynar_remove_at(workers[id], FIRST_ITEM, (void *)toAdd);
		xbt_dynar_push(*w, toAdd);
	
		if (xbt_dynar_length(*w) >= group_formation_min_number) {
			Pc = compute_Arantes_PC(w);
			Pb = compute_Arantes_PB(w);
			res = Pb / Pc;
			complexity += 4.0;
			printf("value of LOC ARANTES %f\n", res);
		}

		complexity += 11.0;

	} while(((res > target_LOC) || (xbt_dynar_length(*w) < group_formation_min_number)) && (xbt_dynar_length(*w) != group_formation_max_number) && (xbt_dynar_length(workers[id]) != 0));
	if (xbt_dynar_length(workers[id]) == 0) {
		if (((res <= target_LOC) && (xbt_dynar_length(*w) >= group_formation_min_number)) || (xbt_dynar_length(*w) == group_formation_max_number)) {
			treat_tasks(w, task_to_treat, id);
		}
		else {
			// we have to put the workers back to the workers list: we have try to create a group but there isn't quiet workers left permitting to have a good threshold
			printf("there isn't enough workers to have a good treshold\n");
			int i;
			long int until = xbt_dynar_length(*w);
			for (i = 0; i < until; i++) {
				xbt_dynar_remove_at(*w, FIRST_ITEM, (void *)toAdd);
				xbt_dynar_push(workers[id], toAdd);
			}	
			complexity = complexity + until * 2.0 + 2.0;			
		}
		complexity += 5.0;
	}
	else {
		treat_tasks(w, task_to_treat, id);
	}
	complexity += 2.0;
	MSG_task_execute(MSG_task_create("task_complexity", complexity, 0, NULL));
}


void formGroup_random_fit_Arantes(msg_task_t * task_to_treat, int id) {
	int nb_rand;
	struct p_worker * toAdd = (struct p_worker *) malloc(sizeof(struct p_worker));

	struct clientDataTask * data = MSG_task_get_data(*task_to_treat);
	double target_LOC = data->target_LOC;

	xbt_dynar_t * w = (xbt_dynar_t *)malloc(sizeof(xbt_dynar_t));			
	*w = xbt_dynar_new(sizeof(struct p_worker), NULL);	
	double res = 0.0;
	double Pc;
	double Pb;
	double complexity = 1.0;

	do {
		nb_rand = rand() % (xbt_dynar_length(workers[id]));
		xbt_dynar_remove_at(workers[id], nb_rand, (void *)toAdd);
		xbt_dynar_push(*w, toAdd);

		if (xbt_dynar_length(*w) >= group_formation_min_number) {
			Pc = compute_Arantes_PC(w);
			Pb = compute_Arantes_PB(w);
			res = Pb / Pc;
			complexity += 4.0;
		}
		complexity += 14.0;
	} while(((res > target_LOC) || (xbt_dynar_length(*w) < group_formation_min_number)) && (xbt_dynar_length(*w) != group_formation_max_number) && (xbt_dynar_length(workers[id]) != 0));
	if (xbt_dynar_length(workers[id]) == 0) {
		if(((res <= target_LOC) && (xbt_dynar_length(*w) >= group_formation_min_number)) || (xbt_dynar_length(*w) == group_formation_max_number)) {
			treat_tasks(w, task_to_treat, id);
		}
		else {
			// we have to put the workers back to the workers list: we have try to create a group but there isn't quiet workers left permitting to have a good LOC
			int i;
			long int until = xbt_dynar_length(*w);
			for (i = 0; i < until; i++) {
				xbt_dynar_remove_at(*w, FIRST_ITEM, (void *)toAdd);
				xbt_dynar_push(workers[id], toAdd);
			}	
			complexity = complexity + 2.0 * until + 2.0;			
		}
		complexity += 5.0;
	}
	else {
		treat_tasks(w, task_to_treat, id);
	}
	complexity += 2.0;
}


void binary_search_one (xbt_dynar_t * w, double * res, int index, double target_LOC, int id) {
	struct p_worker * toAdd = (struct p_worker *) malloc(sizeof(struct p_worker));

	if (index == FIRST_ITEM) {
		xbt_dynar_remove_at(workers[id], index, (void *)toAdd);
		xbt_dynar_push(*w, toAdd);			

		*res = compute_Arantes_PB(w) / compute_Arantes_PC(w);
		MSG_task_execute(MSG_task_create("task_complexity", 5.0, 0, NULL));
		return;
	}
	else {	
		xbt_dynar_remove_at(workers[id], index, (void *)toAdd);
		xbt_dynar_push(*w, toAdd);	

		*res = compute_Arantes_PB(w) / compute_Arantes_PC(w);
		if (*res <= target_LOC) {
			MSG_task_execute(MSG_task_create("task_complexity", 6.0, 0, NULL));
			return;
		}
		else {
			// the added worker doesn't help to achieve the target_LOC. Search for an other worker
			xbt_dynar_pop(*w, toAdd);
			xbt_dynar_insert_at(workers[id], index, toAdd);
			
			MSG_task_execute(MSG_task_create("task_complexity", 9.0, 0, NULL));
			return binary_search_one(w, res, index / 2, target_LOC, id);
		}
	}
}


void formGroup_tight_fit_Arantes(msg_task_t * task_to_treat, int id) {
	unsigned int cpt;
	struct p_worker p_w;
	double complexity = 0.0;

	struct clientDataTask * data = MSG_task_get_data(*task_to_treat);
	double target_LOC = data->target_LOC;

	xbt_dynar_sort(workers[id], compare_reputation_workers);
	struct p_worker * toAdd = (struct p_worker *) malloc(sizeof(struct p_worker));

	xbt_dynar_t * w;

	int index = xbt_dynar_length(workers[id]) - 1;

	// we search where are the workers with a reputation above 50
	xbt_dynar_foreach(workers[id], cpt, p_w) {
		if (p_w.reputation < 50) {
			index = cpt - 1;
			complexity += 2.0;
			break;
		}
	}

	complexity = complexity + 4.0 + cpt;

	if (index >= group_formation_min_number) {
		double res = 0.0;
		w = (xbt_dynar_t *)malloc(sizeof(xbt_dynar_t));			
		*w = xbt_dynar_new(sizeof(struct p_worker), NULL);	

		// put the first node in the list and then use binary search to find other nodes
		xbt_dynar_remove_at(workers[id], FIRST_ITEM, (void *)toAdd);
		xbt_dynar_push(*w, toAdd);	
		index--;	

		complexity += 5.0;
		while (index >= FIRST_ITEM) {
			// at least 1 worker have a reputation above 50, we could do a binary search to add nodes
			binary_search_one(w, &res, index, target_LOC, id);
			// after the binary_search_one it is sure that we have put one worker on the w array
			index--;	
			complexity += 3.0;	
			if (res <= target_LOC) {
				break;
			}
			else if (xbt_dynar_length(*w) >= group_formation_max_number) {
				complexity += 2.0;
				break;
			}
		}
		complexity++;
		if ((res <= target_LOC) || (xbt_dynar_length(*w) >= group_formation_max_number)) {
			// we go out of the while boucle because we find a group whom LOC achieve target_LOC
			treat_tasks(w, task_to_treat, id);
		}
		else if (index < FIRST_ITEM) {
			// we can't form other group of workers. As the algorithm indicate that if it isn't possible to reach target_LOC we search for the group of workers that permit to be near target_LOC, we add the group to the inactive group too
			complexity++;
			treat_tasks(w, task_to_treat, id);
		}
		complexity += 3.0;
	}
	MSG_task_execute(MSG_task_create("task_complexity", complexity, 0, NULL));
}









