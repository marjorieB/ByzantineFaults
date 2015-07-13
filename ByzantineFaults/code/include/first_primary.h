#ifndef _FIRST_PRIMARY_H
#define _FIRST_PRIMARY_H

#include "task.h"
#include "msg/msg.h"

struct primary {
	char name[MAILBOX_SIZE];
	char max_reputation;
	char min_reputation;
};


// this global variable contains all the active primaries of the system. This array contains struct primaries
xbt_dynar_t active_primaries;
// this global variable contains all the inactive primaries of the system. This array contains struct primaries
xbt_dynar_t inactive_primaries;


void send_finalize_to_primaries (void);

void send_task_random(msg_task_t task);

void send_task_reputations(msg_task_t task);

int first_primary (int argc, char * argv[]);

#endif
