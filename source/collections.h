// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#ifndef _COLLECTIONS_H_
#define _COLLECTIONS_H_

#include "types.h"

#include "stdlib.h"
#include <assert.h>
#include <new.h>
#include <utility>

template<typename t>
class StaticArray
{
    unsigned int length;
    t* data;
public:
    StaticArray() : length(0), data(NULL) {}

    StaticArray(unsigned int len) : length(len)
    {
        data = new t[length];
    }

    ~StaticArray()
    {
        clear();
    }

    void reinit(unsigned int len)
    {
        length = len;
        delete[] data;
        data = new t[length];
    }

    void clear()
    {
        length = 0;
        delete[] data;
        data = NULL;
    }

    // We should consider if we want a resize, which would keep existing
    // contents intact, It would then not be safe to hold an internal pointer.

    t& operator[](unsigned int index)
    {
        assert(index < length);
        return data[index];
    }

    const t& operator[](unsigned int index) const
    {
        (*this)[index];
    }

    unsigned int size() const
    {
        return length;
    }
};

/*template <typename T>
void Reflect_Type<StaticArray<T>>(Reflect& reflect)
{
    reflect(&length, "Length").unpersist(
        [] (void* ar, void* len) {
            ((StaticArray*)ar)->reinit(*(unsigned int*)len);
        });

    /*reflect(&data, "Contents").array().unpersist(
        [] (void* ar, T val, unsigned int index) {
            *((StaticArray*)ar)[index] = val;
            });*/
//}*/

template<typename T>
struct ListNode;

template<typename T>
struct ListBase
{
    typedef ListNode<T> NodeT;

    NodeT* first;
    NodeT* last;

    ListBase()
    {
        first = nullptr;
        last = nullptr;
    }

    ~ListBase();
};

template<typename T>
struct ListNode
{
    typedef ListNode<T> NodeT;

    NodeT* next;
    NodeT* prev;
    ListBase<T>* parent;

    ListNode()
    {
        next = nullptr;
        prev = nullptr;
        parent = nullptr;
    }

    void Remove()
    {
        if(prev)
            prev->next = next;
        else if(parent)
            parent->first = next;

        if(next)
            next->prev = prev;
        else if(parent)
            parent->last = prev;

        next = nullptr;
        prev = nullptr;
        parent = nullptr;
    }

    void Insert(NodeT* after)
    {
        Remove();
        assert(after->parent && "Insert node does not have a parent");

        parent = after->parent;

        if(after->next)
            after->next->prev = this;
        else
            parent->last = this;

        next = after->next;

        after->next = this;
    }

    void Insert(NodeT& after)
    {
        Insert(&after);
    }

    void Append(ListBase<T>* list)
    {
        Remove();
        if(list->last)
        {
            assert(list->first && "malformed list.");
            prev = list->last;
            prev->next = this;
            list->last = this;
        }
        else
        {
            assert(!list->first && "malformed list.");
            list->first = this;
            list->last = this;
        }
        parent = list;
    }

    void Append(ListBase<T>& list)
    {
        Append(&list);
    }

    template<ListNode<T> T::* elem>
    T* Get()
    {
        return (T*)((char*)this - (char*)&(((T*)0)->*elem));
    }

    ~ListNode()
    {
        Remove();
    }
};

template<typename T, ListNode<T> T::* elem>
struct ListIter
{
    typedef ListIter<T, elem> IterT;
    typedef ListNode<T> NodeT;

    NodeT* node;

    ListIter(NodeT* n)
    {
        node = n;
    }

    IterT Next()
    {
        return IterT(node->next);
    }

    IterT Prev()
    {
        return IterT(node->prev);
    }

    T* Get()
    {
        return node->Get<elem>();
    }

    operator ListNode<T>*()
    {
        return node;
    }

    operator bool() const
    {
        return node != nullptr;
    }

    bool operator==(const IterT& rhs) const
    {
        return node == rhs.node;
    }

    bool operator==(const NodeT* rhs) const
    {
        return node == rhs;
    }
};

template<typename T, ListNode<T> T::* elem>
struct List : ListBase<T>
{
    typedef ListIter<T, elem> IterT;

    IterT First()
    {
        return IterT(first);
    }

    IterT Last()
    {
        return IterT(last);
    }

    void Add(T* node)
    {
        node->*elem.Append(this);
    }
};

template<typename T>
ListBase<T>::~ListBase()
{
    while(first)
    {
        first->Remove();
    }
}

template <class type, uint32 segmentLen>
class SegmentList
{
    struct Segment
    {
        //union{ type elems[segmentLen] }; // Needs unrestricted union support on msvc...
        char buffer[segmentLen * sizeof(type)];
        Segment* next;

        Segment() {}
        ~Segment() {}
    };

    Segment* first;
    uint32 segmentCount;

public:

    struct iterator
    {
        Segment* seg;
        int32 idx;

        iterator() : seg(nullptr), idx(segmentLen - 1)
        {}

        iterator(iterator const & rhs) : seg(rhs.seg), idx(rhs.idx)
        {}

        iterator(SegmentList* lst) : seg(lst->first), idx(lst->segmentCount - 1)
        {}

        iterator& operator++()
        {
            idx--;
            if(idx < 0 && seg)
            {
                seg = seg->next;
                idx = segmentLen - 1;
            }
            return *this;
        }

        iterator operator++(int)
        {
            iterator copy(*this);
            ++(*this);
            return copy;
        }

        bool operator==(iterator const& rhs) const
        {
            return seg == rhs.seg && idx == rhs.idx;
        }

        bool operator!=(iterator const& rhs) const
        {
            return seg != rhs.seg || idx != rhs.idx;
        }

        type& operator*() const
        {
            return reinterpret_cast<type&>(seg->buffer[idx*sizeof(type)]);
        }
    };

    iterator begin()
    {
        return iterator(this);
    }

    iterator end()
    {
        return iterator();
    }

    SegmentList()
    {
        first = nullptr;
        segmentCount = segmentLen;
    }

    ~SegmentList()
    {
        clear();
    }

    type& append()
    {
        if(segmentCount == segmentLen)
        {
            Segment* s = new Segment;
            s->next = first;

            first = s;
            segmentCount = 0;
        }

        type& t = reinterpret_cast<type&>(first->buffer[(segmentCount++) * sizeof(type)]);

        new(&t) type();

        return t;
    }

    iterator erase(iterator position)
    {
        iterator next = position;
        next++;

        (*position).~type();

        iterator first = begin();
        if(position != first)
            (*position) = std::move(*first);

        segmentCount--;

        if(segmentCount == 0)
        {
            Segment* old = this->first;
            if(old)
                this->first = old->next;
            else
                this->first = nullptr;

            segmentCount = segmentLen;
        }

        return next;
    }

    void clear()
    {
        iterator it = begin();

        while(it != end())
        {
            it = erase(it);
        }
    }
};

#endif //_COLLECTIONS_H_
