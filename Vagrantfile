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
    sudo apt update && sudo apt upgrade

    echo "deb [arch=amd64 trusted=yes] https://nfd-nightly-apt.ndn.today/debian bullseye main" | sudo tee /etc/apt/sources.list.d/nfd-nightly.list
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
    git switch everything
    cd nac-abe
    mkdir build && cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make
    sudo make install

#   latest development version of ndn-python-repo
    git clone https://github.com/JonnyKong/ndn-python-repo.git
    cd ndn-python-repo && /usr/bin/pip3 install -e .
    cd ..

  SHELL
end
