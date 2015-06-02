#include "msg/msg.h"
#include "group_formation_strategy.h"
#include "primary.h"
#include <time.h>

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
