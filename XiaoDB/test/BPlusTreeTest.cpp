#include "ut_tools/ut_tool.h"
#include "Log.h"
#include "BTree.h"
#include "Random.h"
#include "HashTable.hpp"

#include <sstream>
#include <cstring>
#include <cstdio>
#include <algorithm>

using namespace XiaoDB;
using namespace CLPIM;
using namespace std;

/// Delimiter between key and value in B+ node
#define KEY_VALUE_DELIMITER     0x01010101
#define KEY_VALUE_DELIMITER_STR "\01\01\01\01"
/// Delimiter between B+ nodes in B+ file
#define KEYS_DELIMITER          0x02020202
#define KEYS_DELIMITER_STR      "\02\02\02\02"
#define DELIMITER_SIZE          4
#define BITMAP_EXPAND_SIZE      30000

/**
 * Helper functions
 */
void AssignKey(BSTNode *ptr, uchar *val, uint32 len)
{
    ptr->mValue = new uchar[len];
    ptr->mValLen = len;
    memcpy(ptr->mValue, val, len);
    ptr->mPointer = len;
}

/*** Unit Tests ***/
bool OpenTest(void)
{
    BPlusTree tree;
    bool ret = tree.Open("data/GetNodeTest.in");
    TEST_EQUAL(ret, true);
    TEST_EQUAL(tree.mRootPtr->mPointer, 0);

    cout << "End" << endl;
    return true;
}
ADD_CASE(OpenTest);

bool CheckDataTest()
{
    BPlusTree tree;
    bool ret;
    ret = tree.mFileHandler.Open("data/CheckDataTest.in");
    LOG_DEBUG(ret);
    ret = tree.CheckData();
    TEST_EQUAL(ret, true);

    TEST_EQUAL(tree.mKeyNum, 123);
    TEST_EQUAL(tree.mMaxKeyNum, 100000000UL);
    TEST_EQUAL(tree.mRootOffset, 1);    

    cout << "End" << endl;    
    return true;
}
ADD_CASE(CheckDataTest);

bool ParseKeyAndValueTest()
{
    BPlusTree tree;
    bool ret;
    BSTNode *nodePtr = new BSTNode;
    nodePtr->mLeft = nodePtr->mRight = NULL;
    string str = "0008billjeff";
    str += KEY_VALUE_DELIMITER_STR;
    //str += "\0\0\0\0\0\0\0\1";
    size_t ptr = str.length();
    str.resize(ptr+9);
    /// TODO - Consider the endian, this is just for big endian machine.
    str[ptr] = '\1';
    str[ptr+1] = '\0';
    str[ptr+2] = '\0';
    str[ptr+3] = '\0';
    str[ptr+4] = '\0';
    str[ptr+5] = '\0';
    str[ptr+6] = '\0';
    str[ptr+7] = '\0';
    ret = tree.ParseKeyAndValue(nodePtr, str);
    TEST_EQUAL(ret, true);
    unsigned char destKey[] = "billjeff";
    int comRet = memcmp(nodePtr->mValue, destKey, nodePtr->mValLen);
    TEST_EQUAL(comRet, 0);
    TEST_EQUAL(nodePtr->mValLen, 8);
    TEST_EQUAL(nodePtr->mPointer, 1);

    cout << "End" << endl;    
    return true;

}
ADD_CASE(ParseKeyAndValueTest);

extern void DestroyBST(BSTNode *root);

void DFS(BSTNode *root, string &re)
{
    if (root->mLeft != NULL)
        DFS(root->mLeft, re);
    for (size_t i = 0; i < root->mValLen; ++i)
        re += char(root->mValue[i]);
    if (root->mRight != NULL)
        DFS(root->mRight, re);
}

bool BSTInsertNodeTest(void)
{
    BPlusTree bpt;
    BSTNode *root = NULL;
    BSTNode *node1Ptr = new BSTNode;
    node1Ptr->mValue = new unsigned char[3];
    node1Ptr->mValLen = 3;
    memcpy(node1Ptr->mValue, "abc", 3);
    bpt.BSTInsertNode(root, node1Ptr, NULL);

    BSTNode *node2Ptr = new BSTNode;
    node2Ptr->mValue = new unsigned char[2];
    node2Ptr->mValLen = 2;
    memcpy(node2Ptr->mValue, "bc", 2);
    bpt.BSTInsertNode(root, node2Ptr, NULL);

    BSTNode *node3Ptr = new BSTNode;
    node3Ptr->mValue = new unsigned char[3];
    node3Ptr->mValLen = 3;
    memcpy(node3Ptr->mValue, "aaa", 3);
    bpt.BSTInsertNode(root, node3Ptr, NULL);

    BSTNode *node4Ptr = new BSTNode;
    node4Ptr->mValue = new unsigned char[1];
    node4Ptr->mValLen = 1;
    memcpy(node4Ptr->mValue, "A", 1);
    bpt.BSTInsertNode(root, node4Ptr, NULL);

    string re;
    DFS(root, re);
    TEST_EQUAL(re, "Aaaaabcbc");

    DestroyBST(root);

    cout << "End" << endl;    
    
    return true;
}
ADD_CASE(BSTInsertNodeTest);

bool BitComparerTest(void)
{
    BPlusTree tree;

    unsigned char *ptr1 = new (nothrow) unsigned char[4];
    ptr1[0] = '0';
    ptr1[1] = '2';
    ptr1[2] = '3';
    ptr1[3] = '4';

    unsigned char *ptr2 = new (nothrow) unsigned char[4];
    ptr2[0] = '0';
    ptr2[1] = '2';
    ptr2[2] = '3';
    ptr2[3] = '5';

    int ret = tree.BitComparer(ptr1, 4, ptr2, 4);
    TEST_EQUAL(ret, -1);
    
    ptr2[3] = '4';
    ret = tree.BitComparer(ptr1, 4, ptr2, 4);
    TEST_EQUAL(ret, 0);

    ptr1[0] = '1';
    ret = tree.BitComparer(ptr1, 4, ptr2, 4);
    TEST_EQUAL(ret, 1);

    delete[] ptr1;
    delete[] ptr2;

    ptr1 = new (nothrow) unsigned char[6];
    ptr1[0] = '0';
    ptr1[1] = '2';
    ptr1[2] = '3';
    ptr1[3] = '4';
    ptr1[4] = '5';
    ptr1[5] = '6';

    ptr2 = new (nothrow) unsigned char[4];
    ptr2[0] = '0';
    ptr2[1] = '2';
    ptr2[2] = '3';
    ptr2[3] = '5';
    ret = tree.BitComparer(ptr1, 6, ptr2, 4);
    TEST_EQUAL(ret, -1);

    ptr1[3] = '5';
    ret = tree.BitComparer(ptr1, 6, ptr2, 4);
    TEST_EQUAL(ret, 1);

    cout << "End" << endl;    

    return true;
}
ADD_CASE(BitComparerTest);

bool GetNodeTest(void)
{
    BPlusTree tree;
    bool ret;
    ret = tree.mFileHandler.Open("data/GetNodeTest.in");
    TEST_EQUAL(ret, true);

    BTreeNode node;
    ret = tree.GetNode(&node, 1);
    TEST_EQUAL(ret, true);

    string elementStr("");
    DFS(node.mRoot, elementStr);
    TEST_EQUAL(elementStr, "key1key2key3");
    TEST_EQUAL(node.mRoot->mPointer, 111);
    TEST_EQUAL(node.mLeafFlag, true);
    TEST_EQUAL(node.mPointer, 0);

    /// Add destroy node code
    cout << "End" << endl;
    
    return true;
}
ADD_CASE(GetNodeTest);

bool GetBSTKeysTest(void)
{
    BPlusTree tree;

    BSTNode *root = NULL;
    BSTNode *tn1, *tn2, *tn3, *tn4;
    char key1[] = "key2";
    char key2[] = "key1";
    char key3[] = "key4";
    char key4[] = "key3";
    tn1 = new BSTNode;
    tn1->mValue = new uchar[4];
    memcpy(tn1->mValue, key1, 4);
    tn1->mValLen = 4;
    tn2 = new BSTNode;
    tn2->mValue = new uchar[4];
    memcpy(tn2->mValue, key2, 4);
    tn2->mValLen = 4;
    tn3 = new BSTNode;
    tn3->mValue = new uchar[4];
    memcpy(tn3->mValue, key3, 4);
    tn3->mValLen = 4;
    tn4 = new BSTNode;
    tn4->mValue = new uchar[4];
    memcpy(tn4->mValue, key4, 4);
    tn4->mValLen = 4;
    
    vector<BSTNode*> re;
    re.clear();

    tree.GetBSTKeys(root, re);
    TEST_EQUAL(re.size(), 0);

    tree.BSTInsertNode(root, tn1, NULL);
    tree.BSTInsertNode(root, tn2, NULL);
    re.clear();
    tree.GetBSTKeys(root, re);
    {
        string str;
        DFS(root, str);
    }
    
    TEST_EQUAL(re.size(), 2);
    TEST_EQUAL_ARR(re[0]->mValue, ((uchar*)key2), 4);
    TEST_EQUAL_ARR(re[1]->mValue, ((uchar*)key1), 4);

    tree.BSTInsertNode(root, tn3, NULL);
    tree.BSTInsertNode(root, tn4, NULL);
    {
        string str;
        DFS(root, str);
    }
    re.clear();
    tree.GetBSTKeys(root, re);
    TEST_EQUAL(re.size(), 4);
    TEST_EQUAL_ARR(re[0]->mValue, (uchar*)key2, 4);
    TEST_EQUAL_ARR(re[1]->mValue, (uchar*)key1, 4);
    TEST_EQUAL_ARR(re[2]->mValue, (uchar*)key4, 4);
    TEST_EQUAL_ARR(re[3]->mValue, (uchar*)key3, 4);

    DestroyBST(root);

    cout << "End" << endl;    

    return true;
}
ADD_CASE(GetBSTKeysTest);

bool ExpandBitMapTest(void)
{
    BPlusTree tree;
    bool ret = tree.Open("data/GetNodeTest.in");
    TEST_EQUAL(ret, true);
    TEST_EQUAL(tree.mBitMapLen, BITMAP_EXPAND_SIZE);
    TEST_EQUAL(tree.mBitMapPtr[0], true);
    TEST_EQUAL(tree.mBitMapPtr[1], true);
    TEST_EQUAL(tree.mBitMapPtr[2], false);

    ret = tree.ExpandBitMap(1000);
    TEST_EQUAL(ret, true);
    TEST_EQUAL(tree.mBitMapPtr[0], true);
    TEST_EQUAL(tree.mBitMapPtr[1], true);
    TEST_EQUAL(tree.mBitMapPtr[2], false);
    TEST_EQUAL(tree.mBitMapLen, BITMAP_EXPAND_SIZE+1000);

    cout << "End" << endl;    

    return true;
}
ADD_CASE(ExpandBitMapTest);

bool GetNewNodeSlotTest(void)
{
    BPlusTree tree;
    bool ret = tree.Open("data/GetNodeTest.in");
    TEST_EQUAL(ret, true);

    uint64 pos = tree.GetNewNodeSlot();
    TEST_EQUAL(pos, 2);
    for (uint32 i = 2; i < tree.mBitMapLen; ++i)
        tree.UpdateBitMap(i, true);

    pos = tree.GetNewNodeSlot();
    TEST_EQUAL(pos, BITMAP_EXPAND_SIZE);

    cout << "End" << endl;    

    return true;
}
ADD_CASE(GetNewNodeSlotTest);

bool SplitLeafTest(void)
{
    BSTNode node;
    uchar *insertKeyPtr = new uchar[4];
    memcpy(insertKeyPtr, "key5", 4);
    node.mValue = insertKeyPtr;
    node.mValLen = 4;
    node.mPointer = 10000;

    BPlusTree tree;
    bool ret = tree.Open("data/GetNodeTest.in");
    TEST_EQUAL(ret, true);
    TEST_EQUAL(tree.mRootPtr->mIsDirty, false);
    TEST_EQUAL(tree.mBitMapLen, 30000);

    pair<PointerType, PointerType> ptrInfo;
    BSTNode *retPtr = tree.SplitBTreeNode(tree.mRootPtr, &node,
                                          true, ptrInfo);
    TEST_TRUE(retPtr != NULL);
    uchar dest[] = "key2";
    TEST_EQUAL_ARR(retPtr->mValue, (uchar*)dest, 4);
    TEST_EQUAL(retPtr->mValLen, 4);
    /// TEST_EQUAL(retPtr->mPointer, 113);

    TEST_EQUAL(tree.mRootPtr->mIsDirty, true);
    TEST_EQUAL(tree.mBitMapLen, 30000);

    string oldNodeStrRep;
    DFS(tree.mRootPtr->mRoot, oldNodeStrRep);
    TEST_EQUAL(oldNodeStrRep, "key1");

    string newNodeStrRep;
    DFS(tree.mRootPtr->mChildPtr->mRoot, newNodeStrRep);
    TEST_EQUAL(newNodeStrRep, "key2key3key5");

    cout << "End" << endl;    
    
    return true;
}
ADD_CASE(SplitLeafTest);

bool SplitNonLeafTest(void)
{
    BSTNode node;
    uchar *insertKeyPtr = new uchar[4];
    memcpy(insertKeyPtr, "key5", 4);
    node.mValue = insertKeyPtr;
    node.mValLen = 4;
    node.mPointer = 10000;

    BPlusTree tree;
    bool ret = tree.Open("data/GetNodeTest.in");
    TEST_EQUAL(ret, true);
    TEST_EQUAL(tree.mRootPtr->mIsDirty, false);
    TEST_EQUAL(tree.mBitMapLen, 30000);

    pair<PointerType, PointerType> ptrInfo;
    BSTNode *retPtr = tree.SplitBTreeNode(tree.mRootPtr, &node, false,
                                          ptrInfo);
    TEST_TRUE(retPtr != NULL);
    uchar dest[] = "key2";
    TEST_EQUAL_ARR(retPtr->mValue, (uchar*)dest, 4);
    TEST_EQUAL(retPtr->mValLen, 4);
    TEST_EQUAL(retPtr->mRight, NULL);    

    TEST_EQUAL(tree.mRootPtr->mIsDirty, true);
    TEST_EQUAL(tree.mBitMapLen, 30000);

    string oldNodeStrRep;
    DFS(tree.mRootPtr->mRoot, oldNodeStrRep);
    TEST_EQUAL(oldNodeStrRep, "key1");
    TEST_EQUAL(tree.mRootPtr->mPointer, 113);
    TEST_EQUAL(tree.mRootPtr->mRoot->mPointer, 112);

    string newNodeStrRep;
    DFS(tree.mBTreeNodeCache.GetNode(ptrInfo.second)->mRoot, newNodeStrRep);
    TEST_EQUAL(newNodeStrRep, "key3key5");

    TEST_EQUAL(ptrInfo.first, 1);
    TEST_EQUAL(ptrInfo.second, 2);

    cout << "End" << endl;    
    
    return true;
}
ADD_CASE(SplitNonLeafTest);

bool CopyBSTNodeTest(void)
{
    BSTNode *ptr = new BSTNode;
    ptr->mValue = new uchar[5];
    uchar src[] = "billa";
    memcpy(ptr->mValue, (uchar*)src, 5);
    ptr->mValLen = 5;
    ptr->mPointer = 1000;

    BPlusTree t;
    BSTNode *destNodePtr = NULL;
    bool ret = t.CopyBSTNode(destNodePtr, ptr);
    TEST_TRUE(ret);
    TEST_EQUAL(destNodePtr->mValLen, 5);
    TEST_EQUAL(destNodePtr->mPointer, ptr->mPointer);
    TEST_EQUAL_ARR(destNodePtr->mValue, ptr->mValue, ptr->mValLen);

    cout << "End" << endl;    
    
    return true;
}
ADD_CASE(CopyBSTNodeTest);

bool BSTInsertNodeAndGetSthTest(void)
{
    BPlusTree t;
    BSTNode *root = new BSTNode;
    uchar key1[] = "abcdef";
    uchar key2[] = "bcde";
    uchar key3[] = "fffff";
    AssignKey(root, key1, 6);
    BSTNode *node2Ptr;
    t.CopyBSTNode(node2Ptr, root);
    AssignKey(node2Ptr, key2, 4);
    BSTNode *node3Ptr;
    t.CopyBSTNode(node3Ptr, root);
    AssignKey(node3Ptr, key3, 5);

    t.BSTInsertNode(root, node2Ptr, NULL);
    t.BSTInsertNode(root, node3Ptr, NULL);

    /// test insert key at the beginning of BST.
    {
        BSTNode *testPtr;
        t.CopyBSTNode(testPtr, root);
        uchar key[] = "aaa";
        AssignKey(testPtr, key, 3);
        BSTNode *subNodePtr = NULL;
        bool isMin, isMax, flag;
        isMin = isMax = true;
        flag = false;
        t.BSTInsertNodeAndGetSth(root, testPtr, isMin, isMax,
                                 flag, subNodePtr);
        TEST_EQUAL(isMin, true);
        TEST_EQUAL(isMax, false);
        TEST_TRUE(subNodePtr != NULL);
        TEST_EQUAL_ARR(subNodePtr->mValue, (uchar*)key1, 5);
        string dfsStr;
        string dfsDestStr = "aaaabcdefbcdefffff";
        DFS(root, dfsStr);
        TEST_EQUAL(dfsStr, dfsDestStr);
    }

    /// test insert key at the end of BST.
    {
        BSTNode *testPtr;
        t.CopyBSTNode(testPtr, root);
        uchar key[] = "zzzzzzzz";
        AssignKey(testPtr, key, 8);
        BSTNode *subNodePtr = NULL;
        bool isMin, isMax, flag;
        isMin = isMax = true;
        flag = false;
        t.BSTInsertNodeAndGetSth(root, testPtr, isMin, isMax,
                                 flag, subNodePtr);
        TEST_EQUAL(isMin, false);
        TEST_EQUAL(isMax, true);
        TEST_TRUE(subNodePtr == NULL);
        
        string dfsStr;
        string dfsDestStr = "aaaabcdefbcdefffffzzzzzzzz";
        DFS(root, dfsStr);
        TEST_EQUAL(dfsStr, dfsDestStr);
    }

    /// test insert key at the middle of BST.
    {
        BSTNode *testPtr;
        t.CopyBSTNode(testPtr, root);
        uchar key[] = "ccc";
        AssignKey(testPtr, key, 3);
        BSTNode *subNodePtr = NULL;
        bool isMin, isMax, flag;
        isMin = isMax = true;
        flag = false;
        t.BSTInsertNodeAndGetSth(root, testPtr, isMin, isMax,
                                 flag, subNodePtr);
        TEST_EQUAL(isMin, false);
        TEST_EQUAL(isMax, false);
        TEST_TRUE(subNodePtr != NULL);
        TEST_EQUAL_ARR(subNodePtr->mValue, (uchar*)key3, 5);
        
        string dfsStr;
        string dfsDestStr = "aaaabcdefbcdecccfffffzzzzzzzz";
        DFS(root, dfsStr);
        TEST_EQUAL(dfsStr, dfsDestStr);
    }

    DestroyBST(root);

    cout << "End" << endl;    

    return true;
}
ADD_CASE(BSTInsertNodeAndGetSthTest);

bool BTreeNodeInsertSplittedKeyTest(void)
{
    BPlusTree tree;
    BTreeNode *btreeNodePtr = new BTreeNode;
    uchar key1[] = "key1";
    uchar key2[] = "key2";
    uchar key3[] = "key3";
    uchar key4[] = "key4";
    pair<PointerType, PointerType> ptrInfo;
    bool ret;

    ptrInfo.first = 11111;
    ptrInfo.second = 22222;
    BSTNode *node1Ptr = new (nothrow) BSTNode;
    AssignKey(node1Ptr, key2, 4);
    bool mpFlag = false;
    ret = tree.BTreeNodeInsertSplittedKey(btreeNodePtr, node1Ptr, ptrInfo, mpFlag);
    TEST_TRUE(ret);
    string dfsStr;
    DFS(btreeNodePtr->mRoot, dfsStr);
    TEST_EQUAL(dfsStr, "key2");
    TEST_EQUAL(btreeNodePtr->mPointer, 22222);
    TEST_EQUAL(btreeNodePtr->mRoot->mPointer, 11111);

    /// insert node at the front.
    ptrInfo.first = 1000;
    ptrInfo.second = 2000;
    BSTNode *node2Ptr = new (nothrow) BSTNode;
    AssignKey(node2Ptr, key1, 4);
    ret = tree.BTreeNodeInsertSplittedKey(btreeNodePtr, node2Ptr, ptrInfo, mpFlag);
    TEST_TRUE(ret);
    dfsStr = "";
    DFS(btreeNodePtr->mRoot, dfsStr);
    TEST_EQUAL(dfsStr, "key1key2");
    TEST_EQUAL(btreeNodePtr->mPointer, 22222);
    TEST_EQUAL(btreeNodePtr->mRoot->mPointer, 2000);
    
    /// insert node at the end.
    ptrInfo.first = 3000;
    ptrInfo.second = 4000;
    BSTNode *node3Ptr = new (nothrow) BSTNode;
    AssignKey(node3Ptr, key4, 4);
    ret = tree.BTreeNodeInsertSplittedKey(btreeNodePtr, node3Ptr, ptrInfo, mpFlag);
    TEST_TRUE(ret);
    dfsStr = "";
    DFS(btreeNodePtr->mRoot, dfsStr);
    TEST_EQUAL(dfsStr, "key1key2key4");
    TEST_EQUAL(btreeNodePtr->mPointer, 4000);
    TEST_EQUAL(btreeNodePtr->mRoot->mPointer, 2000);
    TEST_EQUAL(btreeNodePtr->mRoot->mRight->mPointer, 3000);

    /// insert node at the middle of BST.
    ptrInfo.first = 3;
    ptrInfo.second = 4;
    BSTNode *node4Ptr = new (nothrow) BSTNode;
    AssignKey(node4Ptr, key3, 4);
    ret = tree.BTreeNodeInsertSplittedKey(btreeNodePtr, node4Ptr, ptrInfo, mpFlag);
    TEST_TRUE(ret);
    dfsStr = "";
    DFS(btreeNodePtr->mRoot, dfsStr);
    TEST_EQUAL(dfsStr, "key1key2key3key4");
    TEST_EQUAL(btreeNodePtr->mPointer, 4000);
    TEST_EQUAL(btreeNodePtr->mRoot->mPointer, 2000);
    TEST_EQUAL(btreeNodePtr->mRoot->mRight->mPointer, 4);
    TEST_EQUAL(btreeNodePtr->mRoot->mRight->mLeft->mPointer, 3);

    DestroyBST(btreeNodePtr->mRoot);

    cout << "End" << endl;

    return true;
}
ADD_CASE(BTreeNodeInsertSplittedKeyTest);

/// TODO
bool RecursiveInsertTest(void)
{
    
    cout << "End" << endl;
    return true;
}
ADD_CASE(RecursiveInsertTest);

bool BPTreeInsertTest(void)
{
    LOG_CALL();

    BPlusTree tree;
    bool ret = tree.Open("data/GetNodeTest.in");
    TEST_EQUAL(ret, true);

    /// for this test, we assume that MAX_ELEMENT_NUMBER is 5.
    /// There are one BTree node when open() is called.
    /// Then we insert 20 different keys, and this would make two new root
    /// generation.
    vector<string> keySet;
    string keyPrefix = "key";
    vector<PointerType> valSet;
    int keyNum = 20;
    SimpleHashTable<> dupChecker;
    for (int i = 0; i < keyNum; ++i) {
        /// generate keyNum different values.
        while (true) {
            int val = Random::NextInt() % 100 + 10;
            int tmp;
            if (dupChecker.CheckKey(val, tmp))
                continue;
            else {
                bool ret = dupChecker.Insert(val, 0);
                if (!ret)
                    cout << "Insert error" << endl;
                stringstream ss;
                ss << keyPrefix << val;
                keySet.push_back(ss.str());
                valSet.push_back(val);
                break;
            }
        }
    }

 
    /// for debug
    for (int i = 0; i < keyNum; ++i) {
        cout << keySet[i] << " " << valSet[i] << endl;
    }

    for (int i = 0; i < keyNum; ++i) {
        bool ret = tree.Insert((const uchar *)keySet[i].c_str(),
                               keySet[i].length(), valSet[i]);
        if (!ret)
            cout << "Insert Error!" << endl;
    }

    /// verification 
    keySet.push_back("key1");
    valSet.push_back(112);
    keySet.push_back("key2");
    valSet.push_back(113);
    keySet.push_back("key3");
    valSet.push_back(111);

    map<string, int> helpMapper;
    for (size_t i = 0; i < keySet.size(); ++i)
        helpMapper[keySet[i]] = i;

    /// Dump BPTree and test result
    vector<Key> keys;
    vector<PointerType> vals;
    tree.DumpTree(keys, vals);
    string allKeys;
    for (size_t i = 0; i < keys.size(); ++i) {
        string k = keys[i].ToString();
        cout << "key: " << k << endl;
        TEST_EQUAL(vals[i], valSet[helpMapper[k]]);
        allKeys += k;
    }
    sort(keySet.begin(), keySet.end());
    string destAllKeys;
    for (size_t i = 0; i < keySet.size(); ++i)
        destAllKeys += keySet[i];
    TEST_EQUAL(allKeys, destAllKeys);

    cout << "End" << endl;
    return true;
}
ADD_CASE(BPTreeInsertTest);

bool WriteBTreeNodeToDiskTest(void)
{
    LOG_DEBUG("WriteBTreeNOdeToDiskTest");
    BPlusTree t;
    bool ret;
    ret = t.Open("data/WriteBTreeNodeToDisk.db");
    TEST_EQUAL(ret, true);

    uchar key1[] = "key1";
    uchar key2[] = "key2";
    uchar key3[] = "key3";
    uchar key4[] = "key4";
    uchar key5[] = "key5";
    uchar key6[] = "key6";
    uchar key7[] = "key7";

    ret = t.Insert(key5, 4, 5);
    TEST_EQUAL(ret, true);
    ret = t.Insert(key6, 4, 6);
    TEST_EQUAL(ret, true);
    ret = t.Insert(key3, 4, 3);
    TEST_EQUAL(ret, true);
    ret = t.Insert(key7, 4, 7);
    TEST_EQUAL(ret, true);
    ret = t.Insert(key1, 4, 1);
    TEST_EQUAL(ret, true);
    ret = t.Insert(key2, 4, 2);
    TEST_EQUAL(ret, true);
    ret = t.Insert(key4, 4, 4);
    TEST_EQUAL(ret, true);

    ret = t.Commit();
    TEST_EQUAL(ret, true);

    BPlusTree t2;
    ret = t2.Open("data/WriteBTreeNodeToDisk.db");
    TEST_EQUAL(ret, true);

    vector<Key> keys;
    vector<PointerType> vals;
    t2.DumpTree(keys, vals);
    string allKeys;
    for (size_t i = 0; i < keys.size(); ++i)
        allKeys += keys[i].ToString();
    string dest = "key1key2key3key4key5key6key7";
    TEST_EQUAL(allKeys, dest);

    remove("data/WriteBTreeNodeToDisk.db");

    cout << "End" << endl;
    return true;
}
ADD_CASE(WriteBTreeNodeToDiskTest);

bool CommitTest(void)
{
    LOG_CALL();

    BPlusTree t;
    bool ret;
    ret = t.Open("data/CommitTest.db");
    TEST_EQUAL(ret, true);

    uchar key1[] = "key1";
    uchar key2[] = "key2";
    t.Insert(key1, 1, 1);
    t.Insert(key2, 2, 2);

    ret = t.Commit();
    TEST_EQUAL(ret, true);

    /// TEST_EQUAL(t.mBTreeNodeCache.GetAllCachedNodes.size(), 0);
    /// TEST_EQUAL(t.mBTreeNodeCache.GetToFlushNodes.size(), 0);

    return true;
}
ADD_CASE(CommitTest);

bool CopyBSTTest(void)
{
    BPlusTree bpt;
    BSTNode *root = NULL;
    BSTNode *node1Ptr = new BSTNode;
    node1Ptr->mValue = new unsigned char[3];
    node1Ptr->mValLen = 3;
    memcpy(node1Ptr->mValue, "abc", 3);
    bpt.BSTInsertNode(root, node1Ptr, NULL);

    BSTNode *node2Ptr = new BSTNode;
    node2Ptr->mValue = new unsigned char[2];
    node2Ptr->mValLen = 2;
    memcpy(node2Ptr->mValue, "bc", 2);
    bpt.BSTInsertNode(root, node2Ptr, NULL);

    BSTNode *node3Ptr = new BSTNode;
    node3Ptr->mValue = new unsigned char[3];
    node3Ptr->mValLen = 3;
    memcpy(node3Ptr->mValue, "aaa", 3);
    bpt.BSTInsertNode(root, node3Ptr, NULL);

    BSTNode *node4Ptr = new BSTNode;
    node4Ptr->mValue = new unsigned char[1];
    node4Ptr->mValLen = 1;
    memcpy(node4Ptr->mValue, "A", 1);
    bpt.BSTInsertNode(root, node4Ptr, NULL);

    BSTNode *copiedBSTptr = NULL;
    bool ret = bpt.CopyBST(copiedBSTptr, root);
    DestroyBST(root);
    TEST_EQUAL(ret, true);
    string re;
    DFS(copiedBSTptr, re);
    TEST_EQUAL(re, "Aaaaabcbc");
    DestroyBST(copiedBSTptr);

    return true;
}
ADD_CASE(CopyBSTTest);

/// TODO
bool InsertRollbackTest(void)
{
    BPlusTree t;

    BTreeNode *bn1Ptr = new BTreeNode;
    bn1Ptr->mOffsetID = 1;
    t.mBTreeNodeCache.AddNode(bn1Ptr, 1);

    BTreeNode *bn2Ptr = new BTreeNode;
    bn2Ptr->mOffsetID = 2;
    t.mModifiedBTreeNodeSet.AddNode(bn2Ptr, 2);
    BTreeNode *bn2NewPtr = new BTreeNode;
    bn2NewPtr->mOffsetID = 2;
    t.mBTreeNodeCache.AddNode(bn2NewPtr, 2);

    t.mNewBTreeNodeSet.insert(1);

    bool ret = t.InsertRollback();
    TEST_EQUAL(ret, true);
    vector<PointerType> idInCache = t.mBTreeNodeCache.GetAllCachedNodes();
    TEST_EQUAL(idInCache.size(), 1);
    TEST_EQUAL(idInCache[0], 2);
    
    return true;
}
ADD_CASE(InsertRollbackTest);

bool RemoveRollbackTest(void)
{
    BPlusTree t;
    
    BTreeNode *bn1Ptr = new BTreeNode;
    bn1Ptr->mOffsetID = 1;
    t.mRemovedBTreeNodeSet.AddNode(bn1Ptr, 1);

    t.RemoveRollback();

    vector<PointerType> idSet = t.mBTreeNodeCache.GetAllCachedNodes();
    TEST_EQUAL(idSet.size(), 1);
    TEST_EQUAL(idSet[0], 1);

    return true;
}
ADD_CASE(RemoveRollbackTest);

REGISTER_UT(BPlusTreeTest);
