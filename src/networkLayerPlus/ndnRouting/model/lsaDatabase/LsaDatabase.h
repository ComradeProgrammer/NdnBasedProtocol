#ifndef __LSA_DATABASE_H_
#define __LSA_DATABASE_H_
#include <memory>
#include <string>

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

    /**
     * @brief return the hash of digests of the lsa database
     *
     * @return std::unique_ptr<unsigned char[]> hash results, which have 16 bytes;
     */
    std::unique_ptr<unsigned char[]> hash();

   private:
    std::vector<std::shared_ptr<LsaDataPack>> adjLsa;
    std::vector<std::shared_ptr<LsaDataPack>> rchLsa;
};
#endif