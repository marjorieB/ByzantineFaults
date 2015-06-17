#ifndef _GROUP_FORMATION_STRATEGY_H
#define _GROUP_FORMATION_STRATEGY_H

#define FIRST_ITEM 0

long int factorial (int n);

double combination (int a, int b);

int compare_reputation_workers (const void * w_a, const void * w_b);

double compute_LOC(xbt_dynar_t * w); 

void formGroup_fixed_fit(void);

void formGroup_first_fit(void);

void formGroup_first_fit_Sonnek(void);

void binary_search_pairs(xbt_dynar_t * w, double * LOC, int index);

void formGroup_tight_fit_Sonnek(void);

void formGroup_random_fit(void);

void formGroup_random_fit_Sonnek(void);

double compute_Arantes_PC(xbt_dynar_t * w);

double compute_Arantes_PB(xbt_dynar_t * w);

void formGroup_first_fit_Arantes(void);

void formGroup_tight_fit_Arantes(void);

void binary_search_one(xbt_dynar_t * w, double * res, int index);

void formGroup_random_fit_Arantes(void);

#endif
