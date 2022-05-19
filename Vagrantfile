# -*- mode: ruby -*-
# vi: set ft=ruby :
Vagrant.configure("2") do |config|
  config.vm.define "mguard-dev-t"
  config.vm.box = "bento/ubuntu-20.04"
  config.vm.hostname = "mguard-dev"
  config.vm.provider "virtualbox" do |vb|
    vb.name = "mguard"
    vb.cpus = "6"
    vb.memory = "16384"
  end
  config.vm.provision "shell", privileged: false, inline: <<-SHELL
	sudo apt update && sudo apt -y upgrade
	sudo apt-get -y install gcc-10 g++-10 clang-12 lldb-12 lld-12 build-essential \
				    pkg-config python3-minimal libboost-all-dev \
							libssl-dev libsqlite3-dev libpcap-dev \
									    libsodium-dev libz-dev \
												liblog4cxx-dev libpcap-dev python-is-python3
	sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 100
	sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-10 100
	sudo update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++-10 100
	sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-12 100
	sudo update-alternatives --install /usr/bin/lld lld /usr/bin/lld-12 100
	sudo update-alternatives --install /usr/bin/lldb lldb /usr/bin/lldb-12 100
	git clone https://github.com/named-data/ndn-cxx
	sudo git config --global --add safe.directory $HOME/ndn-cxx
	cd ndn-cxx
	./waf configure
	./waf
	sudo ./waf install
	sudo ldconfig
	cd ..
	git clone https://github.com/named-data/NFD
	sudo git config --global --add safe.directory $HOME/NFD
	cd NFD
	git submodule update --init
	./waf configure
	./waf
	sudo ./waf install
	sudo cp /usr/local/etc/ndn/nfd.conf.sample /usr/local/etc/ndn/nfd.conf
	cd ..
	sudo apt-get -y install libgtest-dev cmake python3-pip
	git clone https://github.com/zeutro/openabe.git
	cd openabe
	sudo -E ./deps/install_pkgs.sh
	. ./env

	cd deps/relic
	make
	cd ../..

	NO_DEPS=1 BISON=$(which bison) make
	make test
	sudo -E make install
	cd ..

	git clone https://github.com/dulalsaurab/nac-abe
	cd nac-abe
	./waf configure
	./waf
	sudo ./waf install
	sudo ldconfig
	cd ..

	git clone https://github.com/named-data/psync.git
	sudo git config --global --add safe.directory $HOME/psync
	cd psync
	./waf configure
	./waf
	sudo ./waf install
	cd ..

	git clone https://github.com/named-data/ndn-tools/
	sudo git config --global --add safe.directory $HOME/ndn-tools
	cd ndn-tools
	./waf configure
	./waf
	sudo ./waf install
	cd ..

	#   latest development version of ndn-python-repo
	git clone https://github.com/jonnykong/ndn-python-repo.git
	cd ndn-python-repo && /usr/bin/pip3 install -e .
	cd ..

	git clone git@gitlab.com:netlab-memphis/sdulal/mguard.git
	cd mguard
	# installing mGuard
	./waf configure --with-examples
	./waf build && sudo ./waf install && sudo ldconfig

	# Creating Identifies/Certificates

	# publisher
	#ndnsec key-gen -t r /ndn/org/md2k
	# controller
	#ndnsec key-gen -t r /ndn/org/md2k/mguard/controller
	# Consumer
	#ndnsec key-gen -t r /ndn/org/md2k/A
	# Attribute Authority
	#ndnsec key-gen -t r /ndn/org/md2k/mguard/aa

	# md2k data generator
	cd data-generator
	git clone https://github.com/MD2Korg/CerebralCortex-Random-Data-Generator
	mv CerebralCortex-Random-Data-Generator CerebralCortexRandomDataGenerator
	pip install findspark
	pip install cerebralcortex-kernel
	pip install MarkupSafe==2.0.1
	sudo apt -y install default-jdk
	cd ../..

  SHELL
end
