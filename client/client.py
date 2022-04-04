# deque<pair<string,long int> >client_window;
from collections import deque
from ctypes import sizeof
from pydoc import cli
import time
from xxlimited import new
# Import socket module
import socket    
import threading


client_window = deque([])
# int window_size=15;
# int seq_counter=1;
seq_counter = 1
window_size = 15
packet_size=1
extra_ack = []

# long int sys_time()
# {
# 	//get current system time
# 	auto millisec_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
# 	return millisec_since_epoch;
# }

def sys_time():
    return int(round(time.time()* 1000))

def sendPackets(sock):
    buff = []
    while 1:
        if len(client_window) < window_size:
            prt = ""
            buff = []
            client_window.append((seq_counter, sys_time()))
            buff[0] = str(seq_counter)
            msg=(str(seq_counter).encode('utf8'))
            sock.sendall(msg)
            print(f'\nSequence number "+{seq_counter}+" sent from the client to the server\n')
            seq_counter=seq_counter+packet_size%65537


# while(extra_ack.count(client_window.front().first))
# 			{
# 				extra_ack.erase(client_window.front().first);
# 				client_window.pop_front();
# 			}
def timer_window_resize(sock):
    pass


def processAck(sock):
    while 1:
        data=str(sock.recv(1024).decode('utf8'))
        if data is not None and data != "":
            if data == client_window[0][0]:
                client_window.popleft()
                #  contains
                while client_window[0][0] in extra_ack:
                    extra_ack.remove(client_window[0][0])
                    client_window.popleft()
            else:
                # extra_ack.insert(buff[0]);
                extra_ack.append(data)
            timer_window_resize(sock)



timeout_ms=10000
window_increment=0

def timer(sock):
    while 1:
        i = 0
        # if( !client_window.empty() && sys_time()-client_window[i].second>timeout_ms)
        if client_window is not None and sys_time() - client_window[i][1] > timeout_ms:
            msg=(str(client_window[i][0]).encode('utf8'))
            sock.sendall(msg)
            window_size=window_size/2
            window_increment=1
            client_window[i][1] = timeout_ms
            print(f'\nSequence number "+{client_window[i][0]}+" retransmit from the client to the server\n')


window_timeout_ms=3000


maxsize=1000

def timer_window_resize(sock):
    temp = window_size
    if window_increment==0:
        new_window_size=window_size*2
    else:
        new_window_size=window_size+1
    
    if new_window_size>=maxsize:
        window_size=maxsize
    elif new_window_size<=0:
        window_size=1
    else:
        window_size=new_window_size
        print(f'\n window size changed from {temp} to {window_size}')



if __name__ == "__main__":
    # Create a socket object
    s = socket.socket()        
    
    # Define the port on which you want to connect
    port = 12345               
    
    # connect to the server on local computer
    #s.connect(('127.0.0.1', port))
    s.connect(('10.0.0.182', port))
    
    # receive data from the server and decoding to get the string.
    #print (s.recv(1024).decode())
    # close the connection
    s.close()
    t1 = threading.Thread(target=sendPackets, args=(s))
    t2 = threading.Thread(target=processAck, args=(s))
    t3 = threading.Thread(target=timer, args=(s))

  
    # starting thread 1
    t1.start()
    # starting thread 2
    t2.start()
    t3.start()
  
    # wait until thread 1 is completely executed
    t1.join()
    # wait until thread 2 is completely executed
    t2.join()
    t3.join()
  