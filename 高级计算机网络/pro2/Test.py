#!/usr/bin/python

from mininet.net import Mininet
from mininet.node import Controller, RemoteController, OVSController
from mininet.node import CPULimitedHost, Host, Node
from mininet.node import OVSKernelSwitch, UserSwitch
from mininet.cli import CLI
from mininet.log import setLogLevel, info
from mininet.link import TCLink, Intf
from subprocess import call

def myNetwork():

    net = Mininet( topo=None,
                   build=False,
                   ipBase='10.0.0.0/8')

    info( '*** Adding controller\n' )
    
    c0=net.addController(name='c0',
                      controller=RemoteController,
                      protocol='tcp',
                      port=6633)

    info( '*** Add switches\n')
    
    s1 = net.addSwitch('s1', cls=OVSKernelSwitch)
    s2 = net.addSwitch('s2', cls=OVSKernelSwitch)
    
    info( '*** Add hosts\n')
    h1 = net.addHost('h1', cls=Host, ip='10.0.0.1', defaultRoute=None)
    h2 = net.addHost('h2', cls=Host, ip='10.0.0.2', defaultRoute=None)
    
    info( '*** Add links\n')
    net.addLink(s1, h1)
    net.addLink(s2, h2)
    net.addLink(s1, s2)

    info( '*** Starting network\n')
    net.build()
    info( '*** Starting controllers\n')
    for controller in net.controllers:
        controller.start()

    info( '*** Starting switches\n')
    net.get('s1').start([c0])
    net.get('s2').start([c0])

    info( '*** Post configure switches and hosts\n')
    
#    net.configLinkStatus('s1', 's2','down')
#    net.pingAll()
#    net.configLinkStatus('s1', 's2','up')
#    net.pingAll()
 
#    s1.cmd(r'ovs-ofctl add-flow s1 in_port=1,actions=output:2' )
#    s1.cmd(r'ovs-ofctl add-flow s1 in_port=2,actions=output:1' )
#    s2.cmd(r'ovs-ofctl add-flow s2 in_port=1,actions=output:2' )
#    s2.cmd(r'ovs-ofctl add-flow s2 in_port=2,actions=output:1' )
 
    CLI(net)
    net.stop()

if __name__ == '__main__':
    setLogLevel( 'info' )
    myNetwork()

