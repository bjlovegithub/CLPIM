/**
 * Hash table
 * by billjeff, Nov/20/2010
 *
 * Revision History:
 *     Nov/20/2010 - Init
 */

#ifndef CLPIM_HASH_TABLE_H
#define CLPIM_HASH_TABLE_H

#include <new>
#include <cmath>
#include <stdint.h>
#include <vector>

#include "Log.h"
#include "CommonInclude.h"

namespace CLPIM
{

/*** For UnitTest ***/
bool NormalizeSizeTest(void);
bool GetHashKeyTest(void);
bool InsertTest(void);
bool CheckKeyTest(void);    

/**
 * SimpleHashTable uses integer division hash function.
 * It supports unsigned integer as key, and set a value
 * for the related value.
 * If inserted keys take up 75%[TODO - Do some experiment]
 * hash table size, then expan table size to the power of 2.
 */
template <class KeyType = int32_t, class ValueType = int32_t>
class SimpleHashTable
{
    /*** For UnitTest ***/
    friend bool NormalizeSizeTest(void);
    friend bool GetHashKeyTest(void);
    friend bool InsertTest(void);
    friend bool CheckKeyTest(void);    

public:
    /**
     * Constructor
     */
    SimpleHashTable(unsigned int size = 1024)
    {
        mTableSize = NormalizeSize(size);
        GetHashKey();
        mTablePtr  = new (std::nothrow) Node*[mTableSize];
        for (size_t i = 0; i < mTableSize; ++i)
            mTablePtr[i] = NULL;
        mItemNum   = 0;
    }

    /**
     * Destructor
     */
    ~SimpleHashTable()
    {
        for (size_t i = 0; i < mTableSize; ++i) {
            Node *ptr = mTablePtr[i];
            while (ptr) {
                Node *tmpPtr = ptr->nextPtr;
                delete ptr;
                ptr = tmpPtr;
            }
        }
        delete[] mTablePtr;
    }

    /**
     * Insert a key and its value into hash table. If one key is
     * already in the hash table, its value will be updated.
     * @param key Key
     * @param value Value
     * @return Return true if insert key successfully,
     *         otherwise return false.
     */
    bool Insert(KeyType key, ValueType value)
    {
        /// check hash table status
        if ((float)mItemNum/mTableSize > 0.75)
            if (!ExpandHashTable())
                return false;

        uint32 hashKey = Hash(key);
        Node *ptr = mTablePtr[hashKey];
        while(ptr) {
            if (ptr->key == key) {
                ptr->value = value;
                return true;
            }
            ptr = ptr->nextPtr;
        }

        /// no key in the table, insert it into hash table
        Node *newNodePtr = new (std::nothrow) Node;
        if (NULL == newNodePtr) {
            LOG_ERROR("Allocate new memory error: " << sizeof(Node));
            return false;
        }
        newNodePtr->key = key;
        newNodePtr->value = value; 
        newNodePtr->nextPtr = mTablePtr[hashKey];
        mTablePtr[hashKey] = newNodePtr;

        ++mItemNum;

        return true;
    }

    /**
     * Check a key is in the hash table or not. If key exists, put
     * value in the second parameter.
     * @param key Key
     * @param value Reference to value.
     * @return Return true if key is in the table, otherwise false.
     */
    bool CheckKey(KeyType key, ValueType &value)
    {
        uint32 hashKey = Hash(key);
        Node *ptr = mTablePtr[hashKey];
        bool flag = false;
        while (ptr) {
            if (ptr->key == key) {
                value = ptr->value;
                flag = true;
                break;
            }
            ptr = ptr->nextPtr;
        }

        return flag;
    }

    /**
     * Remove a key from hash table.
     * @param key Key
     */
    void RemoveKey(KeyType key)
    {
        uint32 hashKey = Hash(key);
        Node *ptr = mTablePtr[hashKey];
        Node *prevPtr = ptr;
        while (ptr) {
            if (ptr->key == key) {
                if (ptr == prevPtr)
                    mTablePtr[hashKey] = NULL;
                else
                    prevPtr->nextPtr = ptr->nextPtr;
                delete ptr;
                break;
            }
            prevPtr = ptr;
            ptr = ptr->nextPtr;
        }
    }

    /**
     * Get all keys from hash table
     */
    void GetAllKeys(std::vector<KeyType> &ret)
    {
        ret.clear();
        ret.reserve(mItemNum);
        LOG_DEBUG("ItemNum: " << mItemNum << ", TableSize: " << mTableSize);
        for (uint32 i = 0; i < mTableSize; ++i) {
            Node *nodePtr = mTablePtr[i];
            while (nodePtr) {
                ret.push_back(nodePtr->key);
                nodePtr = nodePtr->nextPtr;
            }
        }
    }
    
private:
    /**
     * Inner structure to keep key and value.
     */
    struct Node
    {
        Node()
        {
            nextPtr = NULL;
        }
        KeyType key;
        ValueType value;

        Node *nextPtr;
    };

    /**
     * Normalize input parameter to the nearest unsinged int which is larger than
     * input parameter and it is power of 2.
     * @param input Input parameter
     * @return Nearest unsigned int which is greator than parameter and power of 2
     */
    uint32 NormalizeSize(uint64 input)
    {
        int bitCounter = 0;
        uint32 tmp = input;
        while (tmp)
        {
            tmp >>= 1;
            ++bitCounter;
        }
        uint32 ret = (bitCounter > 0) ? (1 << (bitCounter-1)) : 1;
        if (input > ret)
            ret <<= 1;
        return ret;
    }

    /**
     * Get hash key. The key is the max prime number which is less than
     * mTableSize.
     */
    void GetHashKey()
    {
        uint32 start = mTableSize-1;
        while (!IsPrime(start) && (start >= 2))
            --start;
        mHashKey = start;
    }

    /**
     * Test one number is a prime or not.
     */
    bool IsPrime(uint32 num)
    {
        uint32 end = uint32(sqrt(num));
        for (uint32 i = 2; i <= end; ++i)
            if (num % i == 0)
                return false;
        return true;
    }

    /**
     * hash function.
     */
    uint32 Hash(KeyType key)
    {
        return key%mHashKey;
    }

    /**
     * Expand hash table to fit more keys.
     */
    bool ExpandHashTable()
    {
        uint32_t newTableSize = (mTableSize << 1);
        if (newTableSize >= 10000000) {
            LOG_ERROR("Hash Table size is exceeding the maximum size: " << newTableSize);
            return false;
        }

        {
            uint32_t oldTableSize = mTableSize;
            mTableSize = newTableSize;
            GetHashKey();
            mTableSize = oldTableSize;
        }
        
        Node **newTablePtr = new (std::nothrow) Node*[newTableSize];
        if (NULL == newTablePtr) {
            LOG_ERROR("Memory allocataion error, size: " << mTableSize);
            return false;
        }
        for (uint32_t i = 0; i < newTableSize; ++i)
            newTablePtr[i] = NULL;

        /// re-hash old element in hash table. it is time-consuming task.
        Node **oldTablePtr = mTablePtr;
        mTablePtr = newTablePtr;
        mItemNum = 0;
        for (uint32_t i = 0; i < mTableSize; ++i) {
            Node *ptr = oldTablePtr[i];
            while (ptr) {
                if (!Insert(ptr->key, ptr->value)) {
                    LOG_ERROR("Insert old element error when expanding");
                }
                ptr = ptr->nextPtr;
            }
        }

        /// free the old table
        for (uint32_t i = 0; i < mTableSize; ++i) {
            Node *ptr = oldTablePtr[i];
            Node *tmpPtr;            
            while (ptr) {
                tmpPtr = ptr->nextPtr;
                delete ptr;
                ptr = tmpPtr;
            }
        }
        delete[] oldTablePtr;

        mTableSize = newTableSize;
        mTablePtr = newTablePtr;

        return true;
    }
private:
    /// hash table.
    Node **mTablePtr;
    /// table size.
    uint32_t mTableSize;
    /// hash key
    uint32_t mHashKey;
    /// number of elements in hash table.
    uint32_t mItemNum;
};

} // namespce CLPIM

#endif // CLPIM_HASH_TABLE_H
