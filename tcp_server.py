#!/usr/bin/env python

import socket
import time
import select
import struct

TCP_IP = '127.0.0.1'
TCP_PORT = 1234
BUFFER_SIZE = 20  # Normally 1024, but we want fast response

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind((TCP_IP, TCP_PORT))
s.listen(1)

buf = b'\x04\x02\xff\x08ABCDEFGH'
buf = struct.pack('B', len(buf)) + buf
#crc = 0
#for c in buf:
#    crc += c
#buf = buf + struct.pack('B', ~crc & 0xFF)
print(buf)

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
        try:
            time.sleep(0.2)
            conn.send(buf)  # echo
            i += 1
        except BrokenPipeError:
            break
        except ConnectionResetError:
            break
        except KeyboardInterrupt:
            conn.close()
            break
        except ConnectionAbortedError:
            break

s.close()

