from curses import meta
from ipaddress import summarize_address_range

from numpy import number
from generator import get_cc

import socket
import pandas as pd
from pathlib  import Path
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

def getSender():
  port = 8808
  print('Sender initialize')
  sender = Sender(port)
  return sender

def sendStream(stream_name, data, senderObj):
  print ("Sending data for stream name {}".format(stream_name))

  _size = sys.getsizeof(data)
  number_of_chunks = int(_size/BUFFER_SIZE) + 1
  metadata = "{}|{}|{}".format(stream_name, number_of_chunks, _size)
  print ("Metadata of the stream {}".format(metadata))

  senderObj.send(metadata)
  sleep(10) # sleep a few seconds after sending the metadata

  senderObj.send(data)
  # sleep 20 seconds after sending the first stream, this is because the data-adapter needs to process
  # the previous packet i.e metadata
  sleep(20) 

  print("sending data completed")
  senderObj.close()

def main():
  #removing the old stream data if exists
  shutil.rmtree('/home/map901/cc_data/')

  '''
      names:
      semantic_location
      battery
      location
      accel
      gyro
  '''
  total_number_of_batch = 1

  '''
    @param total_number_of_batch int number of times we will generate the data and send it
    The code below will fetch data for datatime starting from 2022-05-01 till 2022-05-20
    for each of these dates, data for time range 10:00:00 - 10:00:10 i.e. 10 minutes equivalent of data
    will be generated and send to data adapter. As said, the process will continue for 20 iterations.
  '''
  while(total_number_of_batch <= 2):

    start_time = '2022-05-0{} 10:00:00'.format(total_number_of_batch)
    end_time = '2022-05-0{} 10:00:02'.format(total_number_of_batch)
    print ("Fetching data for starttime {} and endtime {}".format(start_time, end_time))

    cc_obj, streams = get_cc(start_time, end_time)
    for stream in streams:
      stream_name = streams[stream]
      senderObj = getSender()
      data = cc_obj.get_stream(stream_name).toPandas()

      print ("Sample data from the stream: \n", data[0:10])
      sendStream(stream_name, data.to_csv(), senderObj)

    total_number_of_batch += 1
    sleep (60) # testing: sleep for 1 minute and send another batch

if __name__ == '__main__':
    main()
