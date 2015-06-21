#include "msg/msg.h"
#include "worker.h"
#include "task.h"
#include <mysql.h>
#include "simulator.h"
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


//this function returns the time the process has to wait before entering in the system
double enter_the_system (struct worker * me) {
	MYSQL * conn;
   MYSQL_RES * result;
   MYSQL_ROW row;
	double toRet;

   const char* server = "127.0.0.1";
   const char* user = "marjo";
   const char* password = "marjo"; /* set me first */
   const char* database = "test";
	char request[REQUEST_SIZE];

   conn = mysql_init(NULL);

	/* Connect to database */
   if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      exit(1);
   }

	// first we check if the node is present at time where it receives the request and during all the time of the execution of the request
	sprintf(request, "SELECT event_start_time from event_trace where node_id=%ld and event_type=1 LIMIT 1\n", me->id);

	if (mysql_query(conn, request)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      exit(1);
   }

   result = mysql_use_result(conn);
	if ((row = mysql_fetch_row(result)) == NULL) {
		// the node is never on
		printf("there is no result\n");
		return -1;
	}
	toRet = atof(row[0]) - time_start;
	mysql_free_result(result);

	printf("value I need to wait before having the right to enter the system %f\n", toRet);
	
	return toRet;	
}


// this function returns 0 if the node is present in the system, otherwise this function returns the time where the node have to recover (ask the primary to enter in the system)
double present(struct worker * me, double duration_task) {
	MYSQL * conn;
   MYSQL_RES * result;
   MYSQL_ROW row;
	double previous_time_start_event;
	int previous_type_event;
	double time_crash = 0.0;
	double toRet = -1;
	char crash = 0;

   const char* server = "127.0.0.1";
   const char* user = "marjo";
   const char* password = "marjo"; /* set me first */
   const char* database = "test";
	char request[REQUEST_SIZE];

   conn = mysql_init(NULL);

	/* Connect to database */
   if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      exit(1);
   }

	// first we check if the node is present at time where it receives the request and during all the time of the execution of the request
	sprintf(request, "SELECT event_type, event_start_time from event_trace where node_id=%ld\n", me->id);

	if (mysql_query(conn, request)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      exit(1);
   }

   result = mysql_use_result(conn);
	while ((row = mysql_fetch_row(result)) != NULL) {
		if ((atof(row[1]) >= time_start + MSG_get_clock())) {
			if (atof(row[1]) == time_start + MSG_get_clock()) {
				// the time in the trace corresponds to the time in the simulation, we have to check if the node is present at this time
				if (atoi(row[0]) == 0) {
					if (crash != 1) {
						crash = 1;
						time_crash = atof(row[1]);
					}
				}
				else {
					if (previous_type_event == 0) {
						if (crash != 1) {
							crash = 1;
							time_crash = previous_time_start_event;
						}
					}
				}
			}
			if (atof(row[1]) > time_start + MSG_get_clock() + duration_task) {
				break;
			}
		}
		previous_time_start_event = atof(row[1]);
		previous_type_event = atoi(row[0]);
	}

	if (crash == 0) {
		mysql_free_result(result);
   	mysql_close(conn);
		return 0.0;
	}


	sprintf(request, "SELECT event_start_time from event_trace where node_id=%ld and event_start_time > %f LIMIT 1\n", me->id, time_crash);

	if (mysql_query(conn, request)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      exit(1);
   }

	result = mysql_use_result(conn);
	if ((row = mysql_fetch_row(result)) != NULL) {
		toRet = atof(row[0]) - (MSG_get_clock() + time_start);
	}
	
	mysql_free_result(result); 
	mysql_close(conn);
	return toRet;
}


double treat_task_worker(struct worker * me, msg_task_t task, char * myMailbox) {
	msg_task_t answer;	
	struct w_task * data_toSend = (struct w_task *) malloc(sizeof(struct w_task));
	double time_to_wait;

	if ((time_to_wait = present(me, MSG_task_get_compute_duration(task))) == 0.0) {	
		if ((rand() % 100) <= me->reputation) {
			data_toSend->answer = GOOD_ANSWER;
		}
		else {
			data_toSend->answer = rand() % BAD_ANSWER;
		} 
		MSG_task_execute(task);	
	}	
	
	strcpy(data_toSend->client, MSG_task_get_data(task));
	strcpy(data_toSend->worker_name, myMailbox);
	strcpy(data_toSend->task_name, MSG_task_get_name(task));

	if (time_to_wait == 0.0) {
		answer = MSG_task_create("answer", ANSWER_COMPUTE_DURATION, ANSWER_MESSAGE_SIZE, data_toSend);
	}
	else if (time_to_wait > 0.0) {
		printf("worker %ld send crash\n", me->id);
		answer = MSG_task_create("crash", 0, 0, data_toSend);
	}
	else {
		printf("error in the function \"present\"\n");
	}

	MSG_task_send(answer, me->primary);

	return time_to_wait;
}


int worker (int argc, char * argv[]) {
	char myMailbox[MAILBOX_SIZE];
	struct worker * me = (struct worker *) malloc(sizeof(struct worker));
	char primary[MAILBOX_SIZE];
	double time_to_wait;
	
	if (argc != 4) {
		exit(1);
	}

	srand(time(NULL) + me->id + MSG_get_clock());

	// the worker ask to join the system, then wait for an acknowledgement from the primary and then wait for request to treat
	me->id = atoi(argv[1]);	
	sprintf(myMailbox, "worker-%ld", me->id);
	strcpy(primary, argv[2]);
	me->reputation = atoi(argv[3]);


	// wait the time indicated in the trace to enter the system	
	MSG_process_sleep(enter_the_system(me));

	printf("%s: ask to join the system to %s\n", myMailbox, primary);
	ask_to_join(primary, myMailbox);
	receive_ack(me, myMailbox);

	while (1) {
		_XBT_GNUC_UNUSED int res;
		msg_task_t task = NULL;

		res = MSG_task_receive(&(task), myMailbox);
		xbt_assert(res == MSG_OK, "MSG_task_receive failed on worker");

		if (!strcmp(MSG_task_get_name(task), "ack")) {
			strcpy(me->primary, (char *)MSG_task_get_data(task));
			MSG_task_destroy(task);
			task = NULL;
		}
		else if (!strcmp(MSG_task_get_name(task), "finalize")) {
			printf("%s: I receive finalize %f\n", myMailbox, MSG_get_clock());
			MSG_task_destroy(task);
			task = NULL;
			break;
		}
		else if (!strncmp(MSG_task_get_name(task), "task", strlen("task") * sizeof(char))) {
			// if the availability_file still don't work, do a random to know if the worker will answer and put a percentage of availability in the xml file to describe the node
			time_to_wait = treat_task_worker(me, task, myMailbox);
			MSG_task_destroy(task);
			task = NULL;
			if (time_to_wait == -1) {
				printf("error in function \"present\"\n");
			}
			else if (time_to_wait != 0.0) {
				// wait until the time where the node recover
				MSG_process_sleep(time_to_wait);
				ask_to_join(primary, myMailbox);
				receive_ack(me, myMailbox);
			}
		}	
		else {
			// message incorrect
			printf("%s message incorrect\n", myMailbox);
			MSG_task_destroy(task);
			task = NULL;
		}
	}	
	
	free(me);

	return 0;
}









