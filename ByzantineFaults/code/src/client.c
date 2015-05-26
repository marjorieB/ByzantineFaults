#include "msg/msg.h"
#include "task.h"
#include "client.h"

// this function permit to send to the node "mailbox" the task number i
void send_task(int i, char * mailbox) {
	msg_task_t task = NULL;
	char task_name[TASK_NAME_SIZE];

	sprintf(task_name, "task-%d", i);
	task = MSG_task_create (task_name, TASK_COMPUTE_DURATION, MESSAGE_SIZE, NULL);
	MSG_task_send(task, mailbox);
}


void receive_answer() {
	msg_task_t answer = NULL;
	_XBT_GNUC_UNUSED int res;

	res = MSG_task_receive(&(answer), MSG_process_get_name(MSG_process_self()));
   xbt_assert(res == MSG_OK, "MSG_task_receive failed on client");
}


void send_finalize(char * mailbox) {
	msg_task_t finalize = MSG_task_create("finalize", 0, 0, 0);
   MSG_task_send(finalize, mailbox);	
}


int client (int argc, char * argv[]) {
	int i;
	int nb_requests;
	char mailbox[MAILBOX_SIZE];

	if (argc != 1) {
		exit(1);
	}

	printf("ma boite aux lettres: %s\n", MSG_host_get_name(MSG_host_self()));

	// clients know the number of requests they must send
	nb_requests = atoi(argv[1]);
	sprintf(mailbox, "primary-1");

	for (i = 1; i <= nb_requests; i++) {
		//send a request to the primary
		send_task(i, mailbox);		

		// wait for the result of the request before sending an other one
		receive_answer();
	}
	
	// signal to the primary that there is no more tasks to treat for that client
	send_finalize(mailbox);

	return 0;
}
