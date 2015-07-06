#include <mysql.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FILE_NAME_SIZE 256
#define REQUEST_SIZE 256


const char* server = "127.0.0.1";
const char* user = "marjo";
const char* password = "marjo";
char database[FILE_NAME_SIZE];

//char separator[1] = ";";
double end_line = -1.0; 


void write_starting_time(int fd) {
	MYSQL * conn;
   MYSQL_RES * result;
   MYSQL_ROW row;
	char request[REQUEST_SIZE];

	conn = mysql_init(NULL);

	/* Connect to database */
   if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      exit(1);
   }

	strcpy(request, "SELECT MIN(event_start_time) from event_trace where event_type = 1");
	if (mysql_query(conn, request)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      exit(1);
   }

   result = mysql_use_result(conn);
	row = mysql_fetch_row(result);

	double start_time = atof(row[0]);

	write(fd, &start_time, sizeof(double));
	//write(fd, separator, sizeof(char));

	mysql_free_result(result); 
	mysql_close(conn);
}


void find_previous_node_id (unsigned long int * previous_node_id) {
	MYSQL * conn;
   MYSQL_RES * result;
   MYSQL_ROW row;
	char request[REQUEST_SIZE];

	conn = mysql_init(NULL);

	/* Connect to database */
   if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      exit(1);
   }

	// find the identity of the first node in the list
	sprintf(request, "SELECT min(node_id) from event_trace\n");

	if (mysql_query(conn, request)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      exit(1);
   }

   result = mysql_use_result(conn);
	while ((row = mysql_fetch_row(result)) != NULL) {
		*previous_node_id = atoi(row[0]);
	}

	mysql_free_result(result); 
	mysql_close(conn);
}


void write_presence_or_crash_times(int fd, int previous_node_id) {
	MYSQL * conn;
   MYSQL_RES * result;
   MYSQL_ROW row;
	char request[REQUEST_SIZE];

	unsigned char event_type;
	double time_event;

	conn = mysql_init(NULL);

	/* Connect to database */
   if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      exit(1);
   }
		
	// find and write all the times of presence and crash of each workers
	sprintf(request, "SELECT node_id,event_start_time,event_type from event_trace order by node_id, event_start_time\n");

	if (mysql_query(conn, request)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      exit(1);
   }

   result = mysql_use_result(conn);
	while ((row = mysql_fetch_row(result)) != NULL) {
		if (previous_node_id == atoi(row[0])) {
			time_event = atof(row[1]);
			event_type = atoi(row[2]);
			write(fd, &time_event, sizeof(double));
			//write(fd, separator, sizeof(char));s
			write(fd, &event_type, sizeof(unsigned char));
			//write(fd, separator, sizeof(char));
		}
		else {
			previous_node_id = atoi(row[0]);
			time_event = atof(row[1]);
			event_type = atoi(row[2]);
			write(fd, &end_line, sizeof(double));			
			write(fd, &time_event, sizeof(double));
			//write(fd, separator, sizeof(char));
			write(fd, &event_type, sizeof(unsigned char));
			//write(fd, separator, sizeof(char));
		}
	}	

	mysql_free_result(result); 
	mysql_close(conn);
}


int main (int argc, char ** argv) {
	char file_name[FILE_NAME_SIZE];
	unsigned long int previous_node_id;
	int fd;

	if (argc != 2) {
		printf("you have to enter the name of the database you want to convert\n");
		exit(1);
	} 
	strcpy(database, argv[1]);	

	// writting the informations in a file
	strcpy(file_name, "database_file/boinc_file");

	if ((fd = open(file_name, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR)) == -1) {
		printf("error open\n");
		exit(1);
	}

	write_starting_time(fd);

	find_previous_node_id(&previous_node_id);

	write_presence_or_crash_times(fd, previous_node_id);
}


     
