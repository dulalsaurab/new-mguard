# installing mGuard
./waf configure --with-examples
./waf build && sudo ./waf install

# Creating Identifies/Certificates
ndnsec key-gen -t r /ndn/org/md2k
ndnsec cert-dump -i /ndn/org/md2k > certs/producer.cert

ndnsec key-gen -t r /ndn/org/md2k/mguard/controller
ndnsec cert-dump -i /ndn/org/md2k/mguard/controller > certs/controller.cert

ndnsec key-gen -t r /ndn/org/md2k/mguard/aa
ndnsec cert-dump -i /ndn/org/md2k/mguard/aa > certs/aa.cert

ndnsec key-gen -t r /ndn/org/md2k/A
ndnsec sign-req /ndn/org/md2k/A > A.ndncsr

ndnsec cert-gen -s /ndn/org/md2k -i /ndn/org/md2k A.ndncsr > A.cert

cp A.cert certs/
ndnsec cert-install A.cert

# md2k data generator
cd data-generator || exit
git clone https://github.com/MD2Korg/CerebralCortex-Random-Data-Generator
mv CerebralCortex-Random-Data-Generator CerebralCortexRandomDataGenerator
pip install findspark
pip install cerebralcortex-kernel
sudo apt install default-jdk
cd ..
