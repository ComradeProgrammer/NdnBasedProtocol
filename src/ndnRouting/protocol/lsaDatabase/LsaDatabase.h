#ifndef __LSADATABASE_H_
#define __LSADATABASE_H_
#include<vector>
#include"util/log/Logger.h"
#include"ndnRouting/dataPack/LsaDataPack.h"

class LsaDataBase: public Jsonfiable{
    public:
    LsaDataBase(std::shared_ptr<Logger>_logger=nullptr){
        logger=Logger::getDefaultLoggerIfNull(_logger);
    }

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
    //get a const reference of adjLsa  lock NEED to be attained before called
    const std::vector<std::shared_ptr<LsaDataPack>>& getAdjLsa(){return adjLsa;}
    //get a const reference of rchLsa lock NEED to be attained before called
    const std::vector<std::shared_ptr<LsaDataPack>>& getRchLsa(){return rchLsa;}

    void rebuildRoutingTable();

    virtual nlohmann::json marshal() override;

    private:
    std::vector<std::shared_ptr<LsaDataPack>>adjLsa;
    std::vector<std::shared_ptr<LsaDataPack>>rchLsa;
    std::shared_ptr<Logger>logger;
};
#endif