from datetime import datetime
from ipaddress import summarize_address_range

from numpy import number
from generator import get_cc

import socket
import pandas as pd
from pathlib  import Path
from time import sleep
import sys
import pickle
import os

BUFFER_SIZE = 1024
user_id = 'dd40c'
study_name =  'mguard'

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
  _size = sys.getsizeof(data)
  number_of_chunks = int(_size/BUFFER_SIZE) + 1
  metadata = "{}|{}|{}".format(stream_name, number_of_chunks, _size)
  print(_size, metadata)
  senderObj.send(metadata)
  sleep(1) # sleep one second after sending the header
  print (data)
  senderObj.send(data)
  sleep(20) # sleep 10 seconds after sending the first stream
  print("sending data completed")
  senderObj.close()


def main():

  '''
      names:
      semantic_location
      battery
      location
      accel
      gyro
  '''
  if not os.path.isfile("dataSet.pkl"):
    cc_Obj, streams = get_cc()

    # first send semantic_location stream
    stream_name = streams['semantic_location']
    print ("getting data for semantic loaction stream: ", stream_name)

    data = cc_Obj.get_stream(stream_name).toPandas().to_csv()
    # sendStream(stream_name, data, senderObj = getSender())
    with open("semLoc.pkl", "wb") as of1:
      pickle.dump(data, of1)

    del streams['semantic_location'] # remove from dict after sending
    print ("Remaining streams to send: ", streams.keys())

    dataSet = {}
    for stream in streams:
      stream_name = streams[stream]
      print ("getting data for stream name: ", stream_name)
      dataSet[stream_name] = cc_Obj.get_stream(stream_name).toPandas()

    with open("dataSet.pkl", "wb") as of2:
      pickle.dump(dataSet, of2)

  else:
    with open ("semLoc.pkl", "rb") as inpf1:
      semLocData = pickle.load(inpf1)

    with open ("dataSet.pkl", "rb") as inpf2:
      dataSet = pickle.load(inpf2)

  # 1. send semantic location data
  stream_name = 'org--md2k--{}--{}--data_analysis--gps_episodes_and_semantic_location'.format(study_name, user_id)
  senderObj = getSender()
  sendStream(stream_name, semLocData, senderObj)

  # we will get all the data at once, and publish x number of rows in a batch except semantic loacation data
  # for example, if an stream contains 1000 rows, 10 batch will be sent each containing 100 rows
  start = 0
  end = 100

  while(True):

    if not dataSet:
      break

    for stream_name in dataSet:
      print ("sending data for stream: ", stream_name)
      senderObj = getSender()
      data = dataSet[stream_name][start:end]
      if data.size == 0:
        del dataSet[stream_name]
        continue
      print (data)
      sendStream(stream_name, data.to_csv(), senderObj)

    start = end
    end = 2*end + 1
    sleep (60) # testing: sleep for 1 minute and send another batch  

if __name__ == '__main__':
    main()
