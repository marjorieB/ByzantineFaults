all: fromMySQLToText

fromMySQLToText: fromMySQLToText.o
	gcc -o fromMySQLToText fromMySQLToText.o `mysql_config --libs`

fromMySQLToText.o:
	gcc `mysql_config --cflags` -lmysqlclient -o fromMySQLToText.o -c src/fromMySQLToText.c 


clean: 
	rm -f *.o fromMySQLToText 
