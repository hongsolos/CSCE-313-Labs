/* 
File: requestchannel.C

Author: R. Bettati
Department of Computer Science
Texas A&M University
Date  : 2012/07/11

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <errno.h>

#include <cassert>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <iostream>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include "reqchannel.h"

using namespace std;

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR/DESTRUCTOR FOR CLASS   R e q u e s t C h a n n e l  */
/*--------------------------------------------------------------------------*/


//Create a NetworkRequestChannel based on the given socket
 NetworkRequestChannel::NetworkRequestChannel(int socket){
  sock = socket;
 }

//client
NetworkRequestChannel::NetworkRequestChannel(const string _server_host_name, const unsigned short _port_no) 
{ 
 // CLIENT
	struct addrinfo hints, *res;
	
	// Network Information
 memset(&hints, 0, sizeof hints);
 hints.ai_family = AF_INET;
 hints.ai_socktype = SOCK_STREAM;
   
 // Get Address Information 
 int status = 0;
 if ((status = getaddrinfo(_server_host_name.c_str(), (std::to_string(_port_no)).c_str(), &hints, &res)) != 0)
  fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
  
 // Create Socket
 printf("Client: Creating Socket...");
 sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
 if (sock < 0) perror ("Error creating socket\n");
 printf("DONE\n");
	printf("Client: Socket FD = %d\n", sock);
	
 // Connect
 printf("Client: Connecting...");
 if ( connect(sock, res->ai_addr, res->ai_addrlen) < 0 ) perror ("Connect Error: ");
 printf("DONE\n");
 

 

 
 /*
 struct sockaddr_in server; 

 // Create Socket
 printf("Creating client socket...");
 if( (sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
  printf("Client Socket Failed");
 }
 printf("done\n");
 
 // Network Information
 memset(&server, '0', sizeof(server)); 
 inet_pton(AF_INET, _server_host_name.c_str(), &(server.sin_addr));
 server.sin_family = AF_INET;
 server.sin_port = htons(_port_no);
 
 // Connect to Server
 printf("Connecting client to server...");
 if( connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0)
 {
  perror("Connect Failed");
 }
 printf("done");
 */
 
}

//Constructor used in Server Side to Create Channel with Specific Port Number and back log.
NetworkRequestChannel::NetworkRequestChannel(const unsigned short _port_no, void * (*connection_handler) (void*), int back_log)
{  
 // SERVER
 struct addrinfo hints, *serv, *p;
 int sockfd, new_fd;
 int status;
 int yes = 1;
 struct sigaction sa;
		
	// Network Information
 memset(&hints, 0, sizeof hints);
 hints.ai_family = AF_UNSPEC;
 hints.ai_socktype = SOCK_STREAM;
 hints.ai_flags = AI_PASSIVE; 
 
 
 // Get Address Information
 if ((status = getaddrinfo(NULL, (std::to_string(_port_no)).c_str(), &hints, &serv)) != 0) 
  fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
  
 // Create Socket
 printf("Server: Creating Socket...");
	if ((sockfd = socket(serv->ai_family, serv->ai_socktype, serv->ai_protocol)) < 0)
	{
  perror("Server: socket");
	}
 printf("DONE\n");
 printf("Server: Connection FD = %d\n", sockfd);
 
 // Bind Socket
 printf("Server: Binding...");
 if( bind(sockfd, serv->ai_addr, serv->ai_addrlen) < 0)
 {
  close(sockfd);
  perror("Server: bind");
  exit(1);
 }
 printf("DONE\n");
 
	
 freeaddrinfo(serv);
 
 printf("Server: Listening...");
 if( listen(sockfd, back_log) == -1 )
 {
  perror("Server: listen");
  exit(1);
 }
 printf("DONE\n");
 
 // Reap Dead Processes
 sa.sa_handler = sigchld_handler; // reap all dead processes
 sigemptyset(&sa.sa_mask);
 sa.sa_flags = SA_RESTART;
 if (sigaction(SIGCHLD, &sa, NULL) == -1) {
  perror("sigaction");
  exit(1);
 } 



 while(1)
 {
  continue;
  new_fd = accept(sock, (struct sockaddr*)NULL, NULL);
  if (new_fd == -1) {
   perror("accept");
   continue;
  }
  printf("Server: Connected with FD = %d", new_fd);
  
  
  if(!fork())
  {
   close(sockfd);
   sock = new_fd;
   cread();
   close(new_fd);
  }
  
  close(new_fd);
  
 }
 
 /*
 int socket_desc, client_sock, *new_sock;
 struct sockaddr_in server; 

 // Create Socket
 printf("Creating server socket...");
 if( (socket_desc = socket(AF_INET, SOCK_STREAM, 0)) < 0)
 {
  printf("Server Socket Failed");
 }
 printf("done\n");
 
 // Network Information
 memset(&server, '0', sizeof(server));
 server.sin_family = AF_INET;
 server.sin_addr.s_addr = htonl(INADDR_ANY);
 server.sin_port = htons(_port_no);
 
 printf("%d",server.sin_port);

 // Bind Socket to a Specific Port
 printf("Binding socket to port...");
 if( (bind(socket_desc, (struct sockaddr*)&server, sizeof(server))) < 0)
 {
  perror("Bind Failed");
 } 
 printf("done");

 // Start Listening for Incoming Connections
 listen(socket_desc, back_log); 
 
 sock = socket_desc;


 for(;;)
 {
  printf("Waiting for Connections");
  client_sock = accept(socket_desc, (struct sockaddr*)NULL, NULL);
  printf("Connection Accepted");
  
  pthread_t sniffer_thread;
  
  new_sock = (int*) malloc(1);
  *new_sock = client_sock;
  

  pthread_create(&sniffer_thread, NULL, connection_handler, (void*) new_sock);

  close(client_sock);
  break;
  
 }
 */
 
 
 
}

//close socket
NetworkRequestChannel::~NetworkRequestChannel() 
{     
 close(sock);
}

/*--------------------------------------------------------------------------*/
/* READ/WRITE FROM/TO REQUEST CHANNELS  */
/*--------------------------------------------------------------------------*/

const int MAX_MESSAGE = 255;

string NetworkRequestChannel::send_request(string _request) 
{
	
}

string NetworkRequestChannel::cread() 
{
 char buf[MAX_MESSAGE];
 
 int n = recv(sock, buf, sizeof(buf), 0);
 buf[n] = '\0';
 
 printf("cread: %s", buf);
 
 return buf;
}

int NetworkRequestChannel::cwrite(string _msg) 
{
 char buf[MAX_MESSAGE];
 
 sprintf(buf, _msg.c_str());
 int n = send(sock, buf, strlen(buf)+1, 0);
 
 printf("cwrite: %s", buf);
 
 return n;
 
}