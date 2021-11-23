
import mininet.cli
from mininet.topo import Topo
from mininet.node import Node
from mininet.link import TCLink, Link,TCIntf,Intf
import time



class MyTopo(Topo):
    "Single switch connected to n hosts."

    def build(self, n=2):
        switch1 = self.addHost('s1')
        switch2 = self.addHost("s2")
        switch3 = self.addHost('s3')


        self.addLink(switch1, switch2, params1={ 'ip' : '192.168.0.1/24' }, params2={ 'ip' : '192.168.0.2/24'})
        self.addLink(switch2, switch3, params1={ 'ip' : '192.168.1.1/24' }, params2={ 'ip' : '192.168.1.2/24'})
        self.addLink(switch1, switch3, params1={ 'ip' : '192.168.2.1/24' }, params2={ 'ip' : '192.168.2.2/24'})






topos = {"mytopo": (lambda: MyTopo())}
# sudo mn --custom FourRouterFork.py --topo mytopo
# mininet> s1 ifconfig s1-eth0 192.168.0.1 netmask 255.255.255.0
# mininet> s2 ifconfig s2-eth0 192.168.0.2 netmask 255.255.255.0
# mininet> s3 ifconfig s3-eth0 192.168.1.2 netmask 255.255.255.0

#route add -net 192.168.0.0/24 gw 192.168.1.1
