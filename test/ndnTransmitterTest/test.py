from mininet.topo import Topo
from mininet.net import Mininet
from mininet.net import Host
from mininet.util import dumpNodeConnections
from mininet.log import setLogLevel
import time


class SingleSwitchTopo(Topo):
    "Single switch connected to n hosts."

    def build(self, n=2):
        switch = self.addSwitch('s1')
        # Python's range(N) generates 0..N-1
        for h in range(n):
            host = self.addHost('h%s' % (h + 1))
            self.addLink(host, switch)


def main():
    "Create and test a simple network"
    topo = SingleSwitchTopo(n=4)
    net = Mininet(topo)
    net.start()
    h1, h2 = net.get("h1", "h2")

    process = h1.popen("../../build/ndnTransmitterTestRecv")
    time.sleep(1)
    h2.cmd("../../build/ndnTransmitterTestSend")
    while process.poll() is None:
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
