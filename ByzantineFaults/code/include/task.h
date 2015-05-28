#ifndef _TASK_H
#define _TASK_H

#define MAILBOX_SIZE 256
#define TASK_NAME_SIZE 256
#define TASK_COMPUTE_DURATION 50000000
#define TASK_MESSAGE_SIZE 1000000
#define ANSWER_COMPUTE_DURATION 5000
#define ANSWER_MESSAGE_SIZE 10000
#define ACK_COMPUTE_DURATION 100
#define ACK_MESSAGE_SIZE 10


struct task {
	char client[MAILBOX_SIZE];
	char task_name[TASK_NAME_SIZE];
	char worker[MAILBOX_SIZE];
	char bool_answer;  // if bool_answer equals -1 the answer isn't correct, otherwise bool_answer equals 1 and the answer is correct
};

#endif
