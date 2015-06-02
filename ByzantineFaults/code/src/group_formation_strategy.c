#include "msg/msg.h"
#include "group_formation_strategy.h"
#include "primary.h"
#include <time.h>


int compare_reputation_workers (void * w_a, void * w_b) {
	struct p_worker p_w_a = *(struct p_worker *) w_a;
	struct p_worker p_w_b = *(struct p_worker *) w_b;

	if (p_w_a.reputation > p_w_b.reputation) {
		return 1;
	}
	else if (p_w_a.reputation < p_w_b.reputation) {
		return -1;
	}
	else {
		return 0;
	}
}


double compute_LOC(xbt_dynar_t * w) {
	int m;
	int i;

	for (m = xbt_dynar_t(


}

void formGroup_fixed_fit() {
	int i;
	int nb_rand;
	struct p_worker * toAdd = (struct p_worker *) malloc(sizeof(struct p_worker));

	srand(time(NULL));

	while (xbt_dynar_length(workers) >= NB_FIXED_GROUP) {
		xbt_dynar_t * w = (xbt_dynar_t *)malloc(sizeof(xbt_dynar_t));
		*w = xbt_dynar_new(sizeof(struct p_worker), NULL);	


		for(i = 0; i < NB_FIXED_GROUP; i++) {
			nb_rand = rand() % (xbt_dynar_length(workers));
			//printf("value nb_rand=%d\n", nb_rand);
			xbt_dynar_remove_at(workers, nb_rand, (void *)toAdd);
			//printf("remove toAdd.name= %s\ntoAdd.reputation=%d\n", toAdd->mailbox, toAdd->reputation);
			xbt_dynar_push(*w, toAdd);
		}
		xbt_fifo_push(inactive_groups, w);
	}
}


void formGroup_first_fit(double target_LOC) {
	xbt_dynar_sort(workers, compare_reputation_workers);
	struct p_worker * toAdd = (struct p_worker *) malloc(sizeof(struct p_worker));

	while (xbt_dynar_length(workers) >= NB_MIN_GROUP) {
		xbt_dynar_t * w = (xbt_dynar_t *)malloc(sizeof(xbt_dynar_t));			
		*w = xbt_dynar_new(sizeof(struct p_worker), NULL);	
		double LOC = 0.0;

		do {
			xbt_dynar_remove_at(workers, FIRST_ITEM, (void *)toAdd);
			xbt_dynar_push(*w, toAdd);
		
			if (xbt_dynar_length(*w) >= NB_MIN_GROUP) {
				LOC = compute_LOC(w);
			}
		} while(((LOC >= target_LOC) && (xbt_dynar_length(*w) >= NB_MIN_GROUP)) || (xbt_dynar_length(*w) == NB_MAX_GROUP) || (xbt_dynar_length(workers) == 0)));
		if (xbt_dynar_length(workers) == 0) {
			if((LOC >= target_LOC) && (xbt_dynar_length(*w) >= NB_MIN_GROUP)) {
				xbt_fifo_push(inactive_groups, w);
				break;
			}
			else {
				// we have to put the workers back to the workers list: we have try to create a group but there isn't quiet workers left permitting to have a good LOC
				int i;
				for (i = 0; i < xbt_dynar_length(*w); i++) {
					xbt_dynar_remove_at(*w, FIRST_ITEM, (void *)toAdd);
					xbt_dynar_push(workers, toAdd);
				}				
				break;
			}
		}
		else {
			xbt_fifo_push(inactive_groups, w);
		}
	}
}


void formGroup_tight_fit() {
	xbt_dynar_sort(workers, compare_reputation_workers);


}


void formGroup_random_fit() {
	int nb_rand;
	struct p_worker * toAdd = (struct p_worker *) malloc(sizeof(struct p_worker));

	srand(time(NULL));

	while (xbt_dynar_length(workers) >= NB_MIN_GROUP) {
		xbt_dynar_t * w = (xbt_dynar_t *)malloc(sizeof(xbt_dynar_t));			
		*w = xbt_dynar_new(sizeof(struct p_worker), NULL);	
		double LOC = 0.0;

		do {
			nb_rand = rand() % (xbt_dynar_length(workers));
			xbt_dynar_remove_at(workers, nb_rand, (void *)toAdd);
			xbt_dynar_push(*w, toAdd);

			LOC = compute_LOC(w);
		} while(((LOC >= target_LOC) && (xbt_dynar_length(*w) >= NB_MIN_GROUP)) || (xbt_dynar_length(*w) == NB_MAX_GROUP) || (xbt_dynar_length(workers) == 0)));
		if (xbt_dynar_length(workers) == 0) {
			if((LOC >= target_LOC) && (xbt_dynar_length(*w) >= NB_MIN_GROUP)) {
				xbt_fifo_push(inactive_groups, w);
				break;
			}
			else {
				// we have to put the workers back to the workers list: we have try to create a group but there isn't quiet workers left permitting to have a good LOC
				int i;
				for (i = 0; i < xbt_dynar_length(*w); i++) {
					xbt_dynar_remove_at(*w, FIRST_ITEM, (void *)toAdd);
					xbt_dynar_push(workers, toAdd);
				}				
				break;
			}
		}
		else {
			xbt_fifo_push(inactive_groups, w);
		}
	}
}


void formGroup_Arantes() {

}












