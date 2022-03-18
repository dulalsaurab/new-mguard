# -*- mode: ruby -*-
# vi: set ft=ruby :
Vagrant.configure("2") do |config|
<<<<<<< HEAD
  config.vm.define "mguard-dev-t" 
=======
  config.vm.define "mguard-dev-t"
>>>>>>> e3be44cd181c8c00b8bbbf2cf17b108106f4a755
  config.vm.box = "bento/ubuntu-20.04"
  config.vm.hostname = "mguard-dev"
  config.vm.provider "virtualbox" do |vb|
    vb.name = "mguard"
<<<<<<< HEAD
    vb.cpus = "8"
=======
    vb.cpus = "6"
>>>>>>> e3be44cd181c8c00b8bbbf2cf17b108106f4a755
    vb.memory = "16384"
  end
  config.vm.provision "shell", privileged: false, inline: <<-SHELL
    sudo apt-get update
    sudo apt-get -y install gcc-10 g++-10 clang-12 lldb-12 lld-12 build-essential \
                            pkg-config python3-minimal libboost-all-dev \
                            libssl-dev libsqlite3-dev libpcap-dev \
                            libsodium-dev libz-dev \
<<<<<<< HEAD
                            liblog4cxx-dev
=======
                            liblog4cxx-dev libpcap-dev python-is-python3
>>>>>>> e3be44cd181c8c00b8bbbf2cf17b108106f4a755
    sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 100
    sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-10 100
    sudo update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++-10 100
    sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-12 100
    sudo update-alternatives --install /usr/bin/lld lld /usr/bin/lld-12 100
    sudo update-alternatives --install /usr/bin/lldb lldb /usr/bin/lldb-12 100
    git clone https://github.com/named-data/ndn-cxx
    cd ndn-cxx
    ./waf configure
    ./waf
    sudo ./waf install
    sudo ldconfig
    cd ..
<<<<<<< HEAD
    
=======

>>>>>>> e3be44cd181c8c00b8bbbf2cf17b108106f4a755
    git clone https://github.com/named-data/NFD
    cd NFD
    git submodule update --init
    ./waf configure
    ./waf
    sudo ./waf install
    sudo cp /usr/local/etc/ndn/nfd.conf.sample /usr/local/etc/ndn/nfd.conf
    cd ..
<<<<<<< HEAD
    
=======

>>>>>>> e3be44cd181c8c00b8bbbf2cf17b108106f4a755
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
<<<<<<< HEAD
   
    git clone https://github.com/UCLA-IRL/NAC-ABE.git
    cd NAC-ABE
=======

    git clone https://github.com/UCLA-IRL/NAC-ABE.git
    cd NAC-ABE
    echo "
    diff --git a/src/consumer.cpp b/src/consumer.cpp
    index dfdf9b4..3fc4f62 100644
    --- a/src/consumer.cpp
    +++ b/src/consumer.cpp
    @@ -75,12 +75,13 @@ Consumer::consume(const Name& dataName,
                       const ErrorCallback& errorCallback)
     {
       Interest interest(dataName);
    -  interest.setMustBeFresh(true);
    +  // interest.setMustBeFresh(true);
       interest.setCanBePrefix(true);

       NDN_LOG_INFO(m_cert.getIdentity() << \" Ask for data \" << interest.getName() );
       m_face.expressInterest(interest,
                              [=] (const Interest&, const Data& data) {
    +                          NDN_LOG_INFO(\"Got the data packet back\");
                                decryptContent(data, consumptionCb, errorCallback);
                              }, nullptr, nullptr);
     }
    @@ -105,10 +106,11 @@ Consumer::decryptContent(const Data& data,
       NDN_LOG_INFO(\"CK Name is \" << ckName);

       Interest ckInterest(ckName);
    -  ckInterest.setMustBeFresh(true);
    +  // ckInterest.setMustBeFresh(true);
       ckInterest.setCanBePrefix(true);
       m_face.expressInterest(ckInterest,
                              [=] (const Interest&, const Data& data) {
    +                           NDN_LOG_INFO(\"Got the cKData packet back\");
                                onCkeyData(data, cipherText, successCallBack, errorCallback);
                              }, nullptr, nullptr);
     }
    " > patch.diff
    git apply patch.diff
>>>>>>> e3be44cd181c8c00b8bbbf2cf17b108106f4a755
    mkdir build && cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make
    sudo make install
    cd ../../
<<<<<<< HEAD
    
=======

>>>>>>> e3be44cd181c8c00b8bbbf2cf17b108106f4a755
    git clone https://github.com/named-data/PSync.git
    cd PSync
    ./waf configure
    ./waf
    sudo ./waf install
    cd ..
<<<<<<< HEAD
    
  SHELL
end
=======

    git clone https://github.com/named-data/ndn-tools/
    cd ndn-tools
    ./waf configure
    ./waf
    sudo ./waf install

#   latest development version of ndn-python-repo
    git clone https://github.com/JonnyKong/ndn-python-repo.git
    cd ndn-python-repo && /usr/bin/pip3 install -e .
    cd ..

  SHELL
end
>>>>>>> e3be44cd181c8c00b8bbbf2cf17b108106f4a755
