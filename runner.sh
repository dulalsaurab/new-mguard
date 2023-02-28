export NDN_LOG=mguard.*=DEBUG:nacabe.*=DEBUG


rm debugg/*

sleep 2

sudo killall mguard-controllerApp
sudo killall mguard-producerApp
sudo killall ndn-python-repo

nfdc cs erase /ndn/org/md2k

# nfd-stop

sleep 2

rm -rf ~/.ndn/ndn-python-repo/sqlite3.db

# nfd-start &>/dev/null

sleep 1

ndn-python-repo -c ndn-python-repo.conf > debugg/repo.log 2>&1 &

sleep 1

mguard-controllerApp > debugg/controller.log 2>&1 &

sleep 1

# valgrind --leak-check=full --log-file=valgrind-out.txt ./build/examples/mguard-producerApp > debugging/p.log 2>&1 &
mguard-producerApp > debugg/producer.log 2>&1 &

sleep 1

python data-generator/main.py > debugg/generator.log 2>&1 &

sleep 0.1

mguard-consumerApp -c certs/local.cert -p /ndn/org/md2k/local > debugg/consumer.log 2>&1 &
