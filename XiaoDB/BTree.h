/**
 * B+ Tree implementation.
 * Note: All interfaces are NOT THREAD SAFE.
 * By billjeff
 * Revision History:
 *   2010/11/1 Init code implemetation.
 */

#ifndef BILLJEFF_BTREE_H
#define BILLJEFF_BTREE_H

#include <string>
#include <vector>

#include "FileUtil.h"
#include "CommonInclude.h"
#include "TreeNodeCacheManager.h"
#include "BTreeCommon.h"

/*** For Unittest ***/
bool OpenTest(void);
bool CheckDataTest(void);
bool ParseKeyAndValueTest(void);
bool BSTInsertNodeTest(void);
bool BitComparerTest(void);
bool GetNodeTest(void);
bool GetBSTKeysTest(void);
bool ExpandBitMapTest(void);
bool GetNewNodeSlotTest(void);
bool SplitLeafTest(void);
bool SplitNonLeafTest(void);
bool CopyBSTNodeTest(void);
bool BSTInsertNodeAndGetSthTest(void);
bool BTreeNodeInsertSplittedKeyTest(void);

namespace XiaoDB
{

/**
 * Class BTree holds a B+ Tree.
 */
class BPlusTree
{
public:
    /**
     * Constructor, initialize B+ Tree.
     */
    BPlusTree();

    /**
     * Dectructor
     */
    ~BPlusTree();

    /**
     * Open B+ tree.
     * @param fileName B+ tree file.
     * @return Return true if open successfully, otherwise return false.
     */
    bool Open(const std::string &fileName);
    

    /**
     * Insert a key with its value into B+ tree.
     * @param key Key
     * @param keyLen Key length
     * @param value Value
     * @return True for inserting successfully, otherwise return false.
     */
    bool Insert(const uchar *key, uint32 keyLen, PointerType value);

    void Delete();

    void Search();

    /**
     * Dump the whole BPTree key and values
     * @param keys All BPTree keys
     * @param vals All values
     */
    void DumpTree(std::vector<Key> &keys,
                  std::vector<PointerType> &vals);


private:
    /**
     * Read header info(version, etc) from B+ tree file, and check
     * data version compatiblity.
     * @return If data is compatible, return true, otherwise return false.
     */
    bool CheckData();

    /**
     * Check data compability.
     * @param btreeVersion B+ tree version(mVersion)
     * @param dataVersion Data version
     * @return Return true if data is compatible, otherwise false.
     */
    bool CheckCompatibility(const std::string &btreeVersion,
                            const std::string &dataVersion);

    /**
     * Read data of nth node from B+ tree file.
     * @param nodePtr Pointer to hold node info.
     * @param n The nth node data in B+ tree file.
     * @return Return true if operation successeded, otherwise false.
     */
    bool GetNode(BTreeNode *nodePtr, uint64 n);
        
    /**
     * Insert a node into Binary Search Tree.
     * @param root Pointer to the root of BST.
     * @param bstNodePtr Pointer to BST.
     * @param ptr Pointer to BTreeNode, ptr->mRoot is equal to parameter root.
     * @return Return true if inserting node with no error, otherwise false.
     */
    bool BSTInsertNode(BSTNode* &root, BSTNode *bstNodePtr, BTreeNode *ptr);

    /**
     * NOTE: DEPRECATED
     * Parse key and value from B+ tree file.
     * @param bstNodePtr BST node pointer.
     * @param bstNodeStr String represetation for node.
     * @return Return true if parse successfully, otherwise return false.
     */
    bool ParseKeyAndValue(BSTNode *bstNodePtr, const std::string &bstNodeStr);

    /**
     * Compare two memory location byte by byte.
     * @param first Pointer to first memory location.
     * @param len1 Number of bytes in first location.
     * @param second See above.
     * @param len2 See above.
     * @return Return 0 if they are equal, return 1 if frist is greater than
     *         second one, otherwise return -1.
     */
    int8 BitComparer(void *first, size_t len1, void *second, size_t len2);

    /**
     * Detect current system endian.
     */
    bool CheckEndian();

    /**
     * Search the BPlusTree, insert the new element at the right place,
     * split node if necessary.
     * @param toInsertNode Pointer to the to be inserted node which contain key/
     *                     keyLen/pointer info.
     * @param currNodeptr Pointer to current BPlusTree node.
     * @param hasNewKey Indicating child node of current node has splitted or not.
     * @param newKey Pointer to key info of splitted child node, this node will be
     *        copied and insert into parent node.
     * @param ptrInfo It stores the two new node offset(pointer) info, if the children
     *        node has been splitted.
     * @return 
     */
    bool RecursiveInsert(BSTNode *toInsertNode, BTreeNode *currNodePtr,
                         bool &hasNewKey, BSTNode* &newKey,
                         std::pair<PointerType, PointerType> &ptrInfo);

    /**
     * Split the BTree node into two new node by the middle key of the BST.
     * @param currNodePtr Pointer to the to be splitted BPlusTree node.
     * @param bstNodePtr To be inserted node pointer
     * @param isLeaf True for splitted node is leaf node, otherwise false.
     * @param ptrInfo Old node and new node offset is kept in this parameter.
     * @return Pointer to the middle key of the old BPlusTree node. Return NULL
     *         for some errors.
     */
    BSTNode* SplitBTreeNode(BTreeNode *node, BSTNode *bstNodePtr, bool isLeaf,
                            std::pair<PointerType, PointerType> &ptrInfo);

    /**
     * Traverse BST and store its key in the output vector in asending order.
     * @param nodePtr BST node poiter.
     * @param vec All keys kept in vec.
     */
    void GetBSTKeys(BSTNode *ptr, std::vector<BSTNode*> &vec);

    /**
     * Get a new slot(empty block in db file) from mBitMap;
     */
    uint64 GetNewNodeSlot();

    /**
     * Update mBitMap, set mBitMap[n] = f.
     * @param n BPlusTree Node ID n, n*block_size is the beginning position
     *          of BPlusTree Node.
     * @param f Value which will be assigned to mBitMap[n].
     * @return True for success, otherwise false(expand bit map error).
     */
    bool UpdateBitMap(uint64 n, bool f);

    /**
     * Expand mBitMap, add newSize to original bit map, and keep the
     * content unchanged.
     * @param addedSize Expanded size.
     * @return Return true if expansion sucessfully, otherwise return false.
     */
    bool ExpandBitMap(uint64 addedSize);

    /**
     * Insert the splitted key into non-leaf node. Note, we have to insert the new
     * key into proper position(BST), and adjust the two new generated child-node
     * pointer.
     * @param currNodePtr Pointer to current node
     * @param newKey Pointer to BSTNode which contains new key value.
     * @param ptrInfo Pair contains left and right child node.
     * @param mpointerChangedFlag If currNodePtr->mPointer is changed, then set
     *        it to true.
     * @return Return true if insert key successfully, otherwise return false.
     */
    bool BTreeNodeInsertSplittedKey(BTreeNode *currNodePtr, BSTNode *newKey,
                                    std::pair<PointerType, PointerType> &ptrInfo,
                                    bool &mpointerChangedFlag);

    /**
     * Make a new BSTNode as the input BSTNode.
     * @param destPtr Pointer to new BSTNode.
     * @param ptr Source BSTNode pointer.
     * @return Return true if copy successfully, otherwise return false.
     */
    bool CopyBSTNode(BSTNode* &destPtr, BSTNode *ptr);

    /**
     * Insert new key into BST, and get info about the inserted key is max/min
     * key in BST, and its subsequent key.
     * @param rootPtr Pointer to BST.
     * @param toInsertKey Pointer to be inserted BSTNode.
     * @param isMin Set it to false if insert key is not the minimum key in BST.
     * @param isMax Set it to false if insert key is not the maximum key in BST.
     * @param flag If key has been inserted, then set it to true.
     * @param subsequentBSTNodePtr Subsequent just follow insert key if it have.
     */
    void BSTInsertNodeAndGetSth(BSTNode *rootPtr, BSTNode *toInsertKey,
                                bool &isMin, bool &isMax, bool &flag,
                                BSTNode* &subsequentBSTNodePtr);

    /**
     * Comit the change to DB file.
     */
    /// TODO

    /**
     * Rollback change to previous status when some error happened.
     * When tree node is modified, the old tree node will be added to a
     * place. When commit failed, get the old node and write them back
     * to DB file.
     */
    /// TODO

    /**
     * Dump all keys and values in BTree.
     * All keys will be kept in parameter keys, and corresponding values are
     * kept in parameter vals.
     */
    void DumpKeyAndVal(std::vector<Key> &keys,
                       std::vector<PointerType> &vals,
                       BTreeNode *nodePtr);

    /**
     * Remove several cached nodes from mBTreeNodeCache. For TreeNodeCacheManager,
     * there is an inner cache which keeps latest used node id. When this
     * function is called, cached node not in this inner cache will be flushed
     * to disk file.
     */
    bool FlushCache();
    
    
private:
    /// All nodes in B+ tree are kept in a file. Each block's size is
    /// defined by constant BLOCK_SIZE(see the constant definition at
    /// the beginning of this file). Assume file size is M, there are
    /// M/BLOCK_SIZE blocks in the file. I use a bit map to indicate that
    /// whether a block is in use or not. If a node is deleted from
    /// B+ tree, then this block will be setted to false(not in use).
    bool *mBitMapPtr;
    uint64 mBitMapLen;

    /// B+ tree file name
    std::string mFileName;
    /// File handler for B+ tree file.
    CLPIM::FileHandler mFileHandler;

    /// If it is true, it means B+ tree has been loaded from disk
    bool mInitFlag;

    /// B+ tree version info
    std::string mVersion;

    /// B+ tree root and current node.
    BTreeNode *mRootPtr, *mCurrNodePtr;

    /// Lazy flag. If it is true, then the modified nodes will not be written
    /// to db file immediately. If it is false, any modification will write to
    /// db file quickly.
    bool mLazy;
    /// B tree node has changed or not
    bool mChangedFlag;

    /// Number of keys in BPlusTree
    uint64 mKeyNum;
    /// Max number of keys in BPlusTree
    uint64 mMaxKeyNum;

    /// indicate system endian
    int8 mEndian;

    /// root offset id in db file, id*block_size is the true offset of root block
    uint64 mRootOffset;

    /// BPlusTree node cache.
    TreeNodeCacheManager mBTreeNodeCache;
    uint32 mCachedNodeNum;

    /// mBTreeNodeBackupCache is used to backup the orginal BTree node before
    /// any modification. mModifiedBTreeNodeSet keeps the modified BTree node
    /// ID(offset id from bitmap). These two fields are used when minor
    /// modification error happened and rollback. When error happened, node ID
    /// in mModifiedBTreeNodeSet will be deleted from mBTreeNodeCache, and
    /// re-insert BTree node in mBTreeNodeBackupCache back into mBTreeNodeCache.
    /// For transaction, we will not use this mechanism. We will make a copy of
    /// entire BTree DB file, and remove this copy only transaction commit
    /// successfully.
    TreeNodeCacheManager mBTreeNodeBackupCache;
    std::vector<PointerType> mModifiedBTreeNodeSet;

    /// During the key insert, if to-insert key is in BPTree, then this flag will
    /// be set to true, otherwise to false;
    bool mInsertKeyDupFlag;
    

/**
 * For Unittest
 */
friend bool ::OpenTest(void);
friend bool ::CheckDataTest(void);
friend bool ::ParseKeyAndValueTest(void);
friend bool ::BitComparerTest(void);
friend bool ::BSTInsertNodeTest(void);
friend bool ::GetNodeTest(void);
friend bool ::GetBSTKeysTest(void);
friend bool ::ExpandBitMapTest(void);
friend bool ::GetNewNodeSlotTest(void);
friend bool ::SplitLeafTest(void);
friend bool ::SplitNonLeafTest(void);
friend bool ::CopyBSTNodeTest(void);
friend bool ::BSTInsertNodeAndGetSthTest(void);
friend bool ::BTreeNodeInsertSplittedKeyTest(void);

};

} // namespace XiaoDB



#endif // BILLJEFF_BTREE_H
