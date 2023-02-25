from generator import get_cc

import socket
from time import sleep
import sys
import os
import json
import shutil

# sleep X seconds after sending each bath, configure as per need
s_after_sending_batch = 20

class Sender:
    def __init__(self, port):
        self.conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.conn.connect(('localhost', port))

    def send(self, payload):
        self.conn.send(payload.encode('utf-8'))

    def close(self):
        self.conn.close()

def get_sender():
    port = 8808
    print('Sender initialize')
    sender = Sender(port)
    return sender

def send_stream(stream_name, data, sender_obj):

    print("Sending data for stream name {}".format(stream_name))

    metadata = "{}|{}".format(stream_name, sys.getsizeof(data))
    print("Metadata of the stream {}".format(metadata))

    data = {'header': metadata, 'payload': data}
    print(data)
    
    serialize_data = json.dumps(data)

    ack = ''
    while not ack:
        sender_obj.conn.send(serialize_data.encode())
        ack = sender_obj.conn.recv(1024).decode()

    print("From server: ", ack)
    sender_obj.conn.close()

def main():
    '''
     Wrapper code for data generation and transport to producer via socket
     :var: total_number_of_batches int number of times we will generate the data and send it
    '''
    total_number_of_batches = 3
    current_batch = 1

    while current_batch <= total_number_of_batches:
        # removing the old stream data if exists
        try:
            shutil.rmtree(os.environ['HOME'] + '/cc_data/')
        except FileNotFoundError:
            print('No existing data to be deleted')

        start_time = '2022-05-0{} 10:00:00'.format(current_batch)
        #50
        end_time = '2022-05-0{} 10:0:50'.format(current_batch)
        #100
        # end_time = '2022-05-0{} 10:01:41'.format(current_batch)
        # 150
        # end_time = '2022-05-0{} 10:02:31'.format(current_batch)
        # 200
        # end_time = '2022-05-0{} 10:03:21'.format(current_batch)
        # 250
        # end_time = '2022-05-0{} 10:04:11'.format(current_batch)
        # 300
        # end_time = '2022-05-0{} 10:05:01'.format(current_batch)
        # 350
        # end_time = '2022-05-0{} 10:05:51'.format(current_batch)
        # 400
        # end_time = '2022-05-0{} 10:06:41'.format(current_batch)
        # 500
        # end_time = '2022-05-0{} 10:08:21'.format(current_batch)

        print("Fetching data for start_time {} and end_time {}".format(start_time, end_time))

        cc_obj, streams = get_cc(start_time, end_time)
        for stream in streams:
            stream_name = streams[stream]
            sender_obj = get_sender()
            data = cc_obj.get_stream(stream_name).toPandas()

            # uncomment for debugging
            # data1 = data
            # data1.to_csv(str(current_batch) + '_' + stream_name, index=False)

            print("Sample data from the stream: \n", data)
            send_stream(stream_name, data.to_csv(), sender_obj)
            
            sleep(2)

        print("Sending data for batch: {}, completed".format(current_batch))
        current_batch += 1

        print("Sleeping for {} second sending batch data".format(s_after_sending_batch))
        sleep(s_after_sending_batch)

    print ("Sending data for all the batch completed")

if __name__ == '__main__':
    main()