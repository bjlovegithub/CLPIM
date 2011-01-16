/**
 * Cache Manager implementation.
 * by billjeff, Dec/24/2010
 *
 * Revision History:
 *     Dec/24/2010: Init
 *
 */

#include "TreeNodeCacheManager.h"
#include "Log.h"

using namespace XiaoDB;
using namespace CLPIM;
using namespace std;

bool TreeNodeCacheManager::CheckNodeExistance(PointerType id)
{
    BTreeNode *ptr;
    return mCacher.CheckKey(id, ptr);
}

bool TreeNodeCacheManager::AddNode(BTreeNode *nodePtr, PointerType id)
{
    bool ret = mCacher.Insert(id, nodePtr);
    if (!ret)
        LOG_ERROR("Insert new key error into cacher");
    else {
        RemoveNodeFromLUNCache(id);
        AddNodeIntoLUNCache(id, nodePtr);
    }

    return ret;
}


void TreeNodeCacheManager::RemoveNode(PointerType id)
{
    mCacher.RemoveKey(id);
    RemoveNodeFromLUNCache(id);
}

BTreeNode* TreeNodeCacheManager::GetNode(PointerType id)
{
    BTreeNode *ptr = NULL;
    bool ret = mCacher.CheckKey(id, ptr);
    if (!ret)
        return NULL;

    return ptr;
}

vector<PointerType> TreeNodeCacheManager::GetAllCachedNodes()
{
    vector<PointerType> allKeys;
    mCacher.GetAllKeys(allKeys);

    return allKeys;
}

/// TODO -UT
vector<PointerType> TreeNodeCacheManager::GetToFlushNodes()
{
    vector<PointerType> allKeys;
    vector<PointerType> retKeys;
    mCacher.GetAllKeys(allKeys);

    typeof(allKeys.begin()) iter;
    for (iter = allKeys.begin(); iter != allKeys.end(); ++iter) {
        bool find = false;
        typeof(mLUNCache.begin()) it = mLUNCache.begin();
        for (; it != mLUNCache.end(); ++it) {
            if (it->id == *iter) {
                find = true;
                break;
            }
        }
        if (!find)
            retKeys.push_back(*iter);
    }
    return retKeys;
}

/// TOOD -UT
void TreeNodeCacheManager::RemoveNodeFromLUNCache(PointerType id)
{
    typeof(mLUNCache.begin()) iter = mLUNCache.begin();
    bool f = false;
    for (; iter != mLUNCache.end(); ++iter)
        if (iter->id == id) {
            f = true;
            break;
        }

    if (f)
        mLUNCache.erase(iter);
}

/// TODO - UT
void TreeNodeCacheManager::AddNodeIntoLUNCache(PointerType id, BTreeNode *ptr)
{
    RemoveNodeFromLUNCache(id);
    NodeInfo n;
    n.id = id;
    n.ptr = ptr;
    mLUNCache.push_front(n);
    while (mLUNCache.size() > MaxLUNCacheSize) {
        mLUNCache.pop_back();
    }
}

