# deque<pair<string,long int> >client_window;
from collections import deque
from ctypes import sizeof
from pydoc import cli
import time
from xxlimited import new
# Import socket module
import socket    
import threading
import random

# void sequence_check(string current_packet)
# {
# 	if(received_packets.empty())
# 	{
# 		received_packets.push_back(stoi(current_packet));
# 		return;
# 	}
# 	deque<int>::iterator it;
# 	it=received_packets.begin();
# 	//cout<<"At the beginning of the loop"<<" "<<received_packets[0]<<" "<<received_packets[1]<<" "<<received_packets[2]<<" "<<received_packets[received_packets.size()-1]<<endl;
#     while(it<received_packets.end())
# 	{
# 		if(*it>stoi(current_packet))
# 		{
# 			received_packets.insert(it,stoi(current_packet));
# 			break;
# 		}
# 		it++;
#     }
# 	//it=received_packets.begin();
# 	while(it<received_packets.end()-1)
# 	{
# 		it=received_packets.begin();
# 		if((*it+packet_size)%65537==*(it+1))
# 			received_packets.erase(received_packets.begin());
# 		else
# 			break;
# 	}
# 	//cout<<"At the end of the loop"<<" "<<received_packets[0]<<" "<<received_packets[1]<<" "<<received_packets[2]<<" "<<received_packets[received_packets.size()-1]<<endl;
# }

received_packets = deque([])
packet_size=1

def sequence_check(current_packet):
    if len(received_packets) == 0:
        received_packets.append(current_packet)
        return
    for i in range(len(received_packets)):
        if received_packets[i] > current_packet:
            received_packets.insert(i,current_packet)
            break
    
    i = 0
    while i < len(received_packets) -1:
        if received_packets[i]+ packet_size == received_packets[i+1]:
            received_packets.popleft()
        else:
            break

def checkIfDropped():
    x = random.randint(1, 100)
    if x == 100:
        return False
    else:
        return True 

if __name__ == "__main__":
    # next create a socket object
    s = socket.socket()		
    print ("Socket successfully created")

    # reserve a port on your computer in our
    # case it is 12345 but it can be anything
    port = 12345			

    # Next bind to the port
    # we have not typed any ip in the ip field
    # instead we have inputted an empty string
    # this makes the server listen to requests
    # coming from other computers on the network
    s.bind(('', port))		
    print ("socket binded to %s" %(port))

    # put the socket into listening mode
    s.listen(5)	
    print ("socket is listening")		

    # a forever loop until we interrupt it or
    # an error occurs
    while True:

    # Establish connection with client.
        c, addr = s.accept()	
        print ('Got connection from', addr )
        data=str(s.recv(1024).decode('utf8'))
        if checkIfDropped():
            sequence_check(int(data))
            # send a thank you message to the client. encoding to send byte type.
            msg=(str(data).encode('utf8'))
            c.send(msg)
            # Close the connection with the client
            c.close()
            # Breaking once connection closed
            # break
