#include "msg/msg.h"
#include "task.h"
#include "client.h"
#include "simulator.h"
#include "primary.h"
#include <time.h>

// this function permit to send to the node "mailbox" the task number i
void send_task(int i, char * mailbox, char * myMailbox) {
	msg_task_t task = NULL;
	char task_name[TASK_NAME_SIZE];
	struct clientDataTask * data = (struct clientDataTask *) malloc(sizeof(struct clientDataTask));

	//printf("%s: I send a request to %s\n", myMailbox, mailbox);

	sprintf(task_name, "task-%d", i);
	strcpy(data->mailbox, myMailbox); 

	if (random_target_LOC == NOT_RANDOM) {
		if (simulator == ARANTES) {
			data->target_LOC = value_target_LOC_stationary;
			data->rangeReputationPrimaryToRequest = 0.1;
		}
		else {
			data->target_LOC = value_target_LOC_stationary;
			data->rangeReputationPrimaryToRequest = 0.90;
		}
	}
	else {
		if (simulator == ARANTES) {
			data->target_LOC = (double)(rand() % 51) / 100.0;
			data->rangeReputationPrimaryToRequest = 0.1;
		}
		else {
			data->target_LOC = (50.0 + ((double)(rand() % 51))) / 100.0;
			data->rangeReputationPrimaryToRequest = 0.1;
		}
	}

	data->start_time = MSG_get_clock();
	task = MSG_task_create (task_name, task_compute_duration, task_message_size, data);
	MSG_task_send(task, mailbox);
}


void receive_answer(char * myMailbox) {
	msg_task_t answer = NULL;
	_XBT_GNUC_UNUSED int res;

	res = MSG_task_receive(&(answer), myMailbox);
	xbt_assert(res == MSG_OK, "MSG_task_receive failed on client");

	if (!strcmp(MSG_task_get_name(answer), "answer")) {
   	//printf("%s: I receive the answer to %s\n", myMailbox, (char *)MSG_task_get_data(answer));
	}
	else if (!strcmp(MSG_task_get_name(answer), "fail")) {
		//printf("%s: the primary failed having a majority of answer from the workers\n", myMailbox);
	}
	else {
		//printf("%s, message incorrect\n", myMailbox);
	}

	MSG_task_destroy(answer);	
}


void send_finalize(char * mailbox) {
	msg_task_t finalize = MSG_task_create("finalize", 0, 0, NULL);
   MSG_task_send(finalize, mailbox);	
}


int client (int argc, char * argv[]) {
	//printf("starting client\n");
	int i;
	unsigned long int id;
	char myMailbox[MAILBOX_SIZE];
	char primary[MAILBOX_SIZE]; // when we use the decentrlized solution, the client send its request to the first-primary. (However the code is the same, we just change in the xml file the identity of the primary by the identity of the first-primary)

	if (argc != 3) {
		//printf("here in the client\n");
		exit(1);
	}

	id = atoi(argv[1]);
	sprintf(myMailbox, "client-%ld", id);
	// the name of the primary is known by the client
	strcpy(primary, argv[2]);

	srand(time(NULL) * id + MSG_get_clock());

	for (i = 0; i < nb_requests; i++) {
		MSG_process_sleep(((double)(rand () % 1000001)) / 1000.0);
		
		//send a request to the primary
		send_task(i, primary, myMailbox);		

		// wait for the result of the request before sending an other one
		receive_answer(myMailbox);
	}
	
	// signal to the primary that there is no more tasks to treat for that client
	//printf("%s: I send finalize\n", myMailbox);
	send_finalize(primary);

	return 0;
}
