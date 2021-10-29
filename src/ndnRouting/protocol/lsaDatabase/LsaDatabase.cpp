#include"LsaDatabase.h"
using namespace std;
shared_ptr<LsaDataPack> LsaDataBase::findLsa(LinkStateType lsaType, uint32_t routerID){
    switch(lsaType){
        case LinkStateType::ADJ:{
            for(auto lsa:adjLsa){
                if(routerID==lsa->routerID){
                    return lsa;
                }
            }
            return nullptr;
        }
        break;
        case LinkStateType::RCH:{
            for(auto lsa:rchLsa){
                if(routerID==lsa->routerID){
                    return lsa;
                }
            }
            return nullptr;
        }
        break;
    }
    return nullptr;
}

void LsaDataBase::insertLsa(shared_ptr<LsaDataPack> lsa){
    switch(lsa->lsType){
        case LinkStateType::ADJ:{
            for(auto itr=adjLsa.begin();itr!=adjLsa.end();itr++){
                if((*itr)->routerID==lsa->routerID){
                    adjLsa.erase(itr);
                    break;
                }
            }
            adjLsa.push_back(lsa);
        break;
        }
        case LinkStateType::RCH:{
            for(auto itr=rchLsa.begin();itr!=rchLsa.end();itr++){
                if((*itr)->routerID==lsa->routerID){
                    rchLsa.erase(itr);
                    break;
                }
            }
            rchLsa.push_back(lsa);
        }
    }
}