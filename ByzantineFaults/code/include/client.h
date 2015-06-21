#ifndef _CLIENT_H
#define _CLIENT_H

#define NB_REQUESTS 1

void send_task(int i, char * mailbox, char * myMailbox);

void receive_answer(char * myMailbox);

void send_finalize(char * mailbox);

int client (int argc, char * argv[]);

#endif
