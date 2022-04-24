#ifndef __MINIMUM_HOP_TREE_H_
#define __MINIMUM_HOP_TREE_H_
#include <unordered_map>
#include <vector>

#include "networkLayerPlus/ndnRouting/dataPack/PacketCommon.h"

class MinimumHopTree {
   public:
    std::vector<RouterID> getRegisteredSons(RouterID root);
    void addToRegisteredSon(RouterID root, RouterID son);
    void deleteFromRegisteredSon(RouterID root, RouterID son);
    long getLastRegistrationTime(RouterID root, RouterID son);
    void setLastRegistrationTime(RouterID root, RouterID son, long timestamp);

    std::pair<bool, RouterID> getRegisteredParent(RouterID root);
    std::unordered_map<RouterID, RouterID> getRegisteredParents() { return registeredParents; }
    void removeRegisteredParent(RouterID root);
    void setRegisteredParent(RouterID root, RouterID parent);

   private:
    // root->sons () root->sons 向本路由器登记为以root为根的儿子的节点
    std::unordered_map<RouterID, std::vector<RouterID>> registeredSons;

    // timestamp we have seen(when son come to register/deregister)
    //[root][son]->time 对以root为根的树，son以儿子身份向本路由器最后一次注册/解除注册的时间
    std::unordered_map<RouterID, std::unordered_map<RouterID, long>> lastOperationTime;

    // root->parents 我们在以root为根的树上向谁注册为儿子了
    std::unordered_map<RouterID, RouterID> registeredParents;
};
#endif