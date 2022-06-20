from mininet.topo import Topo
from mininet.net import Mininet
from mininet.net import Host
from mininet.util import dumpNodeConnections
from MyTopo import MyTopo, run
from mininet.log import setLogLevel
import time
import psutil

if __name__ == '__main__':
    # Tell mininet to print useful information
    tmp=psutil.cpu_times()
    setLogLevel('info')
    run()
    tmp2=psutil.cpu_times()
    print("cpu",tmp2.user-tmp.user)
    print(tmp)
    print(tmp2)

