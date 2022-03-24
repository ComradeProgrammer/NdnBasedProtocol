#ifndef __LSA_DATABASE_H_
#define __LSA_DATABASE_H_
#include <memory>
#include <string>

#include "Graph.h"
#include "ioc.h"
#include "networkLayerPlus/ndnRouting/dataPack/LsaDataPack.h"
#include "util/hash/Md5Hasher.h"
class LsaDatabase : public Jsonfiable {
   public:
    LsaDatabase() = default;

    /**
     * @brief find a by routerid.
     *
     * @return std::shared_ptr<LsaDataPack>. nullptr if not found
     */
    std::shared_ptr<LsaDataPack> findLsa(LinkStateType lsaType, RouterID routerID);
    /**
     * @brief insert a lsa by router id. existing lsa with the same router id and same type will be removed
     */
    void insertLsa(std::shared_ptr<LsaDataPack> lsa);

    void deleteLsa(std::shared_ptr<LsaDataPack> lsa);

    // get a const reference of adjLsa  lock NEED to be attained before called
    const std::vector<std::shared_ptr<LsaDataPack>>& getAdjLsa() { return adjLsa; }
    // get a const reference of rchLsa lock NEED to be attained before called
    const std::vector<std::shared_ptr<LsaDataPack>>& getRchLsa() { return rchLsa; }

    /**
     * @brief calculateShortestPath and insert them into routing table;
     *
     * @param source
     */
    void calculateRoutingTable(RouterID source);
    /**
     * @brief calculate all MinHopTree,return the parent node of current router on each tree.
     *
     * @param source
     * @return std::unordered_map<RouterID, RouterID> root->parentnode
     *
     */
    std::unordered_map<RouterID, RouterID> calculateMinHopTree(RouterID source);

    /**
     * @brief use all lsa's name to calculate the md5 hash
     * @return unsigned char array and the length
     */
    std::pair<std::unique_ptr<unsigned char[]>,int>databaseHash();


    virtual nlohmann::json marshal() const override;

    std::string printContent();

   private:
    std::vector<std::shared_ptr<LsaDataPack>> adjLsa;
    std::vector<std::shared_ptr<LsaDataPack>> rchLsa;
};
#endif