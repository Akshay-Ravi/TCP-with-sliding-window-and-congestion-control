from collections import deque
from ctypes import sizeof
from pydoc import cli
import time
from xxlimited import new
# Import socket module
import socket    
import threading


client_window = deque([])
seq_counter = 1
window_size = 15
packet_size=5
extra_ack = []
wrap_around=65537

def sys_time():
    return int(round(time.time()* 1000))

def sendPackets(sock):
    global seq_counter
    while 1:
        if len(client_window) < window_size:
            client_window.append([seq_counter, sys_time()])
            temp=str(seq_counter)+' '
            msg=temp.encode('utf8')
            sock.sendall(msg)
            print(f'\nSequence number"{temp}" sent from the client to the server\n')
            seq_counter=(seq_counter+packet_size)%wrap_around
            time.sleep(0.0125)


def timer_window_resize(sock):
    pass


def processAck(sock):
    while 1:
        data=str(sock.recv(1024).decode('utf8')).strip()
        if data is not None and data != "":
            for d in data.split(' '):
                print(f'\nAcknowledgement number "{d}" received\n')
                if len(client_window)>0 and int(d) == client_window[0][0]:
                    client_window.popleft()
                    while len(client_window)!=0 and client_window[0][0] in extra_ack:
                        extra_ack.remove(client_window[0][0])
                        client_window.popleft()
                else:
                    extra_ack.append(int(d))
                    timer(sock)
                if len(client_window)>0:
                    print(client_window[0],"\n")
            timer_window_resize(sock)

window_increment=0

def timer(sock):
    global window_size
    timeout_ms=5000
    global window_increment
  #  while 1:
    i = 0
    # if( !client_window.empty() && sys_time()-client_window[i].second>timeout_ms)
    #if len(client_window)!=0 and sys_time() - client_window[i][1] > timeout_ms:
    if len(extra_ack)>=20:
        if len(client_window)>0:
            msg=(str(client_window[i][0])+' ').encode('utf8')
            sock.sendall(msg)
            window_size=int(window_size/2)
            window_increment=1
            client_window[i][1] = sys_time()
            print(f'\nSequence number "{client_window[i][0]}" retransmit from the client to the server\n')


def timer_window_resize(sock):
    global window_increment
    window_timeout_ms=3000
    maxsize=10000
    global window_size
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
    file1.write(str(window_size)+","+str(sys_time())+"\n")


if __name__ == "__main__":
    # Create a socket object
   # s = socket.socket()        
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # Define the port on which you want to connect
    port = 12345
    # Define the IP address of the server
    server_ip_addr='10.0.0.242'
    s.connect((server_ip_addr, port))
    print(f'\n Connected to Server with IP Address: {server_ip_addr} and Port: {port}')
    msg=("Hello Server").encode('utf8')
    s.sendall(msg)
    
    file1 = open("windowsize.txt", "a")  # append mode
    file1.write("15"+","+str(sys_time())+"\n")

    while 1:
        data=str(s.recv(1024).decode('utf8')).strip()
        if data is not None and data != "":
            print(data,'\n\n')
            break

    t1 = threading.Thread(target=sendPackets, args=(s,))
    t2 = threading.Thread(target=processAck, args=(s,))
  
    # starting thread 1
    t1.start()
    # starting thread 2
    t2.start()
    #t3.start()
  
    # wait until thread 1 is completely executed
    t1.join()
    # wait until thread 2 is completely executed
    t2.join()
    file1.close()