#include <iostream>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <deque>
#include <chrono>
#include <sys/time.h>
#define MAX 8000
#define PORT 8083
#include <unistd.h>
#define SA struct sockaddr
using namespace std;
using namespace std::chrono;

deque<pair<int,long int> >client_window;
int window_size=5;
int seq_counter=1;
int packet_size=1;
void sendPackets(int sockfd)
{
	while(1)
	{
		if(client_window.size()<window_size)
		{
			pair<int,long int>p;
			p.first=seq_counter;
			struct timeval tp;
			gettimeofday(&tp, NULL);
			long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
			p.second=ms;
			client_window.push_back(p); //adding the sequnce number to the deque

			char buff[1];
			bzero(buff, sizeof(buff));
			std::string tmp = std::to_string(seq_counter);
    		char const *num_char= tmp.c_str();
			buff[0]=*num_char;
			cout<< buff[0];
			write(sockfd, buff, sizeof(buff));
			cout<<"Sequence number "<<seq_counter<<" sent from the client to the server"<<endl;
			seq_counter=seq_counter+packet_size;
		}
	}
}

// void processAck(int sockfd)
// {
// 	char buff[1];
// 	bzero(buff, sizeof(buff));
// 	while(1)
// 	{
// 		read(sockfd, buff, sizeof(buff));
// 		deque<pair<int,long int>>::iterator it;
//  		it = mydeque.begin();
//     	mydeque.erase(it);
// 		if(buff[0]==(*deque.begin()).first)
// 		{

// 		}
// 	}
// }

void func(int sockfd)
{
	char buff[MAX];
	int n;
	for (;;) {
		bzero(buff, sizeof(buff));
		printf("Enter the string : ");
		n = 0;

		// Reading into buffer until a Enter keyword is hit
		while ((buff[n++] = getchar()) != '\n');
		// Write from buffer to the Socket
		write(sockfd, buff, sizeof(buff));
		// Clean the buffer
		bzero(buff, sizeof(buff));
		// Read from sockfd to the buffer
		read(sockfd, buff, sizeof(buff));
		// Print the message
		printf("From Server : %s", buff);

		// Check if the server has sent an exit keyword, if yes then close the connection
		if ((strncmp(buff, "exit", 4)) == 0) {
			printf("Client Exit...\n");
			break;
		}
	}
}

int main()
{
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;

	// socket create and verification

	// SOCK_STREAM denotes that it is TCP
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");

		// takes the starting location of the memory ans no of bytes to delete from that location and deletes it
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET; // AF_INET Denoted IPV4
	//servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_addr.s_addr = inet_addr("192.168.248.208");

	// converts port i.e.8080 here to 16 bit network byte order
	servaddr.sin_port = htons(PORT);

	printf("%d\n",servaddr.sin_port );

	// connect the client socket to server socket
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
		printf("connected to the server..\n");

	// function for chat
	// func(sockfd);

	sendPackets(sockfd);
	// close the socket
	close(sockfd);
}
