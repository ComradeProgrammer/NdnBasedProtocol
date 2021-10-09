
from mininet.topo import Topo
from mininet.net import Mininet
from mininet.net import Host
from mininet.util import dumpNodeConnections
from mininet.log import setLogLevel
import time


class TwoRouterFork(Topo):
    "Single switch connected to n hosts."

    def build(self, n=2):
        switch1 = self.addHost('s1')
        switch2 = self.addHost("s2")

        self.addLink(switch1, switch2)

        #self.addLink(switch4, switch1)
        # Python's range(N) generates 0..N-1


def run2():
    "Create and test a simple network"
    topo = TwoRouterFork(n=4)
    net = Mininet(topo)
    net.start()
    s1, s2, = net.get("s1", "s2")

    f = open("connection.log", "w")
    for link in net.links:
        f.write(str(link.intf1)+","+str(link.intf2)+"\n")
    f.close()

    process1 = s1.popen(["../../build/ndnRoutingTest", "s1"])
    time.sleep(0.01)
    process2 = s2.popen(["../../build/ndnRoutingTest", "s2"])


    time.sleep(100)
    process1.kill()
    process2.kill()



    # net.stop()
topos = {"mytopo": (lambda: TwoRouterFork())}
# sudo mn --custom FourRouterFork.py --topo mytopo
