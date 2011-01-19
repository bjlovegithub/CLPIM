#include "TreeNodeCacheManager.h"
#include "ut_tools/ut_tool.h"

#include <algorithm>

using namespace std;
using namespace CLPIM;
using namespace XiaoDB;

bool CheckNodeExistanceTest(void)
{
    TreeNodeCacheManager c;
    bool re = c.CheckNodeExistance(0);

    TEST_EQUAL(re, false);

    BTreeNode node;
    c.AddNode(&node, 1);
    re = c.CheckNodeExistance(0);
    TEST_EQUAL(re, false);
    re = c.CheckNodeExistance(1);
    TEST_EQUAL(re, true);

    return true;
}
ADD_CASE(CheckNodeExistanceTest);

bool AddNodeTest(void)
{
    BTreeNode n1, n2, n3;
    TreeNodeCacheManager c;
    c.AddNode(&n1, 1);
    c.AddNode(&n2, 2);
    c.AddNode(&n3, 3);

    bool re = c.GetNode(0);
    TEST_EQUAL(re, false);
    re = c.GetNode(1);
    TEST_EQUAL(re, true);
    re = c.GetNode(2);
    TEST_EQUAL(re, true);
    re = c.GetNode(3);
    TEST_EQUAL(re, true);
    re = c.GetNode(4);
    TEST_EQUAL(re, false);

    return true;
}
ADD_CASE(AddNodeTest);

bool RemoveNodeTest(void)
{
    TreeNodeCacheManager c;
    BTreeNode n1, n2;

    c.AddNode(&n1, 1);
    c.AddNode(&n2, 111);
    c.RemoveNode(3);
    bool re = c.GetNode(1);
    TEST_EQUAL(re, true);
    re = c.GetNode(111);
    TEST_EQUAL(re, true);
    re = c.GetNode(3);
    TEST_EQUAL(re, false);

    c.RemoveNode(111);
    re = c.GetNode(1);
    TEST_EQUAL(re, true);
    re = c.GetNode(111);
    TEST_EQUAL(re, false);

    return true;
}
ADD_CASE(RemoveNodeTest);


bool GetNodeTest(void)
{
    TreeNodeCacheManager c;
    BTreeNode *re;
    re = c.GetNode(1);
    TEST_EQUAL(re, NULL);

    BTreeNode n1, n2;
    bool boolRet = c.AddNode(&n1, 1);
    TEST_EQUAL(boolRet, true);
    boolRet = c.AddNode(&n2, 111);
    TEST_EQUAL(boolRet, true);

    re = c.GetNode(1);
    TEST_EQUAL(re, &n1);
    re = c.GetNode(111);
    TEST_EQUAL(re, &n2);

    return true;
}
ADD_CASE(GetNodeTest);

bool GetToFlushNodesTest(void)
{
    TreeNodeCacheManager c;
    BTreeNode n1, n2, n3, n4, n5, n6, n7;
    c.AddNode(&n1, 1);
    c.AddNode(&n2, 2);
    c.AddNode(&n3, 3);
    c.AddNode(&n4, 4);
    c.AddNode(&n5, 5);
    c.AddNode(&n6, 6);
    c.AddNode(&n7, 7);

    vector<PointerType> ret = c.GetToFlushNodes();
    sort(ret.begin(), ret.end());

    TEST_EQUAL(ret.size(), 2);
    TEST_EQUAL(ret[0], 1);
    TEST_EQUAL(ret[1], 2);    
            
    return true;
}
ADD_CASE(GetToFlushNodesTest);

bool GetAllCachedNodesTest(void)
{
    TreeNodeCacheManager c;
    BTreeNode n1, n2, n3, n4, n5, n6, n7;
    c.AddNode(&n1, 1);
    c.AddNode(&n2, 2);
    c.AddNode(&n3, 3);
    c.AddNode(&n4, 4);
    c.AddNode(&n5, 5);
    c.AddNode(&n6, 6);
    c.AddNode(&n7, 7);

    vector<PointerType> ret = c.GetAllCachedNodes();
    sort(ret.begin(), ret.end());

    TEST_EQUAL(ret.size(), 7);
    for (size_t i = 0; i < ret.size(); ++i)
        TEST_EQUAL(ret[i], i+1);
            
    return true;
}
ADD_CASE(GetAllCachedNodesTest);


REGISTER_UT(TreeNodeCacheManagerUT);
