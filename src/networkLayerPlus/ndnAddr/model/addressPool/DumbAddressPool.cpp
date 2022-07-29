#include "DumbAddressPool.h"
#include<iostream>
using namespace std;
DumbAddressPool::DumbAddressPool() { ptr.setIp("192.168.0.0"); }
DumbAddressPool::DumbAddressPool(Ipv4Address start){
    ptr=start;
}

void DumbAddressPool::addAddressBlock(Ipv4Address _address, Ipv4Address _mask) {}
std::pair<Ipv4Address, Ipv4Address> DumbAddressPool::allocateAddress(int size) {
    Ipv4Address tmp = ptr;
    Ipv4Address mask;
    mask.setIp("255.255.255.252");
    ptr.addr = htonl(ntohl(ptr.addr)+4);
    return {tmp,mask};
}