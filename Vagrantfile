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
    sudo apt -y install nfd libndn-cxx-dev libpsync-dev ndn-tools
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
    mkdir build && cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make
    sudo make install
    cd ../../

#   latest development version of ndn-python-repo
    sudo apt -y install python3.9
    git clone https://github.com/JonnyKong/ndn-python-repo.git
    cd ndn-python-repo && /usr/bin/python3.9 -m pip install -e .
    cd ..

  SHELL
end
