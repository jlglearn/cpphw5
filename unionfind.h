#ifndef _UNIONFIND_H_
#define _UNIONFIND_H_

#include <vector>

typedef enum enumUnionFindError {
    UNIONFIND_ERR_INDEXOUTOFRANGE,
    UNIONFIND_ERR_INVALIDSIZE,
    UNIONFIND_ERR_OUTOFMEMORY
    } UnionFindError;
    
typedef struct structUFNode {
    unsigned int parent;
    unsigned int size;
} UFNode;


class UnionFind {
    public:
    UnionFind(void);
    UnionFind(unsigned int size);
    void Reset(unsigned int size);
    void Join(unsigned int i, unsigned int j);  // join elements i and j
    unsigned int Find(unsigned int i);          // retrieve root of element i
    unsigned int Size(unsigned int i);          // return size of element i's component
    
    
    private:
    std::vector<UFNode> items;
};
#endif
