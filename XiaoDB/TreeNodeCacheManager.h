/**
 * Cache Manager for BPlusTree node.
 * by billjeff, Nov/19/2010
 * 
 * Revision History:
 *     Nov/19/2010: Init
 *     Jan/14/2010: Add codes to keep latest used nodes.
 */

#ifndef BILLJEFF_TREE_NODE_CACHE_MANAGER_H
#define BILLJEFF_TREE_NODE_CACHE_MANAGER_H

#include "BTreeCommon.h"

#include "HashTable.hpp"

#include <list>
#include <vector>
#include <string>

namespace XiaoDB
{

class TreeNodeCacheManager
{
    struct NodeInfo
    {
        PointerType id;
        BTreeNode *ptr;
    };

#ifdef _DEBUG
    static const uint32 MaxLUNCacheSize = 5;
#else
    static const uint32 MaxLUNCacheSize = 500;
#endif
    
public:
    /**
     * Check node is in cache or not.
     * @param id Node id. id*block_size indicates the offset in
     *           db file. And this id is kept in mPointer of parents
     *           node
     * @return Return true if node in the cache, otherwise false.
     */
    bool CheckNodeExistance(PointerType id);

    /**
     * Add a node into cache.
     * @param nodePtr Pointer to node.
     * @param id Node id. See the comment for @CheckNodeExistance.
     * @return Return true if add node successfully, otherwise false.
     */
    bool AddNode(BTreeNode *nodePtr, PointerType id);

    /**
     * Remove a node from cache.
     * @param id Node id.
     */
    void RemoveNode(PointerType id);

    /**
     * Get a node from cache
     * @param id Node id.
     * @return Return a pointer to node if node existed, otherwise
     *         return NULL
     */
    BTreeNode* GetNode(PointerType id);

    /**
     * Get all cached nodes' offset
     * @return Return a vector of cached nodes offset.
     */
    std::vector<PointerType> GetAllCachedNodes();

    /**
     * Get to be flushed node from cache. To flush node is old node
     * which is not in TreeNodeCacheManager's inner latest used node
     * cache.
     * @return Return a vector of pointer to to-flush node
     *         if existed, otherwise return NULL.
     */
    std::vector<PointerType> GetToFlushNodes();

    /**
     * Clear all keys in cache
     */
    void Clear();

    /**
     * Dump the content of cache
     */
    std::string Dump();

protected:
    /**
     * Remove a node from mLUNCache.
     */
    void RemoveNodeFromLUNCache(PointerType id);

    /**
     * Add a node into mLUNCache
     */
    void AddNodeIntoLUNCache(PointerType id, BTreeNode *ptr);
    
private:
    CLPIM::SimpleHashTable<PointerType, BTreeNode*> mCacher;

    /// keep leatest used node info, latest used node is always
    /// at beginning of list.
    std::list<NodeInfo> mLUNCache;
};

} // namespace XiaoDB

#endif // BILLJEFF_TREE_NODE_CACHE_MANAGER_H
