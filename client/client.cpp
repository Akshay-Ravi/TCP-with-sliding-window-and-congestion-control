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
#include<unordered_set>
#define PORT 8085
#define MAX 10
#include <unistd.h>
#include <thread>
#include <string>
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

#define SA struct sockaddr
using namespace std;
using namespace std::chrono;

deque<pair<string,long int> >client_window;
int window_size=15;
int seq_counter=1;
int packet_size=1;
int window_increment=0;
void processAck(int sockfd);
void timer_window_resize(int sockfd);

long int sys_time()
{
	//get current system time
	auto millisec_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	return millisec_since_epoch;
}

void sendPackets(int sockfd)
{
	string buff[MAX];
	while(1)
	{
		if(client_window.size()<window_size)
		{
			string prt="";
			bzero(buff, sizeof(buff));
			int i=0;

		//	while(client_window.size()<window_size)
			{
				std::string num_char = std::to_string(seq_counter);
				pair<string,long int>p;
				p.first=num_char;
				//get current system time
				p.second=sys_time();
				client_window.push_back(p); //adding the sequnce number to the deque
				buff[i++]=num_char;
				prt="\nSequence number "+num_char+" sent from the client to the server\n";
				cout<<prt<<endl;
				seq_counter=(seq_counter+packet_size)%65537;
			}
					
			write(sockfd, buff, sizeof(buff)); // comment for local
			// prt="Client Window-";
			// for(auto x:client_window)
			// 	prt=prt+x.first+":"+std::to_string(x.second)+" ";
			// cout<<prt<<endl;
		}

		//take it slow
		unsigned int microsecond = 1000000/2;
		usleep(1 * microsecond);//sleeps for 3 second
	}
}

// string input[]={"1","2","4","5","3","6","7","8","9","10","11","12","13","14","15"}; 

unordered_set<string>extra_ack;
void processAck(int sockfd)
{
	string buff[1];
	bzero(buff,sizeof(buff));
	while(1)  // comment for local
	{  // comment for local
		read(sockfd, buff, sizeof(buff)); // comment for local
	 if(buff[0]!="\0")
	 {
		string prt="Processing Ack for Seq No:"+buff[0]+"\n";
		cout<<prt<<endl;
		// prt=" Extra Acknowledgement-";
		// for(auto x:extra_ack)
		// 	prt=prt+x+" ";
		// cout<<prt<<endl;

		if(buff[0]!="\0" && buff[0].compare(client_window.front().first)==0)
		{
			client_window.pop_front();
			while(extra_ack.count(client_window.front().first))
			{
				extra_ack.erase(client_window.front().first);
				client_window.pop_front();
			}
		}
		else
		{
			//cout<<"\nWhy is it here? "<<extra_ack[buff[0]]<<endl;
			extra_ack.insert(buff[0]);
			//
		}
		timer_window_resize(sockfd);
	 }
	}  // comment for local
}


void processAck1(int sockfd)
{
	string buff[1];
	bzero(buff,sizeof(buff));
	while(1)  // comment for local
	{  // comment for local
		read(sockfd, buff, sizeof(buff)); // comment for local
	 if(buff[0]!="\0")
	 {
		while(buff[0]!="\0" && stoi(buff[0])>stoi(client_window.front().first))
		{
			string prt="Processing Ack for Seq No:"+buff[0]+"\n";
			cout<<prt<<endl;
			client_window.pop_front();
			timer_window_resize(sockfd);
		}
	 }
	}  // comment for local
}



int timeout_ms=10000;
//bool congestion=false;
long int last_window_resize_ms=sys_time();


void timer(int sockfd)
{
	while(1)
	{
		int i=0;
		//for(int i=0;i<client_window.size();i++)
		{
			string prt="For Retrans:";
			for(auto x:client_window)
					prt=prt+x.first+":"+std::to_string(x.second)+" ";
			//cout<<prt<<endl;
			if( !client_window.empty() && sys_time()-client_window[i].second>timeout_ms)
			{
				//cout<<"Delay "<<sys_time()-client_window[i].second<<endl;
				//last_window_resize_ms=sys_time();
				string packet_num=client_window[i].first;
				int first_trans_time=client_window[i].second;
				string first_trans_time_str=to_string(client_window[i].second);
				prt="\nPacket number: "+packet_num+" lost "+ to_string(sys_time())+" "+first_trans_time_str;
				cout<<prt<<endl;
				if(first_trans_time!=0 && first_trans_time_str.compare("0")!=0 && first_trans_time_str!="0")
				{
				string buff[1];
				bzero(buff, sizeof(buff));
				buff[0]=client_window[i].first;
				//update the time of the retransmitted packet before sending it
				client_window[i].second=sys_time();
				prt="Retran Cl Window-";
				for(auto x:client_window)
					prt=prt+x.first+":"+std::to_string(x.second)+" ";
				cout<<prt<<endl;
				write(sockfd,buff,sizeof(buff)); // comment for local
				window_size=window_size/2;
				window_increment=1;
				}
			}
		}
	}
}


void timer1(int sockfd)
{
	string missed_buff[1];
	bzero(missed_buff,sizeof(missed_buff));
	string prt="";
	while(1)
	{
		read(sockfd, missed_buff, sizeof(missed_buff)); // comment for local
	 	if(missed_buff[0]!="\0")
	 	{
				prt="\nPacket number: "+missed_buff[0]+" lost\n";
				cout<<prt<<endl;
				string buff[1];
				bzero(buff, sizeof(buff));
				buff[0]=missed_buff[0];
				prt="Retran Cl Window-";
				for(auto x:client_window)
					prt=prt+x.first+":"+std::to_string(x.second)+" ";
				cout<<prt<<endl;
				write(sockfd,buff,sizeof(buff)); // comment for local
				window_size=window_size/2;
				window_increment=1;
		}
	}
}



int window_timeout_ms=3000;

void timer_window_resize(int sockfd)
{
	//while(1)
	{
		// if(sys_time()-last_window_resize_ms>window_timeout_ms)
		// {
			last_window_resize_ms=sys_time();
			string prt="\nResizing Window from "+to_string(window_size);
			//if congestion happened
			int new_window_size=0;
			if(window_increment==0)
				new_window_size=window_size*2;
			else
				new_window_size=window_size+1;
	
			if(new_window_size>=MAX)
			{
				window_size=MAX;
				// prt+=" to "+to_string(window_size)+"\n";
				// cout<<prt;
				//break;
			}
			else if(new_window_size<=0)
			{
				window_size=1;
			}
			else
			{
				window_size=new_window_size;
			}
			prt+=" to "+to_string(window_size)+"\n";
			cout<<prt;
		//}
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
	servaddr.sin_addr.s_addr = inet_addr("10.0.0.182");
	//servaddr.sin_addr.s_addr = inet_addr("192.168.76.166");

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


		//int sockfd=1;
		thread t1(sendPackets,sockfd);

		unsigned int microsecond = 1000000;
		usleep(1 * microsecond);//sleeps for 3 second


		thread t2(processAck,sockfd);

		thread t3(timer,sockfd);
	//	thread t4(timer_window_resize,sockfd);
		string str;
		// for(int i=0;i<100;i++)
		// {
		// 	str="system time "+to_string(i)+"="+to_string(sys_time())+"\n";
		// 	cout<<str;
		// 	usleep(1000);
		// }

		t1.join();
		t2.join();
		t3.join();
	//	t4.join();
	// close the socket
	//close(sockfd);
}
