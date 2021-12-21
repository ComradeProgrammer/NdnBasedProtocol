#ifndef __LSADATABASE_H_
#define __LSADATABASE_H_
#include <unordered_set>
#include <vector>

#include "ip/RoutingTable.h"
#include "ndnRouting/dataPack/LsaDataPack.h"
#include "ndnRouting/protocol/lsaDatabase/graph/Graph.h"
#include "util/log/Logger.h"

class LsaDataBase : public Jsonfiable {
   public:
    LsaDataBase(std::shared_ptr<Logger> _logger = nullptr) { logger = Logger::getDefaultLoggerIfNull(_logger); }

    /**
     * @brief find a by routerid.
     *
     * @return std::shared_ptr<LsaDataPack>. nullptr if not found
     */
    std::shared_ptr<LsaDataPack> findLsa(LinkStateType lsaType, uint32_t routerID);
    /**
     * @brief insert a lsa by router id. existing lsa with the same router id and same type will be removed
     */
    void insertLsa(std::shared_ptr<LsaDataPack> lsa);

    void deleteLsa(std::shared_ptr<LsaDataPack> lsa);

    // get a const reference of adjLsa  lock NEED to be attained before called
    const std::vector<std::shared_ptr<LsaDataPack>>& getAdjLsa() { return adjLsa; }
    // get a const reference of rchLsa lock NEED to be attained before called
    const std::vector<std::shared_ptr<LsaDataPack>>& getRchLsa() { return rchLsa; }

    Graph generateGraph();

    void rebuildRoutingTable();

    virtual nlohmann::json marshal() const override;

    void ageDataBase();

   private:
    std::vector<std::shared_ptr<LsaDataPack>> adjLsa;
    std::vector<std::shared_ptr<LsaDataPack>> rchLsa;
    std::shared_ptr<Logger> logger;
    std::unordered_map<uint32_t, std::pair<uint32_t, int>> spfRes;

    void sendInfoInterestDueToAge(std::shared_ptr<LsaDataPack> lsa);
};
#endif