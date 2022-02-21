#ifndef __LSA_DATABASE_H_
#define __LSA_DATABASE_H_
#include <memory>
#include <string>

#include "ioc.h"
#include "networkLayerPlus/ndnRouting/dataPack/LsaDataPack.h"
class LsaDatabase : public Jsonfiable{
   public:
    LsaDatabase()=default;

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

    virtual nlohmann::json marshal() const override;

   private:
    std::vector<std::shared_ptr<LsaDataPack>> adjLsa;
    std::vector<std::shared_ptr<LsaDataPack>> rchLsa;
};
#endif