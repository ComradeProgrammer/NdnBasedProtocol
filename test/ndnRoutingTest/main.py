from mininet.topo import Topo
from mininet.net import Mininet
from mininet.net import Host
from mininet.util import dumpNodeConnections
from TwoRouterFork import TwoRouterFork, run2
from mininet.log import setLogLevel
import time


if __name__ == '__main__':
    # Tell mininet to print useful information
    setLogLevel('info')
    run2()
