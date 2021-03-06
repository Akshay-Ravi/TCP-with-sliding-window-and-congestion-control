#include<iostream>
using namespace std;
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include<deque>
#define MAX 1000
#define PORT 8085
#define SA struct sockaddr
using namespace std;

deque<int>received_packets;
deque<int>toBeDropped_packets;
int window_size=15;
int seq_count=0;
int packet_size=1;

//Function to check sequence of the packets that are received
bool checkIfDropped(string current_packet)
{
	int current_packet_i=stoi(current_packet);
	if(current_packet_i==*(toBeDropped_packets.begin()))
	{
		toBeDropped_packets.pop_front();
		return true;
	}
	else
	{
		return false;
	}
}


void sequence_check(string current_packet)
{
	if(received_packets.empty())
	{
		received_packets.push_back(stoi(current_packet));
		return;
	}
	deque<int>::iterator it;
	it=received_packets.begin();
	//cout<<"At the beginning of the loop"<<" "<<received_packets[0]<<" "<<received_packets[1]<<" "<<received_packets[2]<<" "<<received_packets[received_packets.size()-1]<<endl;
    while(it<received_packets.end())
	{
		if(*it>stoi(current_packet))
		{
			received_packets.insert(it,stoi(current_packet));
			break;
		}
		it++;
    }
	//it=received_packets.begin();
	while(it<received_packets.end()-1)
	{
		it=received_packets.begin();
		if((*it+packet_size)%65537==*(it+1))
			received_packets.erase(received_packets.begin());
		else
			break;
	}
	//cout<<"At the end of the loop"<<" "<<received_packets[0]<<" "<<received_packets[1]<<" "<<received_packets[2]<<" "<<received_packets[received_packets.size()-1]<<endl;
}
// Function designed for chat between client and server.
void func(int connfd)
{
	string buff[MAX];
	string ack_buff[MAX];
	int n;
	//cout<<"test";
	// infinite loop for chat
	for (;;) {
		bzero(buff, MAX);
		bzero(ack_buff, MAX);
		//cout<<"test1";
		// read the message from client and copy it in buffer
		read(connfd, buff, sizeof(buff));

		if(buff[0]!="\0" && !checkIfDropped(buff[0]))
		{
			if(!checkIfDropped(buff[0]))
			{
				// print buffer which contains the client contents
				string prt="\nReceived From client: "+buff[0];
				cout<<prt<<endl;
				//cout<<buff[0];
				sequence_check(buff[0]);
				ack_buff[0]=buff[0];
				write(connfd, ack_buff, sizeof(buff));
				prt="\nSending to client ack for: "+ack_buff[0];
				cout<<prt<<endl;
			}
			// else
			// {
			// 	ack_buff[0]=buff[0];
			// 	write(connfd, ack_buff, sizeof(buff));
			// 	string prt="\nSending to client NO ACK for: "+ack_buff[0];
			// 	cout<<prt<<endl;
			// }
		}

		// int i=0;
		// while(buff[i]!="\0")
		// {
		// 	if(!checkIfDropped(buff[i]))
		// 	{
		// 		bzero(ack_buff, MAX);
		// 		// print buffer which contains the client contents
		// 		string prt="\nReceived From client: "+buff[i];
		// 		cout<<prt<<endl;
		// 		//cout<<buff[0];
		// 		sequence_check(buff[i]);
		// 		ack_buff[0]=buff[i];
		// 		write(connfd, ack_buff, sizeof(buff));
		// 		prt="\nSending to client ack for: "+ack_buff[i];
		// 		cout<<prt<<endl;
		// 	}
		// 	i++;
		// }
	}
		// // copy server message in the buffer
		// while ((buff[n++] = getchar()) != '\n')
		// 	;
		// // and send that buffer to client
		// write(connfd, buff, sizeof(buff));

		// if msg contains "Exit" then server exit and chat ended.
		// if (strncmp("exit", buff, 4) == 0) {
		// 	printf("Server Exit...\n");
		// 	break;
		// }
	//}
}
// Driver function
int main()
{
	toBeDropped_packets.push_back(65530);

	int sockfd, connfd;
	socklen_t len;
	struct sockaddr_in servaddr, cli;
	//socket create and verification
	//SOCK_STREAM is used to establish a connection in TCP
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	//clearing the server address buffer
	bzero(&servaddr, sizeof(servaddr));
	// assign IP, PORT
	servaddr.sin_family = AF_INET; //Denotes IPv4
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //
	servaddr.sin_port = htons(PORT); //Converts PORT(8080) to a network byte order
	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");
	// Now server is ready to listen and verification
	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");
	len = sizeof(cli);
	// Accept the data packet from client and verification
	connfd = accept(sockfd, (SA*)&cli, &len);
	if (connfd < 0) {
		printf("server accept failed...\n");
		exit(0);
	}
	else
		printf("server accept the client...\n");
	//Print the server's IP address
	// Function for chatting between client and server
	cout<<"test2";
	func(connfd);
	// After chatting close the socket
	close(sockfd);
}