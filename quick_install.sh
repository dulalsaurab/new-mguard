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

# md2k data generator
cd data-generator
git clone https://github.com/MD2Korg/CerebralCortex-Random-Data-Generator
mv CerebralCortex-Random-Data-Generator CerebralCortexRandomDataGenerator
pip install findspark
pip install cerebralcortex-kernel
sudo apt install default-jdk
cd ..
