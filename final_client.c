#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <iostream>



// LISTEN function for pthread, output things to console as they come into socket
void *listen(void *port_no){
	using namespace std;
    int client_sock;
	int known_port = *((int *) port_no); 
    struct sockaddr_in serv_addr;
	char host_name[256];
    char buffer[256];
   
    //Create client socket
    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock < 0) 
        cout << "ERROR opening socket" << endl;
	
	//Identify local system as host server
	gethostname(host_name, 256);
    struct hostent *server = gethostbyname(host_name);
   
	
	// Establish serv_addr elements
    serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(known_port);
	
	
	//Connect to server, constantly read and output messages from server
    if (connect(client_sock,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        cout << "ERROR connecting" << endl;
	
	
	int tmp, result;
	 while((tmp=read(client_sock,buffer, 100)) == -1){
       cout<<buffer<<endl;
    }
	
	// end reading, close socket
	close(client_sock);
   }







int main()
{
	using namespace std;
    int client_sock, n;
	int known_port = 5001;
    struct sockaddr_in serv_addr;
	char host_name[256];
	char command[256];
	int port_number, tmp;
    //Create client socket
    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock < 0) 
        cout << "ERROR opening socket" << endl;
	
	//Identify local system as host server
	gethostname(host_name, 256);
    struct hostent *server = gethostbyname(host_name);
   
	
	// Establish serv_addr elements
    serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(known_port);
	
	//Connect to server
    if (connect(client_sock,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        cout << "ERROR connecting" << endl;
	
	
	// Take in client command
	cout << "Chat initialized! Enter command CREATE [number], JOIN [number], or DELETE [number]" << endl;
	cin.getline(command, 100);   
    cin.clear();
    if((tmp = write(client_sock, command,100))==-1) {   
       cout<<"Write error"<<endl;
       exit(1);
    }					
		cout << "Command Sent. Waiting for reply ... " << endl;
		string command_word = strtok(command, " ");
		string number_str = strtok(NULL, " ");
	
	// Wait to get creation response
	if (command_word == "CREATE"){
    	tmp=read(client_sock,command, 100);    
		cout<< "\n Response received: " << command <<endl;   
		close(client_sock);
		return 0;
	}
	
	//Wait to get port number response
	else if (command_word == "JOIN"){
		tmp=read(client_sock,&port_number, sizeof(port_number));                     
		if (port_number == -1){
			cout << "Error: no chatroom found. " << endl;
			return 0;
		}
		else
			cout<< "\n Response received. Port Number: " << port_number  << endl;   
			
	}
	
	//Wait to get delete response
	else if (command_word == "DELETE"){
		tmp=read(client_sock,command, 100);    
    	cout<< "\n Response received: " << command << endl;                         
		close(client_sock);
		return 0;
	}
	
	// Error handling
	else {
		tmp=read(client_sock, command, 100);    
    	cout<< "\n Error received: " << command << endl;    
		return 0;
	}
	
	
	// If we're here, the command was a JOIN and we need to connect to the port number given by the server.
	// Close socket to master socket
    close(client_sock);
	
	pthread_t listen_id;
	pthread_create(&listen_id, NULL, listen, &port_number);
	// Open new socket to chatroom port
	client_sock = socket(AF_INET, SOCK_STREAM, 0);
	serv_addr.sin_port = htons(port_number);
	if (connect(client_sock,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
        cout << "ERROR connecting" << endl;
		return 0;
	}
	
	
	// Connected to chatroom, start chatting 
	
	// Create thread to output messages into console
	cout << "Chat initialized! Send your first message: ";
	while(true){
	while(cin.getline(command, 100)!=NULL) {    
    cin.clear();
    if((tmp = write(client_sock, command,100))==-1) {   
       cout<<"Write error"<<endl;
       exit(1);
    }					     
		cout << "Enter message : ";
   }
	}
    return 0;
	
}