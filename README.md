# mGuard
### 1. Overview
Named Data Networking (NDN) Pub-Sub library with builtin security and access control

### 2. Installation
#### 2.1  Dependencies
(need to install following libraries)
- [ndn-cxx](https://github.com/named-data/ndn-cxx) - NDN C++ library with eXperimental eXtensions
- [ndn-tools](https://github.com/named-data/ndn-tools) - NDN Essential Tools, used to create identity and certs using ndnsec
- [nfd](https://github.com/named-data/nfd), NDN forwarding daemon
- [PSync](https://github.com/named-data/psync), Partial and Full Synchronization Library for NDN, used for pub-sub
- [ndn-python-repo](https://ndn-python-repo.readthedocs.io/en/latest/), used to store encrypted application data and manifes
(repo configuration file: https://gitlab.com/netlab-memphis/sdulal/mguard/-/blob/master/ndn-python-repo.conf)
- [NAC-ABE](https://github.com/UCLA-IRL/NAC-ABE.git), used for attribute based encryption and access control

Please follow the corresponding links for the installaion details.
### 2.2 Install mGuard
```bash
- git clone https://gitlab.com/netlab-memphis/sdulal/mguard.git && cd mguard
- ./waf configure --with-examples
(configure with the examples, we will need them for testing purposes)
- ./waf build && sudo ./waf install
```
### 2.3 Creating example identities/certificates
1. mGuard Publisher, identity: /org/md2k
2. mGuard Controller, identity: /mguard/controller
3. Consumer, identity: /org/md2k/A
4. Attribute Authority, identity: /mguard/aa

Run the following command to create a cert for each of the above identity
`ndnsec key-gen <identity name>`. This will create and store the cert in the default location i.e. ~/.ndn

Note: Above identities are just an example, not the strict requirement. Our examples use these identities so they are needed to run the examples properly.

### 3. Testing
(run the follwing programs sequencally)

1. Start nfd `nfd-start` 
(if running the nfd for the first time, make sure to copy 
 `cp /usr/local/etc/ndn/nfd.conf.sample /usr/local/etc/ndn/nfd.conf`, 
 assuming the nfd is installed from source)
2. run ndn-python-repo
`ndn-python-repo -c ndn-python-repo.conf`
3. run controller
`./build/examples/mguard-controllerApp`
4. run producer app
`./build/examples/mguard-producerApp`
5. run consumer app
`./build/examples/mguard-consumerApp`

If everything goes well, you should see the following (as of Feb 17, 22)

[![image alt text](https://gitlab.com/netlab-memphis/sdulal/mguard/-/raw/master/docs/Screen_Shot_2022-02-17_at_7.11.00_PM.png)]




