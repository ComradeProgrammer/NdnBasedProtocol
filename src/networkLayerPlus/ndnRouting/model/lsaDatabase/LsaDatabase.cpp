#include "LsaDatabase.h"
using namespace std;
using json = nlohmann::json;

shared_ptr<LsaDataPack> LsaDatabase::findLsa(LinkStateType lsaType, uint32_t routerID) {
    switch (lsaType) {
        case LinkStateType::ADJ: {
            for (auto lsa : adjLsa) {
                if (routerID == lsa->routerID) {
                    return lsa;
                }
            }
            return nullptr;
        } break;
        case LinkStateType::RCH: {
            for (auto lsa : rchLsa) {
                if (routerID == lsa->routerID) {
                    return lsa;
                }
            }
            return nullptr;
        } break;
    }
    return nullptr;
}

void LsaDatabase::insertLsa(shared_ptr<LsaDataPack> lsa) {
    switch (lsa->lsType) {
        case LinkStateType::ADJ: {
            for (auto itr = adjLsa.begin(); itr != adjLsa.end(); itr++) {
                if ((*itr)->routerID == lsa->routerID) {
                    adjLsa.erase(itr);
                    break;
                }
            }
            adjLsa.push_back(lsa);
            break;
        }
        case LinkStateType::RCH: {
            for (auto itr = rchLsa.begin(); itr != rchLsa.end(); itr++) {
                if ((*itr)->routerID == lsa->routerID) {
                    rchLsa.erase(itr);
                    break;
                }
            }
            rchLsa.push_back(lsa);
        } break;
    }
    LOGGER->INFOF(2, "LsaDataBase::insertLsa current database %s", toString().c_str());
}

unique_ptr<unsigned char[]> LsaDatabase::hash() {
    vector<LinkStateDigest> digests;
    for (auto lsa : adjLsa) {
        digests.push_back(lsa->generateLSDigest());
    }
    for (auto lsa : rchLsa) {
        digests.push_back(lsa->generateLSDigest());
    }
    sort(digests.begin(), digests.end());

    Md5Hasher hasher;
    for (int i = 0; i < digests.size(); i++) {
        hasher.input(&(digests[i]), sizeof(LinkStateDigest));
    }

    return hasher.getResult();
}

json LsaDatabase::marshal() const {
    json j;
    vector<json> adjlsastr;
    for (auto lsa : adjLsa) {
        adjlsastr.push_back(lsa->marshal());
    }
    vector<json> rchlsastr;
    for (auto lsa : rchLsa) {
        rchlsastr.push_back(lsa->marshal());
    }
    j["adjLsa"] = adjlsastr;
    j["rchLsa"] = rchlsastr;
    return j;
}