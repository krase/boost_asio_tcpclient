#!/usr/bin/env python

import socket
import time
import select

TCP_IP = '127.0.0.1'
TCP_PORT = 1234
BUFFER_SIZE = 20  # Normally 1024, but we want fast response

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind((TCP_IP, TCP_PORT))
s.listen(1)

while 1:
    print("Waiting for client")
    try:
        conn, addr = s.accept()
    except KeyboardInterrupt:
        break
    
    print('Connection address:', addr)

    i = 0
    while 1:
        #data = conn.recv(BUFFER_SIZE)
        #if not data: break
        #print "received data:", data
        data = bytearray('%d '%i, 'ascii')
        try:
            time.sleep(0.01)
            conn.send(data)  # echo
            i += 1
        except BrokenPipeError:
            break
        except ConnectionResetError:
            break
        except KeyboardInterrupt:
            break
        except ConnectionAbortedError:
            break

s.close()

