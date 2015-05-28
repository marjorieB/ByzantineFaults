#include "msg/msg.h"
#include "reputation_strategy.h"


void updateReputation_Sonnek (struct p_worker * worker) {
	worker->reputation = (int)((((double)worker->totC + 1.0) / ((double)worker->totR + 2.0)) * 100.0);
}



