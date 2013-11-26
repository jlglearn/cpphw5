#include "unionfind.h"

UnionFind::UnionFind() {}

UnionFind::UnionFind(unsigned int size)
{   Reset(size);    }

void UnionFind::Reset(unsigned int size)
{
    items.clear();              // clear any previous information
    items.resize(size);         // make room
    
    if (items.size() != size)
        throw UNIONFIND_ERR_OUTOFMEMORY;
    
    for (int i = 0; i < items.size(); i++)
    {
        items[i].parent = i;    // initialize each element's parent to point to itself
        items[i].size = 1;      // initialize each element's size to 1 (itself)
    }
}

// join i to j
void UnionFind::Join(unsigned int i, unsigned int j)
{
    if ((i >= items.size()) || (j >= items.size()))
        throw UNIONFIND_ERR_INDEXOUTOFRANGE;
        
    unsigned int pi = Find(i);  // retrieve root of i's component
    unsigned int pj = Find(j);  // retrieve root of j's component
    
    // do nothing if already joined
    if (pi != pj)
    {        
        // make smaller component point to root of larger component
        if (items[pi].size <= items[pj].size)
        {
            items[pi].parent = pj;
            items[pj].size += items[pi].size;
        }
        else
        {
            items[pj].parent = pi;
            items[pi].size += items[pj].size;
        }
    }
}


// retrieve root of given element
unsigned int UnionFind::Find(unsigned int i)
{
    if (i >= items.size())
        throw UNIONFIND_ERR_INDEXOUTOFRANGE;
        
    unsigned int q = i;
    
    // iterate until we find the root of i's component, compacting path along the way
    while (items[q].parent != q)
    {
        unsigned int p = items[q].parent;   // p points to q's parent
        items[q].parent = items[p].parent;  // make q point to p's parent (eliminating intermediate hop through p)
        q = p;                              // move pointer upward
    }
    
    return q;
}

// return size of given element's component
unsigned int UnionFind::Size(unsigned int i)
{
    if (i >= items.size())
        throw UNIONFIND_ERR_INDEXOUTOFRANGE;
        
    unsigned int pi = Find(i);              // find root of i's component
    return items[pi].size;                  // and return component's size
}
