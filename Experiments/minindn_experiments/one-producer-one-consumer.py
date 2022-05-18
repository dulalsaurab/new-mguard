from time import sleep

from mininet.log import setLogLevel, info
from minindn.minindn import Minindn
from minindn.util import MiniNDNCLI
from minindn.apps.app_manager import AppManager
from minindn.apps.nfd import Nfd
from minindn.apps.nlsr import Nlsr


def run_server(node):
    info("Running server")
    # repo
    node.cmd('ndn-python-repo -c ndn-python-repo.conf > repo.log 2>&1 &')

    # controller
    node.cmd('mguard-controllerApp > controller.log 2>&1 &')
    # producer
    node.cmd('mguard-producerApp')
    # send data to producer
    node.cmd('cd data-generator && python main.py && cd ..')
    info("server run complete")


def run_consumer(node):
    info("running consumer")
    node.cmd('mguard-consumerApp')
    info("consumer run complete")



if __name__ == '__main__':
    setLogLevel('info')
    ndn = Minindn()
    args = ndn.args

    # testFile = "/home/mini-ndn/europa_bkp/mini-ndn/ndndump.txt"
    # testFile = '/home/map901/mguard/mguard/logs/log.dat'
    # testFile = "/home/mini-ndn/europa_bkp/mini-ndn/sdulal_new/multicast-supression-ndn/files/output.dat"
  
    ndn.start()
    info("Starting NFD")
    sleep(2)
    nfds = AppManager(ndn, ndn.net.hosts, Nfd, logLevel='DEBUG')

    nlsrs = AppManager(ndn, ndn.net.hosts, Nlsr)

    sleep(50)
    server = ndn.net['a']  # , ndn.net["sta3"]]
    consumer = ndn.net['b']

    run_server(server)
    # wait for all data to be sent (maybe 140 seconds) before running consumer
    sleep(140)
    run_consumer(consumer)

    ndn.stop()