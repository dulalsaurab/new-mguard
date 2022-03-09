export NDN_LOG=mguard.*=TRACE:nacabe.*=TRACE
nfd-start >> /dev/null
mkdir example_logs
ndn-python-repo -c ndn-python-repo.conf

# this part only runs the first one and I don't think it's sending output to the files
./build/examples/mguard-controllerApp >> example_logs/controller.log
./build/examples/mguard-producerApp >> example_logs/producer.log
./build/examples/mguard-consumerApp >> example_logs/consumer.log

sleep 15
nfd-stop