//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#if !defined(Graph_H)
#define Graph_H

#include <vector>
#include <set>

#include "tinyxml/tinyxml.h" // to read XML files

// Desperate mode on: using boost::graph
#include <boost/config.hpp>
#include <vector>
#include <list>
#include <boost/graph/biconnected_components.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <iterator>


// Really simple Graph implementation using 
// adjacency matrix AND adjacency list (set).
// TODO: a real decent graph implementation

class Graph
{
    private:
	unsigned nVertices_;
	unsigned nEdges_;
	std::vector<unsigned> weights_;

        // some private data used on searchs
        std::vector<unsigned> visited_;
        std::vector<unsigned> discover_;
        std::vector<int> pred_; // to allow -1
        std::vector<unsigned> low_;
        unsigned time_;
        
        void initializeDFS();
        void dfs();
        void dfsVisit(unsigned v);
        
        std::vector<bool> articulationPoint_;
        
    public:
        // some public members
        // TODO: make methods to access these members
	std::vector<std::vector<bool> > adjMatrix_;
	std::vector<std::set<unsigned> > adjs_;

        struct edge { unsigned u, v; };
        mutable std::vector<edge> edges_;
        mutable std::vector<std::vector<short> > edgeMark_;
    
        Graph(unsigned nVertices);
        Graph(const Graph& g);
        
        unsigned nVertices() const;
        unsigned nEdges() const;
        
        void addConnection(unsigned u, unsigned v);
        void removeConnection(unsigned u, unsigned v);
        bool isConnected(unsigned u, unsigned v) const;
        
        void setVertexWeight(unsigned v, unsigned weight);
        unsigned getVertexWeight(unsigned v) const;
        
        unsigned getVertexDegree(unsigned v) const;
        
        Graph getRandomSpanningTree() const;
        
        void updatedEdges() const;
        
        bool isConnected() const;
        
        unsigned totalWeight() const;
        
        unsigned heaviest(unsigned ith) const;
        
        void computeArticulationPoints();
        
        bool isArticulationPoint(unsigned v) const;
        
        bool isBiconnected();
        
        unsigned getEdgeMark(unsigned u, unsigned v) const;
        void setEdgeMark(unsigned u, unsigned v, unsigned mark) const;
        
        unsigned weightOfCompoenent(unsigned startVertex, short forbiddenEdges, std::set<unsigned>& visited) const;
        
        Graph getSubgraph(const std::vector<unsigned>& vertices) const;
        
        Graph getSubgraph(const std::set<unsigned>& vertices, std::vector<unsigned>& vertexMapping) const;
};

Graph readGraphFromXML(const char * filename, int& clusters);

#endif // Graph_H
