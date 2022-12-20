from generator import get_cc

import socket
import os
from time import sleep
import sys
import shutil

BUFFER_SIZE = 1024

# sleep times, configure this after each run
s_after_metadata = 10
s_after_first_stream = 40
s_after_sending_batch = 10

'''
# these numbers should be sufficient
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
rows   | n.steams | s_after_metadata | s_after_first_stream |  s_after_sending_batch |
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
n.rows |   50     |                  |                      |                        |
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
n.rows |   100    |      10          |        20            |         10             |
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
n.rows |   150    |                  |                      |                        |
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
n.rows |   200    |                  |                      |                        |
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
n.rows |   250    |                  |                      |                        |
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
n.rows |   300    |      10          |         40           |          10            |

'''

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

    _size = sys.getsizeof(data)
    number_of_chunks = int(_size / BUFFER_SIZE) + 1
    metadata = "{}|{}|{}".format(stream_name, number_of_chunks, _size)
    print("Metadata of the stream {}".format(metadata))

    sender_obj.send(metadata)
    # sleep a few seconds after sending the metadata
    print("Sleeping for {} second after sending meta data".format(s_after_metadata))
    sleep(s_after_metadata)

    sender_obj.send(data)
    # sleep X seconds after sending the first stream. 
    # this is because the data-adapter needs to process the previous packet i.e. metadata
    #change for 16 min to 100 sec 
    print("Sleeping for {} sec after sending first stream".format(s_after_first_stream))
    sleep(s_after_first_stream)

    sender_obj.close()

def main():
    """Wrapper code for data generation and transport to producer via socket

    The code below will fetch data for datatime starting from 2022-05-01 till 2022-05-20
    for each of these dates, data for time range 10:00:00 - 10:00:10 i.e. 10 minutes equivalent of data
    will be generated and send to data adapter. As said, the process will continue for 20 iterations.

    :var: total_number_of_batches int number of times we will generate the data and send it
    """
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
        end_time = '2022-05-0{} 10:0:51'.format(current_batch)
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

            print("Sample data from the stream: \n", data[:10])
            send_stream(stream_name, data.to_csv(), sender_obj)
        
        print("sending data for batch: {}, completed".format(current_batch))
        current_batch += 1
        
        print("Sleeping for {} second sending batch data".format(s_after_sending_batch))
        sleep(s_after_sending_batch)  # testing: sleep for X minute and send another batch

    print ("sending data for all the batch completed")

if __name__ == '__main__':
    main()
