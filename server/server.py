# deque<pair<string,long int> >client_window;
from collections import deque
from ctypes import sizeof
from pydoc import cli
import time
import sys
from xxlimited import new
# Import socket module
import socket    
import threading
import random
received_packets=deque([])
packet_size=1
def sys_time():
    return int(round(time.time()*1000))
def sequence_check(current_packet):
    if len(received_packets) == 0:
        received_packets.append(current_packet)
        return
    for i in range(len(received_packets)):
        if received_packets[i] > current_packet:
            received_packets.insert(i,current_packet)
            break
    i=0
    while i < len(received_packets) -1:
        if (received_packets[i]+packet_size)%65537==received_packets[i+1]:
            received_packets.popleft()
        else:
            break
def checkIfDropped():
    x=random.randint(1,100)
    if x==100:
        return False
    else:
        return True 
if __name__=="__main__":
    # next create a socket object
    s=socket.socket(socket.AF_INET,socket.SOCK_STREAM)		
    print ("Socket successfully created")
    # reserve a port on your computer in our
    # case it is 12345 but it can be anything
    port=12345
    # Next bind to the port
    # we have not typed any ip in the ip field
    # instead we have inputted an empty string
    # this makes the server listen to requests
    # coming from other computers on the network
    s.bind(('',port))		
    print ("socket binded to %s" %(port))
    # put the socket into listening mode
    s.listen(5)	
    print ("socket is listening \n")		
    # a forever loop until we interrupt it or
    # an error occurs
    c,addr=s.accept()
    print("Connection established with client %s" %(str(addr)))
    hello_msg=c.recv(1024)
    print(hello_msg)
    success_message="Success".encode('utf8')
    c.sendall(success_message)
    no_of_packets_received=0
    no_of_packets_sent=0
    total_packets=0
    file1=open("seq_number_received.txt",'a')
    file2=open("seq_number_dropped.txt",'a')
    file3=open('goodput.txt','a')
    while True:
    # Establish connection with client.	
        #print ('Got connection from',addr)
        data=str(c.recv(1024).decode('utf8')).strip()
        #print(data,"\n")
        for d in data.split(' '):
            if checkIfDropped():
                sequence_check(int(d))
                print(f'\nSequence number "{d}" received\n')
                file1.write(str(d)+','+str(sys_time())+'\n') #writing the time at which the packet was received
                # send a thank you message to the client. encoding to send byte type.
                msg=(str(d)+' ').encode('utf8')
                c.sendall(msg)
                no_of_packets_received+=1
            else:
                file2.write(str(d)+','+str(sys_time())+'\n')
            no_of_packets_sent+=1
            total_packets+=1
            if no_of_packets_received==1000:
                file3.write(str(no_of_packets_received)+"/"+str(no_of_packets_sent)+"="+str(no_of_packets_received/no_of_packets_sent)+"\n")
                no_of_packets_sent=0
                no_of_packets_received=0
            if total_packets>=1000000: #stop the socket after 1 million packets
                file1.close()
                file2.close()
                file3.close()
                c.close()
                sys.exit(0)
