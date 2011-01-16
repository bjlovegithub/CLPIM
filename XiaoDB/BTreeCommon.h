/**
 * Common type define and data structure for BPlusTree.
 */

#ifndef BILLJEFF_BTREE_COMMON_H
#define BILLJEFF_BTREE_COMMON_H

#include "CommonInclude.h"
#include <string>

namespace XiaoDB
{
/**
 * Common const string definition
 */
#define MemoryAllocError "Memory Allocation Error!"
#define GetNodeError     "Get Node from DB file Error!"
#define AddToCacheError  "Add BTreeNode to Cache Error!"

/**
 * The BPlusTree dumped file can contain 2^64 nodes. But for the limit of
 * element number and tree height, it cannot reach such amount(nearly 2^50
 * nodes in tree).
 */
typedef uint64 PointerType;

/**
 * struct to keep key info, including uchar array for key, and its length
 */
struct Key
{
    uchar *mKey;
    uint32 mKeyLen;

    /**
     * Ctor
     */
    Key();
    Key(const uchar *key, uint8 len);
    Key(const Key &rhs);

    /**
     * Dtor
     */
    ~Key();

    /**
     * Get the string representation of key
     */
    std::string ToString() const;
};

/**
 * Binary search tree node.
 */
struct BTreeNode;
struct BSTNode
{
    /**
     * Ctor
     */
    BSTNode();

    /**
     * BSTNode ctor with parameters.
     * @param key Pointer to key.
     * @param keyLen Length of key
     * @param val Value
     */
    BSTNode(const uchar *key, uint32 keyLen, PointerType val);
    
    /**
     * Dtor
     */
    ~BSTNode();

    bool IsOk();

    BSTNode *mLeft, *mRight;
    /// Key
    unsigned char *mValue;
    size_t mValLen;
    /// For non-leaf node, it points to the node which is less than current key
    /// For leaf node, its value is the offset from beginning of data file.
    PointerType mPointer;
    /// Child node pointer when it was loaded in the memory
    BTreeNode *mChildPtr;
    /// If we use the Ctor with parameter, we should check BSTNode construction
    /// is successfully or not.
    bool mIsOk;
};

/**
 * B+ Tree Node
 */
struct BTreeNode
{
    /**
     * Ctor
     */
    BTreeNode();
    /**
     * Dtor
     */
    ~BTreeNode();

    /**
     * Dump BTreeNode
     */
    std::string Dump();

    /// BST root.
    BSTNode *mRoot;
    /// For the non-leaf nodes:
    /// It holds the last pointer in B+ Tree Node. Assume there are n keys in
    /// current node, BST takes n keys and their pointers, but there are
    /// n+1 pointers in BST node. So I use pointer to hold the last one.
    /// For the leaf nodes:
    /// It holds the next node' position in the linked list.
    PointerType mPointer;
    /// Child node pointer in memory.
    BTreeNode *mChildPtr;
    /// Flag using to indicate current node is leaf or not.
    bool mLeafFlag;
    /// If node data has been modified, set it to true.
    bool mIsDirty;
    /// Number of keys in the node
    uint32 mKeyNum;
    /// Current node's offset in db file(index in mBitMap)
    PointerType mOffsetID;
};

}

#endif /// BILLJEFF_BTREE_COMMON_H
