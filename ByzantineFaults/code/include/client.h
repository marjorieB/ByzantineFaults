#ifndef _CLIENT_H
#define _CLIENT_H

void send_task(int i, char * mailbox);

void receive_answer(void);

void send_finalize(char * mailbox);

int client (int argc, char * argv[]);

#endif
