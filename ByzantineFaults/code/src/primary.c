#include "msg/msg.h"
#include "task.h"
#include "primary.h"
#include <string.h>


void send_finalize_to_workers(int nb_workers) {
	char mailbox[MAILBOX_SIZE];
	long int i;

	for (i = 0; i < nb_workers; i++) {
		msg_task_t finalize = MSG_task_create("finalize", 0, 0, 0);	
		sprintf(mailbox, "worker-%ld", i);
		MSG_task_send(finalize, mailbox);
	}
}


int primary (int argc, char * argv[]) {
	int nb_clients;
	int nb_workers;	
	int nb_finalize = 0;
	_XBT_GNUC_UNUSED int res;
	msg_task_t task_todo;

	if (argc != 2) {
		exit(1);
	}

	nb_clients = atoi(argv[0]);
	nb_workers = atoi(argv[1]);

	while (1) {
		/* the primary can receive 3 types of messages: requests from client, 
			finalization from client or answer to tasks of workers */
		res = MSG_task_receive(&(task_todo), MSG_process_get_name(MSG_process_self()));
		xbt_assert(res == MSG_OK, "MSG_task_receive failed on primary");

		if (!strcmp(MSG_task_get_name(task_todo), "finalize")) {
			MSG_task_destroy(task_todo);
			nb_finalize++;
			if (nb_finalize == nb_clients) {
				// if all clients have finish to send requests, the primary ask the workers to stop
				send_finalize_to_workers(nb_workers);
			}
			break;
		}
		else if (!strncmp(MSG_task_get_name(task_todo), "task", sizeof(char) * strlen("task"))) {
			// the primary just receive a task to execute
			
		}
		else if (!strncmp(MSG_task_get_name(task_todo), "answer", sizeof(char) * strlen("answer"))) {
			// the primary receive an answer to a request from a worker

		}
		else {
			// messages incorrect
		}
	}

	return 0;
}

























