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
    echo "deb [arch=amd64 trusted=yes] https://nfd-nightly-apt.ndn.today/ubuntu focal main" | sudo tee /etc/apt/sources.list.d/nfd-nightly.list

    sudo apt update
    sudo apt -y install nfd libndn-cxx-dev libpsync-dev ndn-tools python-is-python3 libgtkmm-3.0-dev libboost-all-dev
    sudo apt -y upgrade

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
    git switch everything
    ./waf configure && ./waf build && sudo ./waf install
    cd ..
    
    # mkdir build && cd build
    # cmake -DCMAKE_BUILD_TYPE=Release ..
    # make
    # sudo make install
    # cd ../../

#   latest development version of ndn-python-repo
    sudo apt -y install python3.9
    git clone https://github.com/JonnyKong/ndn-python-repo.git
    cd ndn-python-repo && /usr/bin/python3.9 -m pip install -e .
    cd ..

    # installing mGuard
    git clone https://github.com/dulalsaurab/new-mguard mguard
    cd mguard
    ./waf configure --with-examples && ./waf build && sudo ./waf install

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
    sudo apt -y install default-jdk
    cd ..

  SHELL
end

