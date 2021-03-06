#ifndef _GROUP_FORMATION_STRATEGY_H
#define _GROUP_FORMATION_STRATEGY_H

#define FIRST_ITEM 0

long int factorial (int n);

double combination (int a, int b);

int compare_reputation_workers (const void * w_a, const void * w_b);

int compare_reputation_workers_inverse (const void * w_a, const void * w_b);

double compute_LOC(xbt_dynar_t * w); 

char formGroup_fixed_fit(msg_task_t * task_to_treat, int id);

char formGroup_first_fit_Sonnek(msg_task_t * task_to_treat, int id);

void binary_search_pairs(xbt_dynar_t * w, double * LOC, int index, double target_LOC, int id);

char formGroup_tight_fit_Sonnek(msg_task_t * task_to_treat, int id);

char formGroup_random_fit_Sonnek(msg_task_t * task_to_treat, int id);

double compute_Arantes_PC(xbt_dynar_t * w);

double compute_Arantes_PB(xbt_dynar_t * w);

char formGroup_first_fit_Arantes(msg_task_t * task_to_treat, int id);

char formGroup_tight_fit_Arantes(msg_task_t * task_to_treat, int id);

void binary_search_one(xbt_dynar_t * w, double * res, int index, double target_LOC, int id);

char formGroup_random_fit_Arantes(msg_task_t * task_to_treat, int id);

#endif
