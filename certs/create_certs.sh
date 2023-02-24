#!/bin/bash

server_certs() {
 echo "Installing server certs (producer, controller, and attribute authority)"
 ndnsec key-gen -t r /ndn/org/md2k
 ndnsec key-gen -t r /ndn/org/md2k/mguard/controller
 ndnsec key-gen -t r /ndn/org/mguard/aa

 ndnsec cert-dump -i /ndn/org/mguard/aa > aa.cert
 ndnsec cert-dump -i /ndn/org/md2k > producer.cert
 ndnsec cert-dump -i /ndn/org/md2k/mguard/controller > controller.cert
}

consumer_cert() {
  echo "Creating and installing consumer certificate"
  ndnsec key-gen -t r /ndn/org/md2k/A
  ndnsec sign-req /ndn/org/md2k/A > A.ndncsr
  ndnsec cert-gen -s /ndn/org/md2k -i /ndn/org/md2k A.ndncsr > A.cert
}

if [ "$1" == "-a" ]; then
  server_certs
  sleep 1
  consumer_cert
  exit
fi

server_certs


