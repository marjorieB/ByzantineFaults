#ifndef _GROUP_FORMATION_STRATEGY_H
#define _GROUP_FORMATION_STRATEGY_H


#define NB_FIXED_GROUP 2
#define NB_MIN_GROUP 2
#define NB_MAX_GROUP 10
#define FIRST_ITEM 1


long int factorial (int n);

double combination (int a, int b);

int compare_reputation_workers (const void * w_a, const void * w_b);

double compute_LOC(xbt_dynar_t * w); 

void formGroup_fixed_fit(void);

void formGroup_first_fit(double target_LOC);

void formGroup_tight_fit(void);

void formGroup_random_fit(double target_LOC);

void formGroup_Arantes(void);

#endif
