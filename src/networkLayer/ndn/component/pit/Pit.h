#ifndef __PIT_H_
#define __PIT_H_
#include <unordered_map>

#include "PitEntry.h"
// NOT THREAD SAFE CLASS PIT
class Pit {
   public:
    Pit() = default;
    virtual ~Pit() = default;
    /**
     * @brief get the entry for the given name; a new item will be created if no
     * item for the given name is found
     * @param name packet name
     * @return shared_ptr of corresponding entry.
     */
    std::shared_ptr<PitEntry> getPitEntry(std::string name);
    /**
     * @brief find the Pitentry item by the name.
     * @param name name of the packet
     * @return shared_ptr of corresponding entry. return nullptr if no item is
     * found
     */
    std::shared_ptr<PitEntry> findPitEntry(std::string name);
    /**
     *@brief delete a pit entry
     */
    void deletePitEntry(std::string name);

   private:
    std::unordered_map<std::string, std::shared_ptr<PitEntry>> entries;
    // std::mutex lock;
};

#endif