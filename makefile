# makefile

all: csrd crc

csrd: final_server.c
	g++ -std=c++11 -pthread -o csrd -g final_server.c 

crc: final_client.c
	g++ -std=c++11 -pthread -o crc final_client.c

clean:
	rm csrd crc