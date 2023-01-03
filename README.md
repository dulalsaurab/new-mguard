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
- **[NAC-ABE, /dulalsaurab/NAC-ABE/tree/everything](https://github.com/dulalsaurab/NAC-ABE/tree/everything)**, used for attribute based encryption and access control
    - Note: original NAC-ABE repository is at: https://github.com/UCLA-IRL/NAC-ABE.git

Please follow the corresponding links for the installaion details.
### 2.2 Install mGuard
```bash
- git clone https://gitlab.com/netlab-memphis/sdulal/mguard.git && cd mguard
- ./waf configure --with-examples
(configure with the examples, we will need them for testing purposes)
- ./waf build && sudo ./waf install
```
### 2.3 Creating example identities/certificates

#### Certificates for mGuard Server
1. mGuard Publisher, identity: `/ndn/org/md2k` (trust anchor)
    - Consumer's certificate will be signed by publiser's cert 
2. mGuard Controller, identity: `/ndn/org/md2k/mguard/controller`
3. Attribute Authority, identity: `/ndn/org/mguard/aa`

Run the following command to generate keys (public-private pairs) for each of the above identity
- `ndnsec key-gen -t r <identity name>`. This will create and store the cert in the default location i.e. ~/.ndn

NOTE: `-t r` flag is used to create RSA keys (required by NAC-ABE), by default ECDSA are created by `ndnsec key-gen`

- Dump the certificate to the `certs` folder `ndnsec cert-dump -i <identity name> > certs/<name>.cert`

#### Certificates for consumer

- create identity at consumer: `ndnsec key-gen -t r /ndn/org/md2k/A`
- create sign request at consumer:  `ndnsec sign-req /ndn/org/md2k/A > A.ndncsr`
- copy the `A.ndncsr` to producer
- generate consumer cert signed by producer `ndnsec cert-gen -s /ndn/org/md2k -i /ndn/org/md2k A.ndncsr > A.cert` at producer
- copy the `A.cert` to consumer i.e. to `certs` directory
- Install consumer cert at consumer: `ndnsec cert-install A.cert`

Note: Above identities are just an example, not the strict requirement. Our examples use these identities so they are needed to run the examples properly.


### 3. Testing
(run the follwing programs sequencally)

Enable logs (mguard and nac-abe log): 
 - `export NDN_LOG=mguard.*=TRACE:nacabe.*=TRACE`

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

`./build/examples/mguard-consumerApp -p <consumer-prefix> -c <consumer-cert-path>`

e.g. `./build/examples/mguard-consumerApp -p /ndn/org/md2k/A -c certs/A.cert`

If everything goes well, you should see the following (as of Dec 22, 22)

![image alt text](https://gitlab.com/netlab-memphis/sdulal/mguard/-/raw/master/docs/Screen_Shot_2022-02-17_at_7.11.00_PM.png)

# Paper
```
@inproceedings{dulal2022building,
  title={Building a secure mhealth data sharing infrastructure over ndn},
  author={Dulal, Saurab and Ali, Nasir and Thieme, Adam Robert and Yu, Tianyuan and Liu, Siqi and Regmi, Suravi and Zhang, Lixia and Wang, Lan},
  booktitle={Proceedings of the 9th ACM Conference on Information-Centric Networking},
  pages={114--124},
  year={2022}
}
```




## Issue Report
https://redmine.named-data.net/projects/mguard/issues


## Aditional Helpful Resources
	- https://github.com/named-data/ndn-cxx/tree/master/examples




