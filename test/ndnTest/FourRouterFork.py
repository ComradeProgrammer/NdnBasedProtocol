
from mininet.topo import Topo
from mininet.net import Mininet
from mininet.net import Host
from mininet.util import dumpNodeConnections
from mininet.log import setLogLevel
import time


class FourRouterFork(Topo):
    "Single switch connected to n hosts."

    def build(self, n=2):
        switch1 = self.addHost('s1')
        switch2 = self.addHost("s2")
        switch3 = self.addHost("s3")
        switch4 = self.addHost("s4")
        self.addLink(switch1, switch2)
        self.addLink(switch2, switch3)
        self.addLink(switch2, switch4)
        #self.addLink(switch4, switch1)
        # Python's range(N) generates 0..N-1
def run2():
    "Create and test a simple network"
    topo = FourRouterFork(n=4)
    net = Mininet(topo)
    net.start()
    s1, s2,s3,s4 = net.get("s1", "s2","s3","s4")
    
    f=open("connection.log","w")
    for link in net.links:
        f.write(str(link.intf1)+","+str(link.intf2)+"\n")

    process1 = s1.popen(["../../build/ndnTest","s1"])
    process2 = s2.popen(["../../build/ndnTest","s2"])
    process3 = s3.popen(["../../build/ndnTest","s3"])
    process4 = s4.popen(["../../build/ndnTest","s4"])
    
    time.sleep(10)
    process1.kill()
    process2.kill()
    process3.kill()
    process4.kill()
    
    #net.stop()
topos={"mytopo":(lambda:FourRouterFork())}