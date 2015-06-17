#ifndef _REPUTATION_STRATEGY
#define _REPUTATION_STRATEGY

#include "primary.h"

#define MAX_REPUTATION 100
#define MIN_REPUTATION 0
#define INITIAL_VALUE 50

void updateReputation_Sonnek (struct p_worker * worker);

void updateReputation_Symmetrical (struct p_worker * worker, char good_or_bad_answer);

void updateReputation_Asymmetrical (struct p_worker * worker, char good_or_bad_answer);

void updateReputation_BOINC (struct p_worker * worker);

#endif
