#include "msg/msg.h"
#include "task.h"
#include "client.h"
#include <time.h>

// this function permit to send to the node "mailbox" the task number i
void send_task(int i, char * mailbox, char * myMailbox) {
	msg_task_t task = NULL;
	char task_name[TASK_NAME_SIZE];
	//struct clientDataTask data;

	printf("%s: I send a request to %s\n", myMailbox, mailbox);

	sprintf(task_name, "task-%d", i);
	//strcpy(data.mailbox, myMailbox); 

	/*if (i == 0) {
		data.targetLOC = 0.2;	
	}
	else {
		data.targetLOC = 0.65;
	}*/
	//task = MSG_task_create (task_name, TASK_COMPUTE_DURATION, TASK_MESSAGE_SIZE, &data);
	task = MSG_task_create (task_name, TASK_COMPUTE_DURATION, TASK_MESSAGE_SIZE, myMailbox);
	MSG_task_send(task, mailbox);
}


void receive_answer(char * myMailbox) {
	msg_task_t answer = NULL;
	_XBT_GNUC_UNUSED int res;

	res = MSG_task_receive(&(answer), myMailbox);
	xbt_assert(res == MSG_OK, "MSG_task_receive failed on client");

	if (!strcmp(MSG_task_get_name(answer), "answer")) {
   	printf("%s: I receive the answer to %s\n", myMailbox, (char *)MSG_task_get_data(answer));
	}
	else if (!strcmp(MSG_task_get_name(answer), "fail")) {
		printf("%s: the primary failed having a majority of answer from the workers\n", myMailbox);
	}
	else {
		printf("%s, message incorrect\n", myMailbox);
	}

	MSG_task_destroy(answer);	
}


void send_finalize(char * mailbox) {
	msg_task_t finalize = MSG_task_create("finalize", 0, 0, NULL);
   MSG_task_send(finalize, mailbox);	
}


int client (int argc, char * argv[]) {
	int i;
	unsigned long int id;
	int nb_requests;
	char myMailbox[MAILBOX_SIZE];
	char primary[MAILBOX_SIZE];

	if (argc != 3) {
		exit(1);
	}

	nb_requests = NB_REQUESTS;

	id = atoi(argv[1]);
	sprintf(myMailbox, "client-%ld", id);
	// the name of the primary is known by the client
	strcpy(primary, argv[2]);

	for (i = 0; i < nb_requests; i++) {
		//send a request to the primary
		send_task(i, primary, myMailbox);		

		// wait for the result of the request before sending an other one
		receive_answer(myMailbox);
	}
	
	// signal to the primary that there is no more tasks to treat for that client
	printf("%s: I send finalize\n", myMailbox);
	send_finalize(primary);

	return 0;
}
