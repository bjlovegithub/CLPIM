#include "BTree.h"

#include "Log.h"
#include "utility.h"

#include <cstdlib>
#include <cstring>
#include <stack>
#include <map>

using namespace std;
using namespace XiaoDB;
using namespace CLPIM;

/// Pointer(size_t) takes up 8 bytes.
#define POINTER_LEN        8
/// Key lenth, element num byte length
#define NUMBER_LEN         4
/// Each B+ tree node takes up 8K bytes in B+ tree file.
#define BLOCK_SIZE         8192
/// Maximum key number in each node
#ifdef _DEBUG
#define MAX_ELEMENT_NUMBER 5
#else
#define MAX_ELEMENT_NUMBER 80
#endif
/// Maximum key length
#define MAX_KEY_LEN        80
/// Maximum height of B+ tree
#define BTREE_HEIGHT       9
/// Maximum key number in BPTree
#define MAX_KEY_NUM 100000000UL
/// When BitMap in BPlusTree is not enough, it will expand it.
/// And BITMAP_EXPAND_SIZE determines expansion size.
#define BITMAP_EXPAND_SIZE 30000
/// Max BTreeNode can be resiented in the cache
#define MAX_CACHED_NODE_NUM 10000

/// Delimiter between key and value in B+ node
#define KEY_VALUE_DELIMITER     0x01010101
#define KEY_VALUE_DELIMITER_STR "\01\01\01\01"
/// Delimiter between B+ nodes in B+ file
#define KEYS_DELIMITER          0x02020202
#define KEYS_DELIMITER_STR      "\02\02\02\02"
#define DELIMITER_SIZE          4

/*** Helper functions ***/

/**
 * Get BST keys and values in asending order.
 */
void DFSKeysAndValues(BSTNode *ptr, vector<Key> &keys,
                      vector<PointerType> &vals)
{
    if (NULL == ptr)
        return;
    if (ptr->mLeft != NULL) {
        DFSKeysAndValues(ptr->mLeft, keys, vals);
    }
    keys.push_back(Key(ptr->mValue, ptr->mValLen));
    vals.push_back(ptr->mPointer);
    if (ptr->mRight != NULL) {
        DFSKeysAndValues(ptr->mRight, keys, vals);
    }
}

/**
 * Get all BSTNode in BST
 */
void DFSGetBSTNodes(BSTNodePtr ptr, vector<BSTNodePtr> &ret)
{
    if (NULL == ptr)
        return;
    ret.push_back(ptr);
    DFSGetBSTNodes(ptr->mLeft);
    DFSGetBSTNodes(ptr->mRight);
}

/**
 * Destroy the whole Binary Search Tree(free memory).
 */
void DestroyBST(BSTNode *root)
{
    if (NULL == root)
        return;
    DestroyBST(root->mLeft);
    DestroyBST(root->mRight);
    delete root;
}

/*** struct Key ctor and dtor implementation ***/
Key::Key()
{
    mKey = NULL;
    mKeyLen = 0;
}

Key::Key(BSTNode *ptr)
{
    if (NULL == ptr) {
        mKey = NULL;
        mKeyLen = 0;
        return;
    }

    mKeyLen = ptr->mValLen;
    mKey = new (nothrow) uchar[ptr->mValLen];
    if (NULL == mKey) {
        mKey = NULL;
        mKeyLen = 0;
        LOG_ERROR("Memory alloation error");
        return;
    }
    memcpy(mKey, ptr->mValue, mKeyLen);
}

Key::Key(const uchar *key, uint8 len)
{
    mKey = new (nothrow) uchar[len];
    if (NULL == mKey) {
        mKey = NULL;
        mKeyLen = 0;
        LOG_ERROR("Memory alloation error");
        return;
    }
    memcpy(mKey, key, len);
    mKeyLen = len;
}

Key::Key(const Key &rhs)
{
    /// delete[] mKey;
    mKeyLen = rhs.mKeyLen;
    mKey = new (nothrow) uchar[mKeyLen];
    if (NULL == mKey) {
        LOG_ERROR(MemoryAllocError);
        return;
    }
    memcpy(mKey, rhs.mKey, mKeyLen);
}

Key::~Key()
{
    delete[] mKey;
}

string Key::ToString() const
{
    string s;
    for (size_t i = 0; i < mKeyLen; ++i)
        s += (char)mKey[i];
    return s;
}

bool Key::operator==(const Key &rhs)
{
    int8 cmpRet = BitComparer(this->mKey, this->mKeyLen,
                              rhs.mKey, rhs.mKeyLen);
    return (cmpRet == 0);
}

/*** Node related member function ***/
BSTNode::BSTNode()
{
    mLeft = mRight = NULL;
    mValue = NULL;
    mPointer = 0;
    mIsOk = false;
}

BSTNode::BSTNode(const uchar *key, uint32 keyLen, PointerType val)
{
    mValue = new (nothrow) uchar[keyLen];
    mIsOk = false;
    if (NULL == mValue) {
        LOG_ERROR("Memory Allocation Error in BSTNode Ctor");
    }
    mPointer = val;
    mValLen = keyLen;
    mLeft = mRight = NULL;
    memcpy(mValue, key, mValLen);
    mIsOk = true;
}

bool BSTNode::IsOk()
{
    return mIsOk;
}

string BSTNode::ToString()
{
    string s(mValLen, 'a');
    for (size_t i = 0; i < mValLen; ++i)
        s[i] = mValue[i];
    return s;
}

BSTNode::~BSTNode()
{
    delete[] mValue;
}

BTreeNode::BTreeNode()
{
    mRoot = NULL;
    mPointer = 0;
    mKeyNum = 0;
    mIsDirty = false;
}

BTreeNode::~BTreeNode()
{
    DestroyBST(mRoot);
}

string BTreeNode::Dump()
{
    if (NULL == mRoot)
        return "Empty BTreeNode";
    vector<Key> keys;
    vector<PointerType> ptrs;
    DFSKeysAndValues(mRoot, keys, ptrs);
    stringstream ss;
    ss << "All key num: " << mKeyNum << " ";
    ss << "Is leaf: " << mLeafFlag << " ";
    ss << "Offset id: " << mOffsetID << " ";
    for (size_t i = 0; i < keys.size(); ++i) {
        ss << ptrs[i] << " [" << keys[i].ToString() << "] ";
    }
    ss << mPointer;
    return ss.str();
}

/*** Public Interface definition for BPlusTree ***/
BPlusTree::BPlusTree()
{
    mInitFlag      = false;
    mKeyNum        = 0;
    mMaxKeyNum     = 0;    
    mBitMapPtr     = NULL;
    mFileName      = "";
    mLazy          = false;
    mChangedFlag   = false;
    mRootPtr       = NULL;
    mBitMapLen     = 0;
    mCachedNodeNum = 0;
    mInsertKeyDupFlag = false;
#ifdef BTREE_VERSION
    mVersion       = BTREE_VERSION;
#else
    mVersion       = "";
#endif
    LOG_DEBUG("BTree Version: " << mVersion);
}

BPlusTree::~BPlusTree()
{
    /// TODO
}

bool BPlusTree::Open(const string &fileName)
{
    if (mInitFlag)
    {
        LOG_INFO("BTree with file name: " << mFileName << " has been opened");
        LOG_INFO("Flush data to " << mFileName << " and close it now.");
        /// Commit changes.
        bool ret = Commit();
        if (!ret) {
            LOG_ERROR("Commit changes for " << mFileName << " failed");
            return false;
        }
        mFileName = fileName;
        mInitFlag = false;
    }
    
    /// Open file.
    bool needInitHead = false;
    if (!FileUtil::CheckFile(fileName)) {
        LOG_INFO("BPTree DB file not existed");
        needInitHead = true;
    }
    if (!mFileHandler.Open(fileName, O_RDWR|O_CREAT))
    {
        LOG_FATAL("Open B+ tree file error");
        return false;
    }
    if (needInitHead) {
        mMaxKeyNum = MAX_KEY_NUM;
        if (!WriteBTreeHead()) {
            return false;
        }
    }

    /// read init data from head part of db file and check data
    if (!needInitHead) {
        if (!CheckData())
        {
            LOG_FATAL("Read B+ tree file header error");
            return false;
        }
    }

    /// init bitmap
    mBitMapPtr = new (nothrow) bool[BITMAP_EXPAND_SIZE];
    if (NULL == mBitMapPtr) {
        LOG_FATAL("Allocate memory for BitMap error");
        return false;
    }
    mBitMapLen = BITMAP_EXPAND_SIZE;
    memset(mBitMapPtr, 0, mBitMapLen);
    /// the first block is used to keep meta data for BPlusTree
    mBitMapPtr[0] = true;

    /// If data is Ok, get the root node.
    mRootPtr = new (nothrow) BTreeNode;
    if (NULL == mRootPtr) {
        LOG_FATAL("Allocate memory error");
        return false;
    }
    if (!needInitHead) {
        if (!GetNode(mRootPtr, mRootOffset))
        {
            LOG_ERROR("Cannot initialize root");
            return false;
        }
    }
    else {
        mRootOffset = 1;
        mRootPtr->mOffsetID = mRootOffset;
        mRootPtr->mLeafFlag = true;
        UpdateBitMap(mRootOffset, true);
    }

    /// add to cache
    if (!mBTreeNodeCache.AddNode(mRootPtr, mRootOffset)) {
        LOG_ERROR(AddToCacheError);
        /// Remove allocated memory
        delete mRootPtr;
        return false;
    }
    else {
        ++mCachedNodeNum;
        if (mCachedNodeNum > MAX_CACHED_NODE_NUM) {
            bool ret = FlushCache();
            if (!ret) {
                LOG_ERROR("FlushCache error");
            }
        }
    }

    mInitFlag = true;
    
    return true;
}

bool BPlusTree::Insert(const uchar *key, uint32 keyLen, PointerType value)
{
    LOG_CALL();

    if (!mInitFlag) {
        LOG_ERROR("B+ Tree has not been initialized");
        return false;
    }

    /// clear minor rollback cache
    ClearMinorRollbackCache();

    LOG_INFO("To Insert Key: " << Key(key, keyLen).ToString());

    /// set insert dup flag to false.
    mInsertKeyDupFlag = false;

    /// create a new BSTNode
    BSTNode *insertNodePtr = new BSTNode(key, keyLen, value);

    LOG_DEBUG("To Insert Key(in BSTNode): " 
              << Key(insertNodePtr->mValue, insertNodePtr->mValLen).ToString());

    if (NULL == insertNodePtr) {
        LOG_ERROR("Memory allocation error");
        return false;
    }
    
    if (!insertNodePtr->IsOk())
        return false;

    /// check number of keys in BPlusTree    
    LOG_INFO("Num of keys in tree: " << mKeyNum << "\tMax Key Num: " << mMaxKeyNum <<
             "\tMax key num per node: " << MAX_ELEMENT_NUMBER);
    BTreeNode *rootPtr = mRootPtr;
    if (mKeyNum+1 > mMaxKeyNum) {
        LOG_ERROR("BPlus Tree is FULL!");
        return false;
    }
    
    pair<PointerType, PointerType> ptrInfo;
    bool hasNewKey = false;
    BSTNode *ptr;
    bool ret = RecursiveInsert(insertNodePtr, mRootPtr, hasNewKey, ptr, ptrInfo);
    if (!ret) {
        LOG_ERROR("RecursiveInsert Error!");
        /// Rollback
        if (!InsertRollback())
            LOG_ERROR("Insert rollback error!");
        return false;
    }
    /// check whether root node has splitted or not.
    if (true == hasNewKey) {
        LOG_INFO("New root");
        ptr->mLeft = ptr->mRight = NULL;
        //// allocate new root.
        mRootPtr = new (nothrow) BTreeNode;
        if (NULL == mRootPtr) {
            LOG_ERROR("Memory allocation error!");
            delete ptr;
            /// Rollback.
            if (!InsertRollback()) 
                LOG_ERROR("Insert rollback error!");
            return false;
        }
        /// assign offset id
        mRootPtr->mOffsetID = GetNewNodeSlot();
        if (0 == mRootPtr->mOffsetID) {
            delete mRootPtr;
            if (!InsertRollback())
                LOG_ERROR("Insert rollback error!");
            return false;
        }
        UpdateBitMap(mRootPtr->mOffsetID, true);
        /// insert new key into new root
        bool dupFlag = false;
        if (!BSTInsertNode(mRootPtr->mRoot, ptr, mRootPtr, dupFlag)) {
            LOG_ERROR("BSTInsertNode Error");
            delete mRootPtr;
            if (!InsertRollback())
                LOG_ERROR("Insert rollback error!");
            return false;
        }
        if (!dupFlag)
            mRootPtr->mKeyNum += 1;
        mRootPtr->mIsDirty = true;
        mRootPtr->mLeafFlag = false;
        mRootPtr->mRoot->mPointer = ptrInfo.first;
        mRootPtr->mPointer = ptrInfo.second;
        /// add to cache
        if (!mBTreeNodeCache.AddNode(mRootPtr, mRootPtr->mOffsetID)) {
            LOG_ERROR("Add node into BTreeNode Cache Error!");
            delete mRootPtr;
            if (!InsertRollback())
                LOG_ERROR("Insert rollback error!");
            return false;
        }
        mRootOffset = mRootPtr->mOffsetID;
        LOG_INFO("Dump new root: " << mRootPtr->Dump());
    }

    /// increment whole key number in BPTree.
    if (!mInsertKeyDupFlag)
        mKeyNum += 1;
    else
        mInsertKeyDupFlag = false;
    
    return true;
}

/*** Private member function definition for BPlusTree ***/

bool BPlusTree::CheckData()
{
    string header;
    if (!mFileHandler.Read(BLOCK_SIZE, header))
    {
        LOG_FATAL("Read header error");
        return false;
    }

    /// Get B+ tree data version.
    /// For B+ tree data header, its format is:
    /// VERSION[KEYS_DELIMITER]KEY_NUM(8 bytes)MAX_KEY_NUM(8 bytes)ROOT_OFFSET(8 bytes)
    size_t pos = header.find(KEYS_DELIMITER_STR);
    if (pos == string::npos)
    {
        LOG_ERROR("Cannot find delimiter in header for version");
        return false;
    }
    string dataVersion = header.substr(0, pos);
    LOG_DEBUG("Data Version: " << dataVersion);

    /// TODO - Get key num and max key num.
    pos += DELIMITER_SIZE;
    string keyNum = header.substr(pos, 8);
    mKeyNum = Binary2Uint64(keyNum);
    LOG_INFO("Key Num: " << mKeyNum);

    pos += 8;
    string maxKeyNum = header.substr(pos, 8);
    mMaxKeyNum = Binary2Uint64(maxKeyNum);
    LOG_INFO("Max Key Num: " << mMaxKeyNum);

    /// get root offset
    pos += 8;
    string rootOffset = header.substr(pos, 8);
    mRootOffset = Binary2Uint64(rootOffset);
    LOG_INFO("root offset: " << mRootOffset);
    
    /// Check data compatibility.
    bool ret = CheckCompatibility(mVersion, dataVersion);
    return ret;
}

bool BPlusTree::CheckCompatibility(const string &version,
                                   const string &dataVersion)
{
    /// Carry out checking logic
    if (version >= dataVersion)
        return true;
    return false;
}


/**
 * Note about node structure in B+ tree file.
 * Its structure looks like this(ignore brackets):
 * [Element Num][LEAF][{ValLen(4byte)}{Key}{Pointer}]...[{LastPointer}].
 * For {Pointer} field in node, it points to the child node if current
 * node is non-leaf, or offset for leaf.
 */
bool BPlusTree::GetNode(BTreeNode *nodePtr, uint64 n)
{
    LOG_CALL();
    LOG_DEBUG("BTreeNode offset: " << n);
    long long startPos = n*BLOCK_SIZE;
    /// TODO - Imporve performance, do not use string, allocate 8192 bytes
    /// previously instead by string's expanding(re-allocation).
    string nodeData;
    if (!mFileHandler.Read(startPos, BLOCK_SIZE, nodeData))
    {
        LOG_ERROR("Read data from " << mFileName << " error");
        return false;
    }
    /// Get element number in current node.
    size_t pos = 0;
    uint32_t elementNum =
        uint32_t(Binary2Uint64((nodeData.substr(0, NUMBER_LEN))));
    pos += NUMBER_LEN;
    if (elementNum > MAX_ELEMENT_NUMBER)
    {
        LOG_ERROR("Invalid element number: " << elementNum);
        return false;
    }
    LOG_DEBUG("Keys in BTreeNode: " << elementNum);
    
    /// Get is leaf or not leaf flag.
    uint64_t leafFlag;
    {
        if (pos >= nodeData.size())
        {
            LOG_ERROR("Cannot parse leaf flag");
            return false;
        }
        leafFlag = Binary2Uint64((nodeData.substr(pos, 4).c_str()));
        LOG_DEBUG("Leaf flag: " << leafFlag);
        nodePtr->mLeafFlag = (leafFlag != 0);
        pos += 4;
    }

    /// Parse element(key and pointer)
    {
        uint32_t i;
        size_t tmpPos;
        string bstNodeStr;
        for (i = 0; i < elementNum; ++i)
        {
            /// get key and pointer.
            BSTNode *bstNodePtr = new (nothrow) BSTNode;
            if (!bstNodePtr)
            {
                LOG_ERROR("Cannot allocate more memory!");
                DestroyBST(nodePtr->mRoot);
                return false;
            }
            /// get key
            uint32_t keyLen = 0;
            keyLen =
                uint32_t(Binary2Uint64(nodeData.substr(pos, NUMBER_LEN).c_str()));
            LOG_DEBUG("Key length: " << keyLen);
            if (keyLen > MAX_KEY_LEN)
            {
                LOG_ERROR("Key length exceeds MAX KEY LEN");
                DestroyBST(nodePtr->mRoot);
                return false;
            }
            bstNodePtr->mValLen = keyLen;
            pos += NUMBER_LEN;
            unsigned char *tmpPtr;
            tmpPtr = new (nothrow) unsigned char[keyLen];
            if (NULL == tmpPtr)
            {
                LOG_ERROR("Memory Allocation Error");
                return false;
            }
            memcpy(tmpPtr, nodeData.c_str()+pos, keyLen);
            bstNodePtr->mValue = tmpPtr;
            pos += keyLen;
            LOG_DEBUG("key: " << Key(tmpPtr, keyLen).ToString());
            /// get pointer
            bstNodePtr->mPointer =
                Binary2Uint64(nodeData.substr(pos, POINTER_LEN));
            
            pos += POINTER_LEN;
            /// insert this node into current B+ tree node.
            bool dupFlag = false;
            BSTInsertNode(nodePtr->mRoot, bstNodePtr, nodePtr, dupFlag);
            if (!dupFlag)
                nodePtr->mKeyNum += 1;
        }

        /// According to the structure of B+ tree node, the last key-value pair
        /// just keeps the last pointer for non-leaf node or the linked list
        /// pointer for leaf node.
        if (i != elementNum)
        {
            LOG_ERROR("Parse B+ node data error");
            DestroyBST(nodePtr->mRoot);
            return false;
        }
        nodePtr->mPointer =
            Binary2Uint64(nodeData.substr(pos, POINTER_LEN));
        pos += POINTER_LEN;
    } // end of parse elements

    nodePtr->mOffsetID = n;
    
    /// update mBitMap
    UpdateBitMap(n, true);

    LOG_DEBUG("GetNode, dump BTreeNode: " << nodePtr->Dump());

    return true;
}

/*** This function has deprecated ***/
bool BPlusTree::ParseKeyAndValue(BSTNode *bstNodePtr, const string &str)
{
    /// The first 4 bytes denote key length
    size_t keyLen = (size_t)atoi(str.substr(0, 4).c_str());
    if (keyLen > MAX_KEY_LEN)
    {
        LOG_ERROR("Parsed key length is too long: " << keyLen);
        return false;
    }
    bstNodePtr->mValLen = keyLen;

    size_t pos = str.find(KEY_VALUE_DELIMITER_STR, 4);
    if (pos == string::npos)
        return false;
    unsigned char *tmpPtr;
    tmpPtr = new (nothrow) unsigned char[pos-4];
    if (NULL == tmpPtr)
    {
        LOG_ERROR("Memory Allocation Error");
        return false;
    }
    memcpy(tmpPtr, str.c_str()+4, pos-4);
    bstNodePtr->mValue = tmpPtr;

    size_t pointerStartPos = pos+DELIMITER_SIZE;
    size_t pointerByteNum =  str.size()-pointerStartPos;
    memcpy((void*)&(bstNodePtr->mPointer),
           (str.c_str()+pointerStartPos), pointerByteNum);

    return true;
}

bool BPlusTree::BSTInsertNode(BSTNode* &root, BSTNode *bstNodePtr,
                              BTreeNode *btreeNodePtr, bool &dupFlag)
{
    LOG_DEBUG("Call BSTInsertNode");
    if (NULL == bstNodePtr) {
        LOG_INFO("To insert key is NULL");
        return false;
    }
    
    LOG_INFO("To Insert Key: " << 
             Key(bstNodePtr->mValue, bstNodePtr->mValLen).ToString());

    if (NULL == root)
    {
        root = bstNodePtr;
        return true;
    }

    BSTNode *ptr = root;
    while(true)
    {
        int compRet = BitComparer(ptr->mValue, ptr->mValLen,
                                  bstNodePtr->mValue, bstNodePtr->mValLen);
        if (0 == compRet)
        {
            LOG_DEBUG("Duppliated Keys: " << bstNodePtr->mValue);
            LOG_INFO("Update duplicated key node info");
            ptr->mPointer = bstNodePtr->mPointer;
            dupFlag = true;
            return true;
        }
        else if (1 == compRet)
        {
            /// current tree node is greater than inserting node.
            if (NULL != ptr->mLeft)
                ptr = ptr->mLeft;
            else
            {
                ptr->mLeft = bstNodePtr;
                break;
            }
        }
        else
        {
            /// to be inserted node is greater than current node
            if (NULL != ptr->mRight)
                ptr = ptr->mRight;
            else
            {
                ptr->mRight = bstNodePtr;
                break;
            }
        }
    } // while(true)

    return true;
}

int8 BPlusTree::BitComparer(void *first, size_t len1, void *second, size_t len2)
{
    size_t minLen = min(len1, len2);
    int re = memcmp(first, second, minLen);
    if (0 == re)
    {
        if (len1 == len2)
            return 0;
        else if (len1 > len2)
            return 1;
        else
            return -1;
    }
    return re;
}

/// TODO - UT
bool BPlusTree::RecursiveInsert(BSTNode *toInsertNode, BTreeNode *currNodePtr,
                                bool &hasNewKey, BSTNode* &newKey,
                                pair<PointerType, PointerType> &ptrInfo)
{
    LOG_DEBUG("To Insert Key in RecursiveInsert: " <<
              Key(toInsertNode->mValue, toInsertNode->mValLen).ToString());
    LOG_DEBUG("Dump curr BPTreeNode: " << currNodePtr->Dump());

    /// find the leaf node, insert it, and split if necessary
    if (currNodePtr->mLeafFlag) {
        LOG_DEBUG("Insert key in leaf");
        if (currNodePtr->mKeyNum < MAX_ELEMENT_NUMBER) {
            /// copy and put the old node into 
            if (!ModifiedBTreeNodeBackup(currNodePtr))
                return false;
            bool dupFlag = false;
            if (!BSTInsertNode(currNodePtr->mRoot, toInsertNode,
                               currNodePtr, dupFlag))
            {
                LOG_ERROR("Key insert error");
                return false;
            }
            /// set dirty flag
            currNodePtr->mIsDirty = true;
            if (!dupFlag)
                currNodePtr->mKeyNum += 1;
            hasNewKey = false;

            LOG_DEBUG("\nAfter Insert(leaf):\n" << currNodePtr->Dump());
        }
        else {
            /// split leaf and get the middle key that separate the two
            /// new leaf node.
            newKey = SplitBTreeNode(currNodePtr, toInsertNode, true, ptrInfo);
            LOG_DEBUG("SplitBTreeNode ret: " << int(newKey));
            if (NULL == newKey) {
                LOG_ERROR("Split Leaf Node Error");
                return false;
            }
            hasNewKey = true;
        }
        
        return true;
    } /// leaf node
    /// visit all keys in non-leaf node, locate the right sub-tree
    LOG_DEBUG("Find right sub-tree");
    BSTNode *ptr = currNodePtr->mRoot;
    PointerType nextNodeOffset = 0;
    /// TODO - We can add a pointer to point to max key in current node,
    /// if to-insert key is larger than max key, then mPointer is the value
    /// for nextNodeOffset.
    /// The following code is used to get the right BSTNode whose key is larger
    /// than to-insert key but it is the minimum key in all keys which are
    /// larger than to-insert key.
    bool isMaxKey = true;
    BSTNode *leastMaxKeyPtr = NULL;
    while (ptr) {
        int compRet = BitComparer(ptr->mValue, ptr->mValLen,
                                  toInsertNode->mValue, toInsertNode->mValLen);
        LOG_DEBUG("\nKey in current node: " <<
                  Key(ptr->mValue, ptr->mValLen).ToString() <<
                  "\nTo insert key: " <<
                  Key(toInsertNode->mValue, toInsertNode->mValLen).ToString() <<
                  "\nBitCmp Ret: " << compRet);
                                                   
        if (compRet <= 0) {
            /// the offset to the next non-leaf is the right key which is
            /// the least key that are greator than current key.
            /*
            if (ptr->mRight != NULL)
                ptr = ptr->mRight;
            else {
                nextNodeOffset = currNodePtr->mPointer;
                find = true;
            }
            */
            ptr = ptr->mRight;
        }
        else if (1 == compRet) {
            /*
            isMaxKey = false;
            if (NULL == ptr->mLeft) {
                nextNodeOffset = ptr->mPointer;
                find = true;
            }
            else {
                int t = BitComparer(ptr->mLeft->mValue, ptr->mLeft->mValLen,
                                    toInsertNode->mValue, toInsertNode->mValLen);
                if (t > 1)
                    ptr = ptr->mLeft;
            }
            */
            /// get the least max key
            isMaxKey = false;            
            if (NULL == leastMaxKeyPtr)
                leastMaxKeyPtr = ptr;
            else {
                int8 t = BitComparer(ptr->mValue, ptr->mValLen,
                                     leastMaxKeyPtr->mValue,
                                     leastMaxKeyPtr->mValLen);
                if (t < 0)
                    leastMaxKeyPtr = ptr;
            }
            ptr = ptr->mLeft;
        }
        else {
            /*
            if (NULL == ptr->mRight) {
                nextNodeOffset = ptr->mPointer;
                find = true;
            }
            else
                ptr = ptr->mRight;
            */
        }
    }
    if (isMaxKey)
        nextNodeOffset = currNodePtr->mPointer;
    else
        nextNodeOffset = leastMaxKeyPtr->mPointer;

    LOG_DEBUG("Next sub-tree offset: " << nextNodeOffset);
    /// Check next node is in the cache or not.
    BTreeNode *nextNodePtr = NULL;
    if (mBTreeNodeCache.CheckNodeExistance(nextNodeOffset)) {
        /// in the cache
        nextNodePtr = mBTreeNodeCache.GetNode(nextNodeOffset);
    }
    else {
        /// get this node from BPlusTree file.
        nextNodePtr = new (nothrow) BTreeNode;
        if (NULL == nextNodePtr) {
            LOG_FATAL("Allocate BTreeNode memory error!");
            return false;
        }
        if (!GetNode(nextNodePtr, nextNodeOffset)) {
            LOG_ERROR("Read BTreeNode from BPlusTree file error!" <<
                      "Offset: " << nextNodeOffset);
            delete nextNodePtr;
            return false;
        }
        /// add new node to cache
        if (!mBTreeNodeCache.AddNode(nextNodePtr, nextNodeOffset)) {
            LOG_ERROR(AddToCacheError);
            /// TODO - Remove allocated memory
        }
        else {
            ++mCachedNodeNum;
            if (mCachedNodeNum > MAX_CACHED_NODE_NUM) {
                /// Reduce cached node num, write dirty node back to file
                FlushCache();
            }
        }
    }

    bool ret;
    ret = RecursiveInsert(toInsertNode, nextNodePtr, hasNewKey, newKey, ptrInfo);
    if (!ret) {
        LOG_ERROR("RecursiveInsert return false!");
        return false;
    }

    /// add the new key(from spiltted leaf node) into current non-leaf node,
    /// maybe splition for this node is needed.
    if (hasNewKey) {
        LOG_DEBUG("RecursiveInsert, non-leaf, has new key, dump current node: " <<
                  currNodePtr->Dump());
        /// Insert new key into current node, split it if necessary.
        hasNewKey = false;
        if (currNodePtr->mKeyNum < MAX_ELEMENT_NUMBER) {
            LOG_DEBUG("Insert new key into non-leaf node");
            /// make a backup for rollback
            if (!ModifiedBTreeNodeBackup(currNodePtr))
                return false;
            bool mpFlag = false;
            if (!BTreeNodeInsertSplittedKey(currNodePtr, newKey,
                                            ptrInfo, mpFlag))
            {
                LOG_ERROR("Insert splitted key from leaf into current node error");
                return false;
            }
            currNodePtr->mKeyNum += 1;
            currNodePtr->mIsDirty = true;

            LOG_DEBUG("\nAfter Insert(non-leaf):\n" << currNodePtr->Dump());

        }
        else {
            /// Split current node.
            LOG_DEBUG("Insert new key into non-leaf node with splition");
            newKey = SplitBTreeNode(currNodePtr, newKey, false, ptrInfo);
            if (NULL == newKey) {
                LOG_ERROR("Split Non leaf node error");
                return false;
            }
            hasNewKey = true;
        }
    }
    return true;
}

/// TODO - UT for non-leaf node.
BSTNode* BPlusTree::SplitBTreeNode(BTreeNode *node, BSTNode *bstNodePtr,
                                   bool isLeaf,
                                   pair<PointerType, PointerType> &ptrInfo)
{
    LOG_DEBUG("Call SplitBTreeNode. Is leaf: " << isLeaf);

    if (NULL == node)
        return NULL;

    if (NULL == bstNodePtr)
        return NULL;

    /// make a copy of to-be splitted nodes
    if (!ModifiedBTreeNodeBackup(node))
        return false;
    
    BSTNode *retPtr = NULL;

    bool cacheInsertErr, copyBSTNodeErr;
    cacheInsertErr = copyBSTNodeErr = false;

    vector<BSTNode*> keyVec;
    GetBSTKeys(node->mRoot, keyVec);
    /// invalidate all to child pointers
    typeof(keyVec.begin()) iter;
    for (iter = keyVec.begin(); iter != keyVec.end(); ++iter)
        (*iter)->mLeft = (*iter)->mRight = NULL;

    /// use the middle key as a splitter
    size_t splitterPos = (keyVec.size()/2);
    LOG_DEBUG("Splitter Position: " << splitterPos);

    /// invalidate current node's keys.
    node->mRoot = NULL;
    node->mKeyNum = 0;
    /// TODO - Maybe randomize the keys first is better(keep the BST balanced)
    LOG_DEBUG("Insert splitted keys into left node");
    for (size_t i = 0; i < splitterPos; ++i) {
        bool dupFlag = false;
        BSTInsertNode(node->mRoot, keyVec[i], node, dupFlag);
        if (!dupFlag)
            node->mKeyNum += 1;
    }
    node->mIsDirty = true;
    node->mLeafFlag = isLeaf;

    /// create a new BTreeNode
    BTreeNode *newNodePtr = new (nothrow) BTreeNode;
    if (NULL == newNodePtr) {
        LOG_FATAL("Memory allocation error!");
        /// if allocate new node error, old keys should be inserted again.
        node->mRoot = NULL;
        for (size_t i = splitterPos; i < keyVec.size(); ++i) {
            BSTInsertNodeWrapper(node->mRoot, keyVec[i], node);
        }
        return NULL;
    }
    /// no error to allocate new BTree node, then init this node now.
    /// for leaf node, we insert key at splitterPos back into new leaf node,
    /// and the splitter will be inserted into their parent node.
    /// for non-leaf node, we do not insert key at splitterPos back into
    /// new non-leaf BTree node, it will be inserted into parent node.
    {
        size_t i;
        if (isLeaf)
            i = splitterPos;
        else
            i = splitterPos+1;
        newNodePtr->mKeyNum = 0;
        LOG_DEBUG("Insert splitted kyes into new node");
        for (; i < keyVec.size(); ++i) {
            BSTInsertNodeWrapper(newNodePtr->mRoot, keyVec[i], newNodePtr);
        }
    }

    /// Assign a slot for new leaf node in db file.
    newNodePtr->mOffsetID = GetNewNodeSlot();
    /// Update bitmap
    UpdateBitMap(newNodePtr->mOffsetID, true);
    
    /// insert to be inserted key
    {
        BSTNode *tmpPtr = keyVec[splitterPos];
        int8 ret = BitComparer(tmpPtr->mValue, tmpPtr->mValLen,
                               bstNodePtr->mValue, bstNodePtr->mValLen);
        /// to be insert node should insert into new node
        BTreeNode *insertIntoPtr = NULL;
        if (ret <= 0) {
            insertIntoPtr = newNodePtr;
        }
        else {
            /// insert into old node
            insertIntoPtr = node;
        }
        /// we use different insert function for leaf and non-leaf node
        if (isLeaf) {
            if (!BSTInsertNodeWrapper(insertIntoPtr->mRoot, bstNodePtr,
                                      insertIntoPtr))
            {
                LOG_ERROR("Insert new key into leaf node error");
                /// TODO - I think this is a critical error, quit the whole
                /// process is allowable.
                return NULL;
            }
            /// For leaf node, update mPointer field for the splitted node to form
            /// new linked list.
            PointerType prePointer = node->mPointer;
            node->mPointer = newNodePtr->mOffsetID;
            newNodePtr->mPointer = prePointer;
            node->mChildPtr = newNodePtr;
        }
        else {
            bool mpFlag = false;
            PointerType oldPtr = node->mPointer;
            if (!BTreeNodeInsertSplittedKey(insertIntoPtr, bstNodePtr,
                                            ptrInfo, mpFlag))
            {
                LOG_ERROR("Insert new key into non-leaf node error");
                /// TODO
                return NULL;
            }
            /// If new key is inserted into the old(offset not changed) node,
            /// this node's mPointer value is depending on return flag of
            /// BTreeNodeInsertSplittedKey(mpointerFlag). If mpointerFlag is
            /// true, it means node->mPointer has been updated by ptrInfo,
            /// if mpionterFlag is false, then its value is splitted key's
            /// mPointer.
            /// So does to the new generated node.
            LOG_DEBUG("mpointerFlag: " << mpFlag);
            if (insertIntoPtr == node) {
                newNodePtr->mPointer = oldPtr;
                /*
                if (!mpFlag)
                    node->mPointer = ptrInfo.second;
                else
                    node->mPointer = keyVec[splitterPos]->mPointer;
                */
                if (!mpFlag)
                    node->mPointer = keyVec[splitterPos]->mPointer;                    
            }
            else if (insertIntoPtr == newNodePtr) {
                node->mPointer = keyVec[splitterPos]->mPointer;
                /*
                if (!mpFlag)
                    newNodePtr->mPointer = ptrInfo.second;                    
                else
                    newNodePtr->mPointer = oldPtr;
                */
                if (!mpFlag)
                    newNodePtr->mPointer = oldPtr;                    
            }
        }
        insertIntoPtr->mIsDirty = true;
        insertIntoPtr->mLeafFlag = isLeaf;
        /// insertIntoPtr->mKeyNum += 1;
    }

    /// Add new node to the cache.
    if (!mBTreeNodeCache.AddNode(newNodePtr, newNodePtr->mOffsetID)) {
        cacheInsertErr = true;
    }

    /// make a copy of splitted key
    {
        if (!CopyBSTNode(retPtr, keyVec[splitterPos]))
            copyBSTNodeErr = true;
        else {
            retPtr->mLeft = retPtr->mRight = NULL;
            retPtr->mPointer = 0;
        }
    }
    
    ++mCachedNodeNum;
    if (mCachedNodeNum > MAX_CACHED_NODE_NUM) {
        /// Write dirty and least recently used BTree Nodes back to
        /// db file.
        FlushCache();
    }

    if (cacheInsertErr || copyBSTNodeErr) {
        if (cacheInsertErr)
            LOG_FATAL("Add new leaf node to cache error!");
        if (copyBSTNodeErr)
            LOG_FATAL("Copy BSTNode Error!");
        UpdateBitMap(newNodePtr->mOffsetID, false);
        /// re-insert original key back to original leaf node.
        for (size_t i = splitterPos; i < keyVec.size(); ++i) {
            BSTInsertNodeWrapper(node->mRoot, keyVec[i], node);
        }
        /// set newLeaf's mRoot to null, so keys in newLeaf will not be deleted.
        newNodePtr->mRoot = NULL;
        delete newNodePtr;
        return NULL;
    }

    newNodePtr->mLeafFlag = isLeaf;
    newNodePtr->mIsDirty = true;
    node->mIsDirty = true;
    
    LOG_DEBUG("Dump splitted node(L): " << node->Dump());
    LOG_DEBUG("Dump splitted node(R): " << newNodePtr->Dump());

    ptrInfo.first = node->mOffsetID;
    ptrInfo.second = newNodePtr->mOffsetID;
    LOG_DEBUG("New node offset(in bitmap): " << newNodePtr->mOffsetID);

    /// record the new generated node into set for rollback
    mNewBTreeNodeSet.insert(newNodePtr->mOffsetID);

    return retPtr;
}

void BPlusTree::GetBSTKeys(BSTNode *ptr, vector<BSTNode*> &vec)
{
    if (NULL == ptr)
        return;

    map<uint64, bool> counter;
    
    stack<BSTNode*> s;
    s.push(ptr);
    counter[(uint64)ptr] = true;
    BSTNode *tmpPtr;
    while (!s.empty()) {
        tmpPtr = s.top();
        while (tmpPtr->mLeft != NULL) {
            if (counter.find((uint64)tmpPtr->mLeft) != counter.end())
                break;
            s.push(tmpPtr->mLeft);
            tmpPtr = tmpPtr->mLeft;
        }
        tmpPtr = s.top();
        s.pop();
        counter[(uint64)tmpPtr] = true;
        vec.push_back(tmpPtr);
        if (tmpPtr->mRight != NULL){
            s.push(tmpPtr->mRight);
        }
    }
}

uint64 BPlusTree::GetNewNodeSlot()
{
    bool found = false;
    for (uint64 i = 0; i < mBitMapLen; ++i)
        if (false == mBitMapPtr[i]) {
            found = true;
            return i;
        }

    if (!found) {
        if (!ExpandBitMap(BITMAP_EXPAND_SIZE)) {
            LOG_ERROR("Expand bit map error");
            return 0;
        }
        return (mBitMapLen-BITMAP_EXPAND_SIZE);
    }
            
    return 0;
}

bool BPlusTree::UpdateBitMap(uint64 n, bool f)
{
    if (n >= mBitMapLen) {
        LOG_INFO("N execeeds bit map length, try to expand bit map");
        if (!ExpandBitMap(BITMAP_EXPAND_SIZE)) {
            LOG_ERROR("Expand bit map error");
            return false;
        }
    }

    mBitMapPtr[n] = f;

    return true;
}

bool BPlusTree::ExpandBitMap(uint64 addedSize)
{
    LOG_INFO("Call ExpandBitMap");
    LOG_INFO("New Bit Map Len: " << (mBitMapLen+addedSize));
    
    bool *newBitMapPtr = new (nothrow) bool[mBitMapLen+addedSize];
    if (NULL == newBitMapPtr) {
        LOG_FATAL("Memory allocation error when expand bitmap");
        return false;
    }
    memset(newBitMapPtr, 0, mBitMapLen+addedSize);
    if (NULL != mBitMapPtr) {
        memcpy(newBitMapPtr, mBitMapPtr, mBitMapLen);
        delete[] mBitMapPtr;
    }
    else {
        newBitMapPtr[0] = true;
    }
    mBitMapPtr = newBitMapPtr;
    mBitMapLen += addedSize;
     
    return true;
}

bool BPlusTree::CopyBSTNode(BSTNode* &destPtr, BSTNode *ptr)
{
    if (NULL == ptr) {
        LOG_INFO("Source BSTNode pointer is NULL");
        destPtr = ptr;
        return true;
    }

    destPtr = new (nothrow) BSTNode;
    LOG_DEBUG("new bstnode: " << destPtr);
    if (NULL == destPtr) {
        LOG_ERROR("Memory allocation error!");
        return false;
    }
    destPtr->mLeft = ptr->mLeft;
    destPtr->mRight = ptr->mRight;
    destPtr->mValue = new (nothrow) uchar[ptr->mValLen];
    if (NULL == destPtr->mValue) {
        LOG_ERROR("Memory allocation error!");
        delete destPtr;
        return false;
    }
    memcpy(destPtr->mValue, ptr->mValue, ptr->mValLen);
    destPtr->mValLen = ptr->mValLen;
    destPtr->mPointer = ptr->mPointer;
    destPtr->mChildPtr = ptr->mChildPtr;

    return true;
}


/**
 * Make a copy of a Binary Search Tree.
 */
bool BPlusTree::CopyBST(BSTNode* &destPtr, BSTNode *sourcePtr)
{
    LOG_CALL();
    bool ret = CopyBSTNode(destPtr, sourcePtr);
    if (!ret) {
        LOG_ERROR("CopyBSTNode Error!");
        destPtr = NULL;
        return false;
    }
    if (NULL == sourcePtr)
        return true;
    if (sourcePtr->mLeft != NULL) {
        ret = CopyBST(destPtr->mLeft, sourcePtr->mLeft);
        if (!ret) {
            LOG_ERROR("CopyBSTNode(mLeft) Error!");
            destPtr->mLeft = NULL;
            return false;
        }
    }
    else
        destPtr->mLeft = NULL;
    if (sourcePtr->mRight != NULL) {
        ret = CopyBST(destPtr->mRight, sourcePtr->mRight);
        if (!ret) {
            LOG_ERROR("CopyBSTNode(mRight) Error!");
            destPtr->mRight = NULL;
            return false;
        }
    }
    else
        destPtr->mRight = NULL;
    return true;
}


/// TODO - UT
bool BPlusTree::CopyBTreeNode(BTreeNode* &destPtr, BTreeNode *ptr)
{
    LOG_CALL();
    /// TODO
    destPtr = new (nothrow) BTreeNode;
    LOG_DEBUG("new btree node: " << destPtr);
    if (!destPtr) {
        LOG_ERROR(MemoryAllocError);
        return false;
    }
    if (!CopyBST(destPtr->mRoot, ptr->mRoot)) {
        LOG_ERROR("Copy BST Error!");
        destPtr->mRoot = NULL;
        delete destPtr;
        return false;
    }
    LOG_DEBUG("new btree node root: " << destPtr->mRoot);
    destPtr->mPointer = ptr->mPointer;
    destPtr->mLeafFlag = ptr->mLeafFlag;
    destPtr->mIsDirty = ptr->mIsDirty;
    destPtr->mKeyNum = ptr->mKeyNum;
    destPtr->mOffsetID = ptr->mOffsetID;

    return true;
}

bool BPlusTree::BTreeNodeInsertSplittedKey(
    BTreeNode *currNodePtr, BSTNode *newKey,
    std::pair<PointerType, PointerType> &ptrInfo,
    bool &mpointerChangedFlag)
{
    /// if the new key should be inserted at the beginning of all keys, then assign
    /// ptrInfo->first to new keys' mPointer field, ptrInfo->second to mPointer
    /// field of newKey's subsequent node.
    /// if the new key should be inserted at the end of all keys, ptrInfo->first should
    /// be assigned to new keys' mPointer field, and ptrInfo->second assigned to
    /// current node's mPointer field(last pointer to child node).
    /// if the new key is not the minimum or maximum key, then assign ptrInfo->first
    /// to new key's mPointer field, and ptrInfo->second to new key's subsequent node.
    LOG_DEBUG("Call BTreeNodeInsertSplittedKey");
    LOG_DEBUG("To insert key: " << Key(newKey->mValue, newKey->mValLen).ToString()
              << "\nPtrInfo: " << ptrInfo.first << ", " << ptrInfo.second);
    bool isMax, isMin;
    isMax = isMin = true;
    BSTNode *subsequentBSTNodePtr = NULL;
    BSTNode *toInsertKey = NULL;

    if (!CopyBSTNode(toInsertKey, newKey)) {
        LOG_FATAL("Memory allocation error!");
        return false;
    }

    /// insert new key into BST, and get whether the inserted node is max/min node
    /// and its subsequent node pointer.
    {
        bool foundFlag = false;
        BSTInsertNodeAndGetSth(currNodePtr->mRoot, toInsertKey,
                               isMin, isMax, foundFlag,
                               subsequentBSTNodePtr);
    }

    LOG_DEBUG("Is Max: " << isMax << ", Is Min: " << isMin);

    if (isMin && isMax) {
        /// no key in current node
        toInsertKey->mPointer = ptrInfo.first;
        currNodePtr->mPointer = ptrInfo.second;
        mpointerChangedFlag = true;
        currNodePtr->mRoot = toInsertKey;
    }
    else if (isMin && subsequentBSTNodePtr != NULL) {
        toInsertKey->mPointer = ptrInfo.first;
        subsequentBSTNodePtr->mPointer = ptrInfo.second;
    }
    else if (isMax) {
        toInsertKey->mPointer = ptrInfo.first;
        currNodePtr->mPointer = ptrInfo.second;
        mpointerChangedFlag = true;
    }
    else if (!isMin && !isMax && (subsequentBSTNodePtr != NULL)) {
        toInsertKey->mPointer = ptrInfo.first;
        subsequentBSTNodePtr->mPointer = ptrInfo.second;
    }
    else {
        LOG_ERROR("Insert new key error! IsMin: " << isMin << ", isMax: " <<
                  isMax << ", subsequentBSTNodePtr: " << subsequentBSTNodePtr);
        delete toInsertKey;
        return false;
    }

    /// LOG_DEBUG("After insert: " << currNodePtr->Dump());
    
    return true;
}

void BPlusTree::BSTInsertNodeAndGetSth(BSTNode *rootPtr, BSTNode *toInsertKey,
                                       bool &isMin, bool &isMax, bool &flag,
                                       BSTNode* &subsequentBSTNodePtr)
{
    if (NULL == rootPtr)
        return;

    BSTNode *ptr = rootPtr;
    int cmpRet = BitComparer(ptr->mValue, ptr->mValLen,
                             toInsertKey->mValue, toInsertKey->mValLen);
    if (0 == cmpRet) {
        LOG_FATAL("Duplicated key founded, No duplicate key is allowed" <<
                  " in non leaf node");
        delete toInsertKey;
        return;
    }
    else if (1 == cmpRet) {
        if (NULL == subsequentBSTNodePtr)
            subsequentBSTNodePtr = ptr;
        else {
            int8 tmpCmp = BitComparer(subsequentBSTNodePtr->mValue,
                                      subsequentBSTNodePtr->mValLen,
                                      ptr->mValue, ptr->mValLen);
            if (tmpCmp > 0)
                subsequentBSTNodePtr = ptr;
        }
        
        isMax = false;
        if (NULL != ptr->mLeft) {
            BSTInsertNodeAndGetSth(ptr->mLeft, toInsertKey, isMin,
                                   isMax, flag, subsequentBSTNodePtr);
        }
        else {
            ptr->mLeft = toInsertKey;
            toInsertKey->mRight = toInsertKey->mLeft = NULL;
            return;
        }
    }
    else {
        isMin = false;
        if (NULL != ptr->mRight) {
            BSTInsertNodeAndGetSth(ptr->mRight, toInsertKey, isMin,
                                   isMax, flag, subsequentBSTNodePtr);
        }
        else {
            ptr->mRight = toInsertKey;
            toInsertKey->mRight = toInsertKey->mLeft = NULL;
            return;
        }
    }
}

void BPlusTree::DumpTree(vector<Key> &keys, vector<PointerType> &vals)
{
    DumpKeyAndVal(keys, vals, mRootPtr);
}

void BPlusTree::DumpKeyAndVal(vector<Key> &keys, vector<PointerType> &vals,
                              BTreeNode *ptr)
{
    if (NULL == ptr)
        return;
    /// Get all offset(in bitmap) from current BPlusTree node.
    vector<PointerType> subTreePointer;
    vector<Key> subTreeKey;
    DFSKeysAndValues(ptr->mRoot, subTreeKey, subTreePointer);
    subTreePointer.push_back(ptr->mPointer);

    /// if current node is leaf, then get all keys and values.
    if (true == ptr->mLeafFlag) {
        for (size_t i = 0; i < subTreeKey.size(); ++i) {
            keys.push_back(subTreeKey[i]);
            vals.push_back(subTreePointer[i]);
        }
    }
    else { /// non leaf, recursive visit subtree
        for (size_t i = 0; i < subTreePointer.size(); ++i) {
            PointerType p = subTreePointer[i];
            BTreeNode *nextNodePtr;
            /// check next node in the cache or not
            if (mBTreeNodeCache.CheckNodeExistance(p)) {
                nextNodePtr = mBTreeNodeCache.GetNode(p);
            }
            else {
                /// read node from db file
                nextNodePtr = new (nothrow) BTreeNode;
                if (NULL == nextNodePtr) {
                    LOG_ERROR(MemoryAllocError);
                    return;
                }
                if (!GetNode(nextNodePtr, p)) {
                    LOG_ERROR(GetNodeError);
                    delete nextNodePtr;
                    return;
                }
                /// put it into cache
                if (!mBTreeNodeCache.AddNode(nextNodePtr, p)) {
                    LOG_ERROR(AddToCacheError);
                    delete nextNodePtr;
                    return;
                }
            }
            DumpKeyAndVal(keys, vals, nextNodePtr);
        } /// for all pointer for next nodes
    }
}

/// TODO - UT
bool BPlusTree::FlushCache()
{
    LOG_DEBUG("Call FlushCache");
    /// get to be flushed nodes
    vector<PointerType> s;
    s = mBTreeNodeCache.GetToFlushNodes();
    LOG_DEBUG("# of nodes will be removed from cache: " << s.size());

    size_t i = 0;
    for (; i < s.size(); ++i) {
        BTreeNode *ptr = mBTreeNodeCache.GetNode(s[i]);
        if (NULL == ptr) {
            LOG_ERROR("BTreeNode cache return a null pointer");
        }
        else {
            if (ptr->mIsDirty) {
                LOG_DEBUG("Write dirty node back to disk, id: " << 
                          ptr->mOffsetID);
                /// Write the dirty node back to disk
                if (!WriteBTreeNodeToDisk(ptr)) {
                    LOG_ERROR("Write BTree Node back to disk error");
                }
                else {
                    mBTreeNodeCache.RemoveNode(ptr->mOffsetID);
                    delete ptr;
                }
            }
            else {
                LOG_DEBUG("Remove not dirty node from cache");
                mBTreeNodeCache.RemoveNode(ptr->mOffsetID);
                delete ptr;
            }
        }
    } /// for

    return true;
}

/// TODO - UT
bool BPlusTree::WriteBTreeNodeToDisk(BTreeNode *ptr)
{
    /// seek to right file offset
    LOG_CALL();
    LOG_DEBUG("BTreeNode offset: " << ptr->mOffsetID);

    mFileHandler.Seek(ptr->mOffsetID*BLOCK_SIZE);
    string s;
    BTreeNode2Bin(ptr, s);
    if (!mFileHandler.Write(s)) {
        LOG_ERROR("Write BTreeNode to disk error");
        return false;
    }
    return true;
}

/// TODO - UT
bool BPlusTree::WriteBTreeHead()
{
    LOG_CALL();
    string str;
    str.reserve(BLOCK_SIZE);
    /// write db versoin
    str += mVersion;
    str += KEYS_DELIMITER_STR;
    /// write key num
    string s = UInt2Binary(mKeyNum, mEndian);
    LOG_DEBUG("key num: " << mKeyNum << "->" << s);
    str += s;
    /// write max allowd key num
    s = UInt2Binary(mMaxKeyNum, mEndian);
    LOG_DEBUG("max key num: " << mMaxKeyNum << "->" << s);
    str += s;
    /// write root node offset
    s = UInt2Binary(mRootOffset, mEndian);
    LOG_DEBUG("root offset: " << mRootOffset << "->" << s);
    str += s;
    /// fill unused bytes with '\0'
    for (size_t i = str.length(); i < BLOCK_SIZE; ++i)
        str.push_back('\0');

    /// write to db file
    mFileHandler.Seek(0);
    if (!mFileHandler.Write(str)) {
        LOG_ERROR("Write head info to db file error");
        return false;
    }
    return true;
}

/// TODO - UT
bool BPlusTree::BTreeNode2Bin(BTreeNode *ptr, string &str)
{
    /// pre-allocate block size
    str.clear();
    str.reserve(BLOCK_SIZE);
    /// write # of keys
    string s = UInt2Binary(ptr->mKeyNum, mEndian);
    LOG_DEBUG("key num: " << s);
    str += s;
    /// write is leaf flag
    int32 leaf = ptr->mLeafFlag;
    s = UInt2Binary(leaf, mEndian);
    str += s;
    /// write keys
    vector<Key> keys;
    vector<PointerType> vals;
    DFSKeysAndValues(ptr->mRoot, keys, vals);
    LOG_DEBUG("# of keys: " << keys.size());
    for (size_t i = 0; i < keys.size(); ++i) {
        Key key = keys[i];
        LOG_DEBUG("Write key: " << key.ToString());
        /// write key len
        s = UInt2Binary(key.mKeyLen, mEndian);
        str += s;
        /// write key
        for (uint32 j = 0; j < key.mKeyLen; ++j)
            str += key.mKey[j];
        /// write offset
        s = UInt2Binary(vals[i], mEndian);
        str += s;
    }
    /// write last offset(or current leaf's next subling leaf node)
    s = UInt2Binary(ptr->mPointer, mEndian);
    str += s;
    /// set the unused bytes.
    for (size_t i = str.size(); i < BLOCK_SIZE; ++i)
        str.push_back('\0');
    LOG_DEBUG("Bin repr size: " << str.length());

    return true;
}


bool BPlusTree::Commit()
{
    LOG_CALL();
    /// update head part
    mFileHandler.Seek(0);
    if (!WriteBTreeHead()) {
        LOG_ERROR("Write db head part error");
        return false;
    }
    /// write modified nodes to db file
    vector<PointerType> allNodes =  mBTreeNodeCache.GetAllCachedNodes();
    for (size_t i = 0; i < allNodes.size(); ++i) {
        BTreeNode *ptr = mBTreeNodeCache.GetNode(allNodes[i]);
        if (NULL == ptr) {
            LOG_ERROR("BTreeNodeCache GetNode return null pointer");
            return false;
        }
        if (!WriteBTreeNodeToDisk(ptr)) {
            LOG_ERROR("Call WriteBTreeNodeToDisk error");
            return false;
        }
    }
    /// remove all cached from cache.
    for (size_t i = 0; i < allNodes.size(); ++i) {
        BTreeNode *ptr = mBTreeNodeCache.GetNode(allNodes[i]);
        delete ptr;
    }
    return true;
}

bool BPlusTree::ModifiedBTreeNodeBackup(BTreeNode *ptr)
{
    LOG_CALL();
    BTreeNode *copiedPtr = NULL;
    if (!(CopyBTreeNode(copiedPtr, ptr))) {
        LOG_ERROR("Copy BTreeNode Error!");
        return false;
    }
    mModifiedBTreeNodeSet.AddNode(copiedPtr, copiedPtr->mOffsetID);
    return true;
}

bool BPlusTree::RemovedBTreeNodeBackup(BTreeNode *ptr)
{
    LOG_CALL();
    BTreeNode *copiedPtr = NULL;
    if (!(CopyBTreeNode(copiedPtr, ptr))) {
        LOG_ERROR("Copy BTreeNode Error!");
        return false;
    }
    mRemovedBTreeNodeSet.AddNode(copiedPtr, copiedPtr->mOffsetID);
    return true;
}

void BPlusTree::ClearMinorRollbackCache()
{
    LOG_CALL();
    /// clear new generated node set
    mNewBTreeNodeSet.clear();
    /// for mModifiedBTreeNodeSet and mRemovedBTreeNodeSet, if rollback happened,
    /// then related rollback action will clear these two sets.
    vector<PointerType> s;
    s = mModifiedBTreeNodeSet.GetAllCachedNodes();
    LOG_DEBUG("Dump of mModifiedBTreeNodeSet:\n" 
              << mModifiedBTreeNodeSet.Dump());
    for (size_t i = 0; i < s.size(); ++i) {
        BTreeNode *ptr = mModifiedBTreeNodeSet.GetNode(s[i]);
        LOG_DEBUG("Del: " << ptr);
        delete ptr;
    }
    mModifiedBTreeNodeSet.Clear();

    s = mRemovedBTreeNodeSet.GetAllCachedNodes();
    LOG_DEBUG("Dump of mRemovedBTreeNodeSet:\n" 
              << mRemovedBTreeNodeSet.Dump());
    for (size_t i = 0; i < s.size(); ++i) {
        BTreeNode *ptr = mRemovedBTreeNodeSet.GetNode(s[i]);
        delete ptr;
    }
    mRemovedBTreeNodeSet.Clear();
}

bool BPlusTree::InsertRollback()
{
    LOG_CALL();
    /// remove modified and new generated btree nodes.
    typeof(mNewBTreeNodeSet.begin()) iter;
    for (iter = mNewBTreeNodeSet.begin(); iter != mNewBTreeNodeSet.end(); ++iter)
    {
        BTreeNode *ptr = mBTreeNodeCache.GetNode(*iter);
        mBTreeNodeCache.RemoveNode(*iter);
        delete ptr;
        UpdateBitMap(*iter, false);
    }
    vector<PointerType> modifiedBTreeNodeID = 
        mModifiedBTreeNodeSet.GetAllCachedNodes();
    for (size_t i = 0; i < modifiedBTreeNodeID.size(); ++i) {
        PointerType id = modifiedBTreeNodeID[i];
        BTreeNode *ptr = mBTreeNodeCache.GetNode(id);
        mBTreeNodeCache.RemoveNode(id);
        delete ptr;
    }
    /// put the backup node into btree cache.
    for (size_t i = 0; i < modifiedBTreeNodeID.size(); ++i) {
        PointerType id = modifiedBTreeNodeID[i];
        bool ret = mBTreeNodeCache.CheckNodeExistance(id);
        LOG_DEBUG("Check node in cache: " << ret);
        BTreeNode *ptr = mModifiedBTreeNodeSet.GetNode(id);
        mBTreeNodeCache.AddNode(ptr, id);
    }
    /// clear minor modified cache
    mNewBTreeNodeSet.clear();
    mModifiedBTreeNodeSet.Clear();
    return true;
}


bool BPlusTree::RemoveRollback()
{
    LOG_CALL();
    vector<PointerType> removedBTreeNodeID = 
        mRemovedBTreeNodeSet.GetAllCachedNodes();
    for (size_t i = 0; i < removedBTreeNodeID.size(); ++i) {
        PointerType id = removedBTreeNodeID[i];
        BTreeNode *ptr = mRemovedBTreeNodeSet.GetNode(id);
        mBTreeNodeCache.AddNode(ptr, id);
        delete ptr;
    }
    /// clear minor rollback cache
    mRemovedBTreeNodeSet.Clear();
    return true;
}

bool BPlusTree::BSTInsertNodeWrapper(BSTNode* &root, BSTNode *bstNodePtr,
                                     BTreeNode *btreeNodePtr)
{
    LOG_CALL();
    bool dupFlag = false;
    if (!BSTInsertNode(root, bstNodePtr, btreeNodePtr, dupFlag)) {
        LOG_ERROR("BSTInsertNode Error!");
        return false;
    }
    if (!dupFlag && btreeNodePtr)
        btreeNodePtr->mKeyNum += 1;

    return true;
}

/// TODO: UT
bool BPlusTree::Delete(const uchar *key, uint32 keyLen)
{
    LOG_CALL();

    /// clear cached b+ tree node in minor rollback cache
    ClearMinorRollbackCache();

    /// delete key from b+ tree recursivly, and adjust some nodes(redistribute).
    if (!RecursiveDelete(Key(key, keyLen), mRootPtr)) {
        LOG_ERROR("Delete key from B+ tree error, key: " <<
                  Key(key, keyLen).ToString());
        return false;
    }

    return true;
}


/// TODO - UT
bool BPlusTree::RecursiveDelete(const Key &key, BTreeNode *currNodePtr,
                                BTreeNodePtr parentPtr)
{
    LOG_CALL();

    /// if current node is leaf node, just find the key and remove it!
    if (currNodePtr->mLeafFlag) {
        BSTNodePtr ptr = SearchBST(currNodePtr, key);
        if (NULL == ptr) {
            LOG_INFO("Key: " << key.ToString() << " not in B+ tree");
            return true;
        }
        else {
            /// make a backup of current node.
            ModifiedBTreeNodeBackup(currNodePtr);
            /// remove to-del key from leaf node.
            if (!RemoveKeyFromBTreeNode(currNodePtr, key)) {
                LOG_ERROR("RemoveKeyFromBTreeNode error, key: " << key.ToString());
                return false;
            }
            /// TODO
        }
    }
    

    return true;
}

BSTNode* BPlusTree::SearchBST(BTreeNode *ptr, const Key &key)
{
    LOG_CALL();

    BSTNode *bstPtr = ptr->mRoot;
    while (bstPtr) {
        int8 cmpRet = BitComparer(bstPtr->mValue, bstPtr->mValLen,
                                  key.mKey, key.mKeyLen);
        if (0 == cmpRet)
            return bstPtr;
        else if (1 == cmpRet)
            bstPtr = bstPtr->mLeft;
        else
            bstPtr = bstPtr->mRight;
    }

    return bstPtr;
}

/// TODO - UT
bool BPlusTree::RemoveKeyFromBTreeNode(BTreeNodePtr btreeNodePtr, const Key &key)
{
    /// this is an simple implementation, we just get all keys into an array,
    /// and insert them back except to-del key.
    vector<BSTNodePtr> keySet;
    DFSGetBSTNodes(btreeNodePtr->mRoot, keySet);

    btreeNodePtr->mRoot = NULL;
    bool dummy = false;
    BSTNodePtr toDelBSTNodePtr = NULL;
    for (size_t i = 0; i < keySet.size(); ++i) {
        int8 cmpRet = BitComparer(keySet[i].mValue, keySet[i].mValLen,
                                  key.mKey, key.mKeyLen);
        if (cmpRet != 0)
            BSTInsertNode(btreeNodePtr->mRoot, keySet[i], btreeNodePtr, dummy);
        else
            toDelBSTNodePtr = keySet[i];
    }
    LOG_DEBUG("To remove key: " << toDelBSTNodePtr->ToString());

    delete toDelBSTNodePtr;
    
    return true;
}
