#include "msg/msg.h"
#include "primary.h"
#include "worker.h"
#include "client.h"

int main (int argc, char * argv[]) {
	msg_error_t res = MSG_OK;

	MSG_init(&argc, argv);

	MSG_function_register("client", client);
	MSG_function_register("primary", primary);
	//MSG_function_register("worker", worker);

	MSG_create_environment("../platforms/plat_finalize.xml");

	

	printf("lancement\n");
	res = MSG_main();
	printf("après lancemen\n");

MSG_launch_application("../platforms/dep_finalize.xml");


	if (res == MSG_OK) {
		return 0;
	}
	else {
		return 1;
	}
}
