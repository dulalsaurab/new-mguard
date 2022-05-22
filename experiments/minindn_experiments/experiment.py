from mininet.log import setLogLevel, info
from minindn.minindn import Minindn
from minindn.util import MiniNDNCLI
from minindn.apps.app_manager import AppManager
from minindn.apps.nfd import Nfd
from minindn.apps.nlsr import Nlsr

from time import sleep
import subprocess
import os
from pathlib import Path

policyPath = "../../policies/policy1"
certPath = "../../certs"
availableStreamPath = "../../policies/available_streams"
safeBagPath = "../../safebag"
sb_core_list = ["producer.sb", "controller.sb", "aa.sb"]
producerPrefix = "/ndn/org/md2k"
data_generator_path = "/home/map901/mguard/mguard/data-generator"

def importSafebag(node, args, isProducer):
    safebagDir = args.workDir+"/"+node.name+"/safebag"

    # if poroducer, import all safebags, else just what's needed
    source = Path(safebagDir)
    sbgs = [x.name for x in source.iterdir()]
    if (isProducer):
        for sb in sbgs:
            cmd = "ndnsec import safebag/{} -P lab221".format(sb)
            node.cmd(cmd)
    else:
        # sb_core_list.append(node.name.upper()+".sb")
        # for sb in sb_core_list:
        #     print ("importing sb", sb)
        cmd = "ndnsec import safebag/{} -P lab221".format(node.name.upper()+".sb")
        node.cmd(cmd)

def run_repo_n_generator(node):
    # run repo
    print ("Starting repo")
    command = "ndn-python-repo -c /home/map901/mguard/mguard/ndn-python-repo.conf > repo.log 2>&1 &"
    node.cmd(command)
    sleep(3) # sleep to init repo properly

    # send data to producer
    print("Starting data generator")
    node.cmd("export SPARK_HOME='/home/map901/.local/lib/python3.8/site-packages/pyspark'")
    node.cmd('python {}/main.py > generator.log 2>&1 &'.format(data_generator_path))

def run_server(node, args):
    info("Running server \n")
    node_dir = args.workDir+"/"+node.name
    
    node.cmd("mkdir -p policies")
    policy_dir = node_dir+"/policies"
    subprocess.run(["cp", policyPath, policy_dir])
    subprocess.run(["cp", availableStreamPath, policy_dir])

    importSafebag(node, args, True)
    
    # advertise producer prefix
    node.cmd('nlsrc advertise {}'.format(producerPrefix))
    sleep(20) # enough sleep time for prefix propagation

    # controller
    node.cmd('mguard-controllerApp > controller.log 2>&1 &')
    sleep(2)

    # producer
    node.cmd('mguard-producerApp > producer.log 2>&1 &')
    sleep(1)

    info("controller and producer started on the server\n")
    

def run_consumer(consumers):
    info("running consumer\n")
    for node in consumers:
        consumerName = node.name.upper()
        cmd = "mguard-consumerApp -c certs/{}.cert -p /ndn/org/md2k/{}> consumer.log 2>&1 &".format(consumerName, consumerName)
        node.cmd(cmd)
        sleep(2)
        importSafebag(node, args, False)
    
    info("consumer run complete \n")

if __name__ == '__main__':
    setLogLevel('info')
    subprocess.run(["mn", "-c"])
    subprocess.run(["rm", "-rf", "/tmp/minindn/*"])

    ndn = Minindn()
    args = ndn.args

    ndn.start()
    info("Starting NFD")
    sleep(2)
    nfds = AppManager(ndn, ndn.net.hosts, Nfd, logLevel='DEBUG')
    nlsrs = AppManager(ndn, ndn.net.hosts, Nlsr, logLevel='DEBUG')
    sleep (120) # nlsr convergence

    # enable logs in all the nodes
    for host in ndn.net.hosts:
        host.cmd("export NDN_LOG=mguard.*=TRACE:nacabe.*=TRACE")

        # copy certs and safe bags in the node folder
        host_dir = args.workDir+"/"+host.name
        subprocess.run(["cp", "-r", certPath, host_dir])
        subprocess.run(["cp", "-r", safeBagPath, host_dir])

    c = ["b", "c", "d", "e"] # these are the consumer, look at testbed.conf topology
    producer = ndn.net["a"]
    consumers = [ndn.net[x] for x in c] 
    # consumers = [y for y in ndn.net.hosts if y.name not in [producer.name]]
    
    run_server(producer, args)
    sleep(5)
    run_consumer(consumers)

    # finally start ndn-python-repo
    run_repo_n_generator(producer)

    MiniNDNCLI(ndn.net)
    ndn.stop()