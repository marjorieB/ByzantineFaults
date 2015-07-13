#ifndef REPLICATION_STRATEGY_H
#define REPLICATION_STRATEGY_H

#define REPLICATION_MAX 16

#include "primary.h"
#include "msg/msg.h"

#include "additional_replication_strategy.h"
#include "group_formation_strategy.h"
#include "simulator.h"
#include "primary.h"
#include <math.h>


int verify (struct p_task * p_t, struct p_worker w);

void find_workers_fixed_fit (struct p_task * p_t, xbt_dynar_t * array_tmp, int * nb_replications, int id);

double valueCond2_replication (struct p_task * p_t);

void replication_fixed_fit (struct p_task * p_t, int id);

int find_workers_first_fit (struct p_task * p_t, xbt_dynar_t * array_tmp, int id);

double binary_search_one_replication(struct p_task * p_t, xbt_dynar_t * array_tmp, int index, int id);

int find_workers_tight_fit (struct p_task * p_t, xbt_dynar_t * array_tmp, int id);

int find_workers_random_fit (struct p_task * p_t, xbt_dynar_t * array_tmp, int id);

void replication_others_fit (struct p_task * p_t, int id);

#endif
