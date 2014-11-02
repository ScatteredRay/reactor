// Copyright (c) 2014, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

#include "collections.h"
#include <assert.h>

struct NodeObj
{
    ListNode<NodeObj> list_node;
    int num;
    NodeObj(int n) : num(n)
    {}
};

struct ListObj
{
    List<NodeObj, &NodeObj::list_node> list;

	ListObj()
	{
	}
};

void TestCollections()
{
    ListObj obj;
 
    assert(obj.list.First() == nullptr);
    assert(!obj.list.First());

    assert(obj.list.Last() == nullptr);
    assert(!obj.list.Last());

    (new NodeObj(1))->list_node.Append(obj.list);
    (new NodeObj(2))->list_node.Append(obj.list);
    (new NodeObj(3))->list_node.Append(obj.list);

    auto node = obj.list.First();

    assert(node && node.Get()->num == 1);
    node = node.Next();

    assert(node && node.Get()->num == 2);
	auto mid = node;
    node = node.Next();

    assert(node && node.Get()->num == 3);
    assert(node == obj.list.Last());
	auto last = node;
    node = node.Next();

    assert(node == nullptr);
    assert(!node);

	(new NodeObj(4))->list_node.Insert(mid);
	(new NodeObj(5))->list_node.Insert(mid);
	(new NodeObj(6))->list_node.Insert(last);

	node = obj.list.First();

    assert(node && node.Get()->num == 1);
    node = node.Next();

	assert(node && node.Get()->num == 2);
    node = node.Next();

	assert(node && node.Get()->num == 5);
    node = node.Next();

	assert(node && node.Get()->num == 4);
    node = node.Next();

	assert(node && node.Get()->num == 3);
    node = node.Next();

	assert(node && node.Get()->num == 6);
	assert(node == obj.list.Last());
    node = node.Next();

	assert(node == nullptr);
    assert(!node);

	while(node = obj.list.First())
		delete node;

	assert(obj.list.First() == nullptr);
	assert(obj.list.Last() == nullptr);


    SegmentList<int, 64> segmentList;

    segmentList.append() = 1;
    segmentList.append() = 2;
    segmentList.append() = 3;
    segmentList.append() = 4;

    auto it = segmentList.begin();
    assert(it != segmentList.end());
    assert(*(it++) == 4);
    assert(*(it++) == 3);
    assert(*(it++) == 2);
    assert(*(it++) == 1);
    assert(it == segmentList.end());
}
