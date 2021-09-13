from mininet.topo import Topo
from mininet.net import Mininet
from mininet.net import Host
from mininet.util import dumpNodeConnections
from mininet.log import setLogLevel
import time


class SingleSwitchTopo(Topo):
    "Single switch connected to n hosts."

    def build(self, n=2):
        switch1 = self.addSwitch('s1')
        switch2=self.addSwitch("s2")
        self.addLink(switch1,switch2)
        # Python's range(N) generates 0..N-1


def main():
    "Create and test a simple network"
    topo = SingleSwitchTopo(n=4)
    net = Mininet(topo)
    net.start()
    s1, s2 = net.get("s1", "s2")

    process1 = s1.popen(["../../build/ndnTest","s1"])
    process2 = s1.popen(["../../build/ndnTest","s2"])
    while True:
        time.sleep(1)

    net.stop()

    if process.poll() == 0:
        print("[E2E TEST] ndnTransmitterTest: SUCCESS")
    else:
        print("[E2E TEST] ndnTransmitterTest: FAIL with return value %d" %
              (process.returncode))


if __name__ == '__main__':
    # Tell mininet to print useful information
    setLogLevel('info')
    main()
