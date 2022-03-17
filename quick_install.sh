# installing mGuard
./waf configure --with-examples
./waf build && sudo ./waf install
sudo ldconfig

# Creating Identifies/Certificates

# publisher
ndnsec key-gen -t r /org/md2k
# controller
ndnsec key-gen -t r /mguard/controller
# Consumer
ndnsec key-gen -t r /org/md2k/A
# Attribute Authority
ndnsec key-gen -t r /mguard/aa
