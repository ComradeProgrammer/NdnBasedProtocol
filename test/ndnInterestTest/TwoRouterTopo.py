
from mininet.topo import Topo
from mininet.net import Mininet
from mininet.net import Host
from mininet.util import dumpNodeConnections
from mininet.log import setLogLevel
import time


class TwoRouterTopo(Topo):
    "Single switch connected to n hosts."

    def build(self, n=2):
        switch1 = self.addSwitch('s1')
        switch2 = self.addSwitch("s2")
        self.addLink(switch1, switch2)
        # Python's range(N) generates 0..N-1


def run1():
    "Create and test a simple network"
    topo = TwoRouterTopo(n=4)
    net = Mininet(topo)
    net.start()
    s1, s2 = net.get("s1", "s2")

    process1 = s1.popen(["../../build/ndnTest", "s1"])
    process2 = s1.popen(["../../build/ndnTest", "s2"])

    time.sleep(10)

    net.stop()
