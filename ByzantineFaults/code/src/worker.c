#include "msg/msg.h"
#include "worker.h"
#include "task.h"
#include <time.h>


void ask_to_join(char * mailbox, char * myMailbox) {
	msg_task_t join = MSG_task_create("joining", 0, 0, myMailbox);
	MSG_task_send(join, mailbox);
}


void receive_ack(struct worker * worker, char * myMailbox) {
	_XBT_GNUC_UNUSED int res;
	msg_task_t ack = NULL;

	res = MSG_task_receive(&(ack), myMailbox);
	xbt_assert(res == MSG_OK, "MSG_task_receive failed on worker");

	strcpy(worker->primary, (char *)MSG_task_get_data(ack));

	MSG_task_destroy(ack);
}


void treat_task_worker(struct worker * me, msg_task_t task, char * myMailbox) {
	msg_task_t answer;	
	struct w_task data_toSend;
		
	MSG_task_execute(task);
	strcpy(data_toSend.client, MSG_task_get_data(task));
	strcpy(data_toSend.worker_name, myMailbox);
	strcpy(data_toSend.task_name, MSG_task_get_name(task));

	if ((rand() % 100) <= me->reputation) {
		data_toSend.bool_answer = GOOD_ANSWER;
	}
	else {
		data_toSend.bool_answer = BAD_ANSWER;
	} 	
	
	printf("%s: I send the answer of %s %s to %s\n", myMailbox, data_toSend.task_name, data_toSend.client, me->primary);
	answer = MSG_task_create("answer", ANSWER_COMPUTE_DURATION, ANSWER_MESSAGE_SIZE, &data_toSend);
	MSG_task_send(answer, me->primary);
}


int worker (int argc, char * argv[]) {
	unsigned long int id;
	char myMailbox[MAILBOX_SIZE];
	struct worker * me = (struct worker *) malloc(sizeof(struct worker));
	char primary[MAILBOX_SIZE];
	
	if (argc != 4) {
		exit(1);
	}

	// the worker ask to join the system, then wait for an acknowledgement from the primary and then wait for request to treat
	id = atoi(argv[1]);	
	sprintf(myMailbox, "worker-%ld", id);
	strcpy(primary, argv[2]);
	me->reputation = atoi(argv[3]);

	printf("%s: ask to join the system to %s\n", myMailbox, primary);
	ask_to_join(primary, myMailbox);

	receive_ack(me, myMailbox);

	srand(time(NULL));

	while (1) {
		_XBT_GNUC_UNUSED int res;
		msg_task_t task = NULL;

		res = MSG_task_receive(&(task), myMailbox);
		xbt_assert(res == MSG_OK, "MSG_task_receive failed on worker");

		if (!strcmp(MSG_task_get_name(task), "finalize")) {
			printf("%s: I receive finalize\n", myMailbox);
			MSG_task_destroy(task);
			task = NULL;
			break;
		}
		else if (!strncmp(MSG_task_get_name(task), "task", strlen("task") * sizeof(char))) {
			// if the availability_file still don't work, do a random to know if the worker will answer and put a percentage of availability in the xml file to describe the node
			treat_task_worker(me, task, myMailbox);
			MSG_task_destroy(task);
			task = NULL;
		}	
		else {
			// message incorrect
			printf("%s message incorrect\n", myMailbox);
			MSG_task_destroy(task);
			task = NULL;
		}
	}	

	return 0;
}









