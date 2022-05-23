from generator import get_cc

import socket
import os
from time import sleep
import sys
import shutil

BUFFER_SIZE = 1024


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
    #change for 16 min to 20 sec
    sleep(10)

    sender_obj.send(data)
    # sleep X seconds after sending the first stream. 
    # this is because the data-adapter needs to process the previous packet i.e. metadata
    #change for 16 min to 100 sec 
    sleep(30)
    sender_obj.close()


def main():
    """Wrapper code for data generation and transport to producer via socket

    The code below will fetch data for datatime starting from 2022-05-01 till 2022-05-20
    for each of these dates, data for time range 10:00:00 - 10:00:10 i.e. 10 minutes equivalent of data
    will be generated and send to data adapter. As said, the process will continue for 20 iterations.

    :var: total_number_of_batches int number of times we will generate the data and send it
    """
    total_number_of_batches = 5

    current_batch = 1
    while current_batch <= total_number_of_batches:
        # removing the old stream data if exists
        try:
            shutil.rmtree(os.environ['HOME'] + '/cc_data/')
        except FileNotFoundError:
            print('No existing data to be deleted')

        start_time = '2022-05-0{} 10:00:00'.format(current_batch)
        end_time = '2022-05-0{} 10:02:00'.format(current_batch)
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
        sleep(60)  # testing: sleep for X minute and send another batch

    print ("sending data for all the batch completed")

if __name__ == '__main__':
    main()
