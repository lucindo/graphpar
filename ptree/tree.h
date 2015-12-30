//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#if !defined(tree_H)
#define tree_H

#include <vector>
#include <cassert>

#include <graphml.h>

// Declaration of class and functions to deal with Trees.
// We need fast implementation to handle cuts on edges
// of trees and calculate components weights
// I'm thinking in use BGL (http://www.boost.org/libs/graph/doc/)
// in the future. For now, all code is independent of
// extern libraries.


// All method of TreeEdge and TreeVertex are inline

struct TreeEdge
{
    unsigned from;
    unsigned to;
    
    TreeEdge(unsigned u, unsigned v) : from(u), to(v), nCuts_(0)
    {
    }
    
    void addCut() const
    {
        this->nCuts_++;
    }
    
    void removeCut() const
    {
        assert(this->nCuts_ > 0); // this can avoid many mistakes
        this->nCuts_--;
    }
    
    bool hasCut() const
    {
        return this->nCuts_ > 0;
    }
    
    void cleanCuts() const
    {
        this->nCuts_ = 0;
    }
    
    private:
        mutable unsigned nCuts_;
        // all access to this class will be done by const objects
};

struct TreeVertex
{
    unsigned weight;
    unsigned self;
    
    std::vector<TreeEdge> edges;
    
    TreeVertex() : weight(0), self(0)
    {
    }
        
    void cleanEdgeCuts() const
    {
        for (unsigned e = 0; e < edges.size(); ++e)
        {
            this->edges[e].cleanCuts();
        }
    }
};

// Forward declaration
struct PartitionComponent;

// The class Tree is far away to be an generic Tree class.
// It is specialized to better implementation of Perl & Schach
// max-min q-partition algorithm. It capabilities:
//  - can handle more than one cut on the same edge
//  - uses only directed edges
//    (all nodes are built to-down, away from the root)
//  - connected tree
class Tree
{
    private:
        std::vector<TreeVertex> vetex_;
        unsigned nEdges_;

        void doAddConnection(unsigned from, unsigned to);
        
        bool isTree() const;

        void writeEdges(GraphmlWriter& writer, bool writeLabels = false) const;
        
    public:
        explicit Tree(unsigned nVertices);
        
        ~Tree();
        
        unsigned getRootVertex() const;
        
        void setVertexWeight(unsigned vertex, unsigned weight);
        unsigned getVertexWeight(unsigned vertex) const;
        
        const std::vector<TreeEdge>& getEdgesOfVertex(unsigned vertex) const;
        
        void addConnection(unsigned from, unsigned to);
        void cleanCuts();
        
        unsigned getVertexCount() const;
        unsigned getEdgeCount() const;
        
        void writeToFile(const char * fileName) const;
        void writeToFile(const char * fileName, std::vector<PartitionComponent>& components) const;
        void dump() const;
        void sanityTest() const;
};

#endif // tree_H
