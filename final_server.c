/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <vector>
#include <pthread.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}


struct ChatRoom{
	int port_no;
	int clients;
	int name;
	pthread_t thread_id;
	ChatRoom(int port, int _name){
		port_no = port;
		name = _name;
		clients = 0;
	}
};


void *chat_server(void *port_no){
	// CHAT SERVER CODE
	
	using namespace std;
     int master_sock, newsockfd;
	 int no_clients = 0;
	 int portno = *((int *) port_no); 
	 int* new_sock;
   	
	
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
    
     
	// Create master socket
     master_sock = socket(AF_INET, SOCK_STREAM, 0);
     if (master_sock < 0) 
        error("ERROR opening socket");
	
	 // Initialize serv_addr elements
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
	
	 // Bind master socket
     if (bind(master_sock, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("Binding Failed.");
	
	 // Listen for connections
	 cout << "Listening for connections ...  " << endl;
     listen(master_sock,5);
	
	 // Accept connections
	 socklen_t size = sizeof(cli_addr);
     newsockfd = accept(master_sock, 
                 (struct sockaddr *) &cli_addr, 
                 &size);
     if (newsockfd < 0) 
          error("ERROR on accept");
	
     bzero(buffer,256);

        //strcpy(buffer, "Server connected...\n");
       // send(newsockfd, buffer, 256, 0);
       
	int nbytes, pid;
        cout << "Chat initialized on port " << portno << endl;
	
	//Accept all connections coming in. When a connection is accepted, a fork call goes out to handle the receiving of that connection's data
	fd_set fds;
	int rc, result;
	
	while(newsockfd = accept(master_sock, (struct sockaddr *) &cli_addr, &size)){
		if ((pid = fork()) == -1)
        {
            close(newsockfd);
            continue;
        }
		else if(pid > 0)
        {
            close(newsockfd);
            no_clients++;
        }
		else if (pid == 0){
			//no_clients++;
			cout << "The number of clients on this server is " << no_clients/2 << endl;
			while((nbytes=read(newsockfd,buffer, 100))!=-1) {    
			cout<< "\n Message received: " << buffer <<endl;                         
			write(newsockfd,buffer, 100);
		}
		}
	}
     close(newsockfd);
     close(master_sock);
	 
}



// STUFF TO DO! 
// Send out closing message to everyone on chat when DELETE command called
// Close chat room when DELETE is called
// Return info on chat room when JOIN called

int main()
{
	 using namespace std;
     int master_sock, newsockfd;
	 int portno = 5001; 
	 int current_port = 5002;
	 int* current_port_ptr = &current_port;
	
   	 string command_word;
	 char command[256];
	 int number;
     struct sockaddr_in serv_addr, cli_addr;
     string number_str;
	 vector<ChatRoom*> local_database;
     
	// Create master socket
     master_sock = socket(AF_INET, SOCK_STREAM, 0);
     if (master_sock < 0) 
        error("ERROR opening socket");
	
	 // Initialize serv_addr elements
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
	
	 // Bind master socket
     if (bind(master_sock, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("Binding Failed.");
	
	 // Listen for connections
	 cout << "Listening for connections ...  " << endl;
     listen(master_sock,5);
	
	 // Accept connections
	 socklen_t size = sizeof(cli_addr);
	
	
	
	// Handle client commands constantly
	
	while (true) {
	 cout << "waiting for client ... " << endl;
     newsockfd = accept(master_sock, 
                 (struct sockaddr *) &cli_addr, 
                 &size);
     if (newsockfd < 0) 
          error("ERROR on accept");
	
     bzero(command,256);

       // Receive and decode command message from client
		int nbytes;
        cout << "Link initialized! Waiting for client command ... " << endl;
	    nbytes=read(newsockfd,command, 100);    // get command from stdin
		cout<< "\n Message received: " << command <<endl;                         // print message to stdout
		command_word = strtok(command, " ");
		number_str = strtok(NULL, " ");

		// We have instructions, now do stuff.
			// Some random variables to help with mumbo jumbo
			number = stoi(number_str);
			int port_number = -1;
	
		
		
		/* Check if chatroom exists
				if so, return error message
		   Create chatroom entry in local database
		   Instantiate chatroom server thread 
		   Return notification
	    */
		if (command_word == "CREATE"){
			// Check if chatroom exists
			bool alreadyExists = false;
			for (int i = 0; i < local_database.size(); i++){
				if (local_database[i] != NULL){
					if (local_database[i]->name == number)
						alreadyExists = true;
					}
				}
				
			if (alreadyExists == false){
				cout << "Creating chatroom ... " << endl;
				// Create chatroom struct, push to local database
				local_database.push_back(new ChatRoom(*current_port_ptr, number));
				// Tell client it happened.
				strcpy(command, "Chatroom created with name specified." ); 
				pthread_create(&(local_database.back()->thread_id), NULL, chat_server, &current_port);
				*current_port_ptr++;
				}
			else
				strcpy(command, "Chatroom already exists." ); 
				nbytes = write(newsockfd, command,100);
		}
	
		
		
		/* Check if chatroom exists
				if so, return port number
				if not, return -1. Client can handle this return value
		  
	    */
		
		else if(command_word == "JOIN"){
			for (int i = 0; i < local_database.size(); i++){
				if (local_database[i] != NULL){
					if (local_database[i]->name == number){
						port_number = local_database[i]->port_no;
					}
				}
			}
			cout << "Returning port number for chat room: " << port_number << endl;
			nbytes = write(newsockfd, &port_number, sizeof(port_number));
		}
	
		
		
		/* Check if chatroom exists
				if not, return error notification
		   If so, kill pthread handling server processes
		   Delete chatroom entry from local database
	    */
		
		else if(command_word == "DELETE"){
			bool exists = false;
			// Send warning message to all peoples in the chat
			// Close socket, delete entry.
			for (int i = 0; i < local_database.size(); i++){
				if (local_database[i] != NULL){
					if (local_database[i]->name == number){
						pthread_cancel(local_database[i]->thread_id);
						*current_port_ptr--;
						local_database.erase(local_database.begin() + i);
						exists = true;
					}
				}
				
			// Entry is deleted, port_number holds port of the socket we want to close.
		}
			if (exists == true){
			strcpy(command, "Chatroom deleted with name specified." );  
			nbytes = write(newsockfd, command,100);
			}
			else {
			strcpy(command, "Chatroom specified does not exist." );  
			nbytes = write(newsockfd, command,100);
			}
		}
		
		
		// Input was not CREATE, JOIN, or DELETE command
		else {
			strcpy(command, "Command not recognized. Commands are: CREATE [number], JOIN [number], DELETE [number]" );  
			nbytes = write(newsockfd, command,100);
			}
		
	}
     close(newsockfd);
     close(master_sock);
	 
     return 0; 
}