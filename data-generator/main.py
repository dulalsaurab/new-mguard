# from generator import get_cc
# from client import *

# data_limit = 2**13
# port = 17001


# def send_stream(name):
#     to_send = cc.get_stream(streams[name]).toPandas().to_csv()
#     limited = limit(to_send, data_limit)
#     client = Client(port)
#     client.send(limited)


# if __name__ == '__main__':
#     cc, streams = get_cc()
#     """
#     names:
#     battery
#     location
#     semantic_location
#     accel
#     gyro
#     """
#     send_stream('battery')


from generator import get_cc

import socket
import pandas as pd
from pathlib  import Path
from time import sleep
import sys
import pickle

BUFFER_SIZE = 1024

class Sender:
    def __init__(self, port):
        self.conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.conn.connect(('localhost', port))

    def send(self, payload):
        self.conn.send(payload.encode('utf-8'))

    def close(self):
        self.conn.close()

def getSender():
    port = 8808
    print('Sender initialize')
    sender = Sender(port)
    return sender

def main():

    '''
        names:
        semantic_location
        battery
        location
        accel
        gyro
    '''
    cc_Obj, streams = get_cc()
    senderObj = getSender()

    for key in streams:
        stream_name = streams[key]
        data = cc_Obj.get_stream(stream_name).toPandas().to_csv()
        
        # first send the metadata
        _size = sys.getsizeof(data)
        number_of_chunks = int(_size/BUFFER_SIZE) + 1; # rounding
        metadata = "{}|{}|{}".format(stream_name, number_of_chunks, _size)
        print (_size, metadata)
        
        senderObj.send(metadata)
        sleep(1)
        senderObj.send(data)
        sleep(10)

    print("sending data completed")
    senderObj.close()

if __name__ == '__main__':
    main()


