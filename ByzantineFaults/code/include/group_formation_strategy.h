#ifndef _GROUP_FORMATION_STRATEGY_H
#define _GROUP_FORMATION_STRATEGY_H


#define NB_FIXED_GROUP 2
#define NB_MIN_GROUP 2
#define NB_MAX_GROUP 10
#define FIRST_ITEM 1


int compare_reputation_workers (void * w_a, void * w_b);

double compute_LOC(xbt_dynar_t * w); 

void formGroup_fixed_fit(void);

void formGroup_first_fit(double target_LOC);

void formGroup_tight_fit(void);

void formGroup_random_fit(void);

void formGroup_Arantes(void);

#endif
