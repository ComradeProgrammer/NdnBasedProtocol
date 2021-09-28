from mininet.topo import Topo
from mininet.net import Mininet
from mininet.net import Host
from mininet.util import dumpNodeConnections
from mininet.log import setLogLevel
from TwoRouterTopo import TwoRouterTopo, run1
from FourRouterCircle import FourRouterCircle, run2
import time


if __name__ == '__main__':
    # Tell mininet to print useful information
    setLogLevel('info')
    run2()
