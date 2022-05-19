from time import sleep

from mininet.log import setLogLevel, info
from minindn.minindn import Minindn
from minindn.util import MiniNDNCLI
from minindn.apps.app_manager import AppManager
from minindn.apps.nfd import Nfd
from minindn.apps.nlsr import Nlsr


def run_server(node):
    info("Running server \n")
    # repo
    # logs are saved in  tmp/minindn/a 
    # node.cmd('cd ~/mguard/mguard/')
    # node.cmd('pwd > initial.log 2>&1 &')

    node.cmd('ndn-python-repo -c ~/home/map901/mguard/mguard/ndn-python-repo.conf > repo.log 2>&1 &')
    # controller
    node.cmd('mguard-controllerApp > controller.log 2>&1 &')
    # producer
    node.cmd('mguard-producerApp > producer.log 2>&1 &')
    # send data to producer
    node.cmd('cd data-generator && python main.py && cd ..')
    info("server run complete \n")


def run_consumer(node):
    info("running consumer\n")
    # logs are saved in tmp/minindn/b 
    node.cmd('mguard-consumerApp > consumer.log 2>&1 &')
    info("consumer run complete \n")



if __name__ == '__main__':
    setLogLevel('info')
    ndn = Minindn()
    args = ndn.args

    ndn.start()
    info("Starting NFD")
    sleep(2)
    nfds = AppManager(ndn, ndn.net.hosts, Nfd, logLevel='DEBUG')
    nlsrs = AppManager(ndn, ndn.net.hosts, Nlsr)

    sleep(50)
    server = ndn.net['a']
    consumer = ndn.net['b']

    # advertise producer prefix
    producerPrefix = "/ndn/org/md2k"
    server.cmd('nlsrc advertise {} > advertise.log 2>&1 &'.format(producerPrefix))
    sleep(5)


    run_server(server)
    # wait for all data to be sent (maybe 140 seconds) before running consumer
    sleep(140)
    run_consumer(consumer)

    ndn.stop()