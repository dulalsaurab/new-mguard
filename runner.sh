export NDN_LOG=mguard.*=DEBUG:nacabe.*=TRACE:psync.*=DEBUG


rm debugging/*setUpdateManifestName


nfd-stop

sleep 2

rm -rf ~/.ndn/ndn-python-repo/sqlite3.db

nfd-start &>/dev/null

sleep 1

ndn-python-repo -c ndn-python-repo.conf > debugging/r.log 2>&1 &

sleep 1

./build/examples/mguard-controllerApp > debugging/c.log 2>&1 &

sleep 1

valgrind --leak-check=full --log-file=valgrind-out.txt ./build/examples/mguard-producerApp > debugging/p.log 2>&1 &

sleep 1

python data-generator/main.py > debugging/g.log 2>&1 &

sleep 0.1

./build/examples/mguard-consumerApp -c certs/A.cert -p /ndn/org/md2k/A > debugging/con.log 2>&1 &
