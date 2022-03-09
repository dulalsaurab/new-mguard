# installing mGuard
./waf configure --with-examples
./waf build && sudo ./waf install

# Creating Identifies/Certificates

# publisher
ndn-sec key-gen -t r /org/md2k
# controller
ndn-sec key-gen -t r /mguard/controller
# Consumer
ndn-sec key-gen -t r /org/md2k/A
# Attribute Authority
ndn-sec key-gen -t r /mguard/aa
