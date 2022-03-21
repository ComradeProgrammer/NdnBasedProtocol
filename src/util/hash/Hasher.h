#ifndef __HASHER_H_
#define __HASHER_H_
#include <memory>
class Hasher{
    public:
    /**
     * @brief input some data into buffer so that they can be hashed later;
     */
    virtual void input(const void* data, unsigned int length)=0;
    /**
     * @brief return hash of data stored in buffer. Once this function is called, this hasher should not be reused again.
     */
    virtual std::unique_ptr<unsigned char[]> getResult()=0;

    /**
     * @brief instantly hash some data
     */
    virtual std::unique_ptr<unsigned char[]> hash(const void* data, unsigned int length)=0;
};
#endif
