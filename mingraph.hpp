#ifndef _MINGRAPH_HPP_
#define _MINGRAPH_HPP_

#include <vector>
#include <queue>
#include <algorithm>

typedef unsigned int VertexID;
typedef std::vector<VertexID> VertexIDSet;

template <class T> class MinGraph;

template <class T>
class Vertex {
    T value;
    VertexIDSet neighbors;
    
    friend class MinGraph<T>;
};

template <class T>
class MinGraph {

    public:
    MinGraph(void);
    MinGraph(int n);
    inline int V(void) { return VS.size(); }
    
    void AddEdge(VertexID u, VertexID v);
    bool HasPath(VertexID u, VertexID v, bool restrictToSimilar=false);    
    void Neighbors(VertexID v, VertexIDSet &vs, bool restrictToSimilar=false);
    T    SetVertexValue(VertexID v, T value);
    T    GetVertexValue(VertexID v);
    
    void Reset(int n);  
    void Reset(int n, T value);
    
    private:
    std::vector<Vertex<T> > VS;
};

typedef enum enumMinGraphError {
    MINGRAPH_ERR_INVALIDSIZE = 0x100,
    MINGRAPH_ERR_INVALIDEDGE,
    MINGRAPH_ERR_INVALIDVERTEX
} MinGraphError;


/* ============================================================================
   class MinGraph
   
   Implements an undirected graph ADT with a minimal set of functionality
   ============================================================================ */
   
/*  ---------------------------------------------------------------------------
    constructors:
    MinGraph::MinGraph(void);   -- creates an empty graph (0 edges, 0 vertices)
    MinGraph::MinGraph(int n)   -- creates a disconnected graph of n vertices
    --------------------------------------------------------------------------- */
template <class T>
MinGraph<T>::MinGraph(void)
{}

template <class T>    
MinGraph<T>::MinGraph(int n)
{   Reset(n);   }

/* ----------------------------------------------------------------------------
   void AddEdge(VertexID u, VertexID v);
   
   Creates an edge between vertices u, v.
   ---------------------------------------------------------------------------- */
template <class T>
void MinGraph<T>::AddEdge(VertexID u, VertexID v)
{
    int size = V();
    
    if ((u >= size) || (v >= size))
        throw MINGRAPH_ERR_INVALIDVERTEX;       // invalid vertex id
        
    if (u == v) 
        throw MINGRAPH_ERR_INVALIDEDGE;         // self loops disallowed
        
    // add each vertex to the other's adjacency list
    VS[u].neighbors.push_back(v);
    VS[v].neighbors.push_back(u);
}

/* ----------------------------------------------------------------------------
   bool HasPath(VertexID u, VertexID v, bool restrictToSimilar=false);
   
   Determines whether a path exists between vertices u, v.
   Returns true if a path exists, false otherwise
   
   If the optional third parameter restrictToSimilar is set to true, then a path 
   will be considered only through vertices that have same value as the source 
   vertex.
   ---------------------------------------------------------------------------- */
template <class T>
bool MinGraph<T>::HasPath(VertexID u, VertexID v, bool restrictToSimilar)
{    
    int size = V();
    
    if ((u < 0) || (u >= size) || (v < 0) || (v >= size) || (u == v))
        throw MINGRAPH_ERR_INVALIDVERTEX;

    std::vector<bool> seen(size, false);
    std::queue<VertexID> Q;
    
    T sourceValue = VS[u].value;
    
    // initialize the queue with the source vertex u;
    Q.push(u);
    
    while (!Q.empty())
    {        
        VertexID x = Q.front();         // retrieve a vertex to examine
        Q.pop();                        // and remove it from the queue
            
        seen[x] = true;                 // mark vertex as seen            
        
        // examine x's neighbors
        for (int i = 0; i < VS[x].neighbors.size(); i++)
        {
            VertexID idNeighbor = VS[x].neighbors[i];
            
            // if we are restricting path to vertices of same value as
            // source vertex and this neighbor is not of the same value, skip
            if (restrictToSimilar && (VS[idNeighbor].value != sourceValue))
                continue;

            // done if this is the vertex we're looking for
            if (idNeighbor == v) return true;
            
            // skip this neighbor if we have already seen it
            if (seen[idNeighbor]) continue;
            
            Q.push(idNeighbor);         // add neighbor to queue for later processing
        }    
    }
    
    // if we didn't find the destination vertex while we were traversing the graph
    // there is no path between u and v
    return false;
}

/* ----------------------------------------------------------------------------
   void SetVertexValue(VertexID v, T value);
   
   Set the value of the given vertex.  Returns the previous value.
   ---------------------------------------------------------------------------- */
template <class T>
T MinGraph<T>::SetVertexValue(VertexID v, T value)
{
    if (v >= V()) throw MINGRAPH_ERR_INVALIDVERTEX;
    std::swap(VS[v].value, value);
    return value;
}

/* ----------------------------------------------------------------------------
   T GetVertexValue(VertexID v);
   
   Returns the value of given vertex
   ---------------------------------------------------------------------------- */
template <class T>
T MinGraph<T>::GetVertexValue(VertexID v)
{
    if (v >= V()) throw MINGRAPH_ERR_INVALIDVERTEX;
    return VS[v].value;
}

/* ----------------------------------------------------------------------------
    void Neighbors(VertexID v, VertexIDSet &vs, bool restrictoToSimilar=false);
    
    returns the vertices adjacent to the given vertex.
    
    if the thirs optional parameter restrictToSimilar is set to true, only those
    neighbors with value same as that of the given vertex will be considered.
    
    client must provide a reference to a VertexIDSet where the resulting set
    will be returned
   ---------------------------------------------------------------------------- */
template <class T>
void MinGraph<T>::Neighbors(VertexID v, VertexIDSet &vs, bool restrictToSimilar)
{
    if ((v < 0) || (v >= V()))
        throw MINGRAPH_ERR_INVALIDVERTEX;
        
    T sourceValue = VS[v].value;
        
    vs.clear();                                 // clear client VertexIDSet
    int nNeighbors = VS[v].neighbors.size();    // get number of neighbors
    vs.reserve(nNeighbors);                     // create space for neighbor ids
    
    // copy the neighbor IDs to client VertexIDSet
    for (unsigned int i = 0; i < nNeighbors; i++)
    {   
        unsigned int idNeighbor = VS[v].neighbors[i];
        
        // if restrictToSimilar is set to true and this neighbor's value
        // is not the same as the source vertex value, ignore
        if (restrictToSimilar && (VS[idNeighbor].value != sourceValue))
            continue;
        
        vs.push_back(idNeighbor);
    }        
}   

/* ----------------------------------------------------------------------------
   void MinGraph::Reset(int n);
   
   Clears all internal states and creates a graph of n vertices and 0 edges.
   ---------------------------------------------------------------------------- */
template <class T>
void MinGraph<T>::Reset(int n)
{
    if (n < 0) throw MINGRAPH_ERR_INVALIDSIZE;

    VS.clear();
    VS.resize(n);
}
template <class T>
void MinGraph<T>::Reset(int n, T value)
{
    if (n < 0) throw MINGRAPH_ERR_INVALIDSIZE;
    
    VS.clear();
    VS.resize(n);
    
    for (int i = 0; i < VS.size(); i++)
        VS[i].value = value;
}

#endif
