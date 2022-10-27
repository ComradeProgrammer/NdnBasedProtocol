#include "DumbAddressPool.h"
#include<iostream>
#include"ioc.h"
using namespace std;
DumbAddressPool::DumbAddressPool() { ptr.setIp("192.168.0.0"); }
DumbAddressPool::DumbAddressPool(Ipv4Address start){
    ptr=start;
    startAddr=start;
    mask=Ipv4Address("255.255.0.0");
}

void DumbAddressPool::addAddressBlock(Ipv4Address _address, Ipv4Address _mask) {}

std::pair<Ipv4Address, Ipv4Address> DumbAddressPool::allocateAddress(int size) {
    Ipv4Address tmp = ptr;
    Ipv4Address mask;
    mask.setIp("255.255.255.252");
    ptr.addr = htonl(ntohl(ptr.addr)+4);
    // if(!((ptr.andMask(mask))==(startAddr.andMask(mask)))){
    //     return { Ipv4Address("0.0.0.0"), Ipv4Address("0.0.0.0")};
    // }
    //for test use
    //return {Ipv4Address("10.1.0.0"),mask};
    return {tmp,mask};

}