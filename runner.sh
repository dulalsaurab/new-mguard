echo "set mguard and nacabe logs to debug"
export NDN_LOG=ndn.security.*=DEBUG:mguard.*=DEBUG:nacabe.*=DEBUG

echo "reset debug logs"
rm -rf debug
mkdir debug

sleep 2

sudo ldconfig

#sudo pkill -f -e -c mguard
echo "killall controller"
sudo killall mguard-controllerApp
echo "killall producer"
sudo killall mguard-producerApp
echo "killall repo"
sudo killall ndn-python-repo

echo "nfdc erase"
nfdc cs erase /ndn/org/md2k

echo "nfdstop"
nfd-stop

sleep 2

echo "delete repo"
rm -rf ~/.ndn/ndn-python-repo/sqlite3.db

echo "start nfd"
sudo nfd-start &>/dev/null

sleep 1

echo "start repo"
ndn-python-repo -c ndn-python-repo.conf > debug/repo.log 2>&1 &

sleep 1

echo "start controller"
mguard-controllerApp > debug/controller.log 2>&1 &

sleep 1

# valgrind --leak-check=full --log-file=valgrind-out.txt ./build/examples/mguard-producerApp > debugging/p.log 2>&1 &
echo "start producer"
mguard-producerApp > debug/producer.log 2>&1 &

sleep 4

echo "run data generator"
python data-generator/CerebralCortex-Random-Data-Generator/main.py > debug/generator.log 2>&1 &

sleep 0.1

echo "run consumer"
mguard-consumerApp -c certs/local.cert -p /ndn/org/md2k/local > debug/consumer.log 2>&1 &
