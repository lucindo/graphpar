//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#include "tree.h"
#include "partition.h"

#include <log.h>
#include <graphml.h>

#include <cassert>
#include <queue>
#include <set>
#include <fstream>
#include <cstdio>

// class Tree implementation.
// Some ugly stuff can be found here due to achieve a more optimied code :(

// This is the only constructor. We need to know how
// many vertices the tree has to allocate all necessary
// memory.
// The vertices are from 0 to nVertices - 1
Tree::Tree(unsigned nVertices) : vetex_(nVertices), nEdges_(0)
{
    log(LG_DEBUG, "constructing tree with %u vertices", nVertices);
    for(unsigned v = 0; v < this->vetex_.size(); ++v)
    {
        // setting the vertex id
        this->vetex_[v].self = v;
    }
    log(LG_DEBUG, "tree with %u vertices constructed", nVertices);
}

// Only for logging
Tree::~ Tree()
{
    log(LG_DEBUG, "destroying tree (%u vertices, %u edges)", this->vetex_.size(), this->nEdges_);
}

//
// Private Methods
//

// Internal method to verify if some cycle is present
bool Tree::isTree() const
{
    // Perform a Breath-First Search to find children with more than one father
    unsigned nVertices = this->vetex_.size();
    bool hasCycle = false;
    bool visited[nVertices];
    
    // Inicialization
    for (unsigned mark = 0; mark < nVertices; ++mark)
    {
        visited[mark] = false;
    }
    
    // This method can be called with a disconnected tree
    // so we need to start the search on all possivel subtrees
    for (unsigned v = 0; v < nVertices; ++v)
    {
        if (not visited[v])
        {
            // start BFS on vertex v
            std::queue<unsigned> queue;
            
            // mark and enqueue the vertex
            visited[v] = true;
            queue.push(v);
            
            while(not queue.empty())
            {
                unsigned vertex = queue.front();
                queue.pop();
                
                const std::vector<TreeEdge>& edges = this->vetex_[vertex].edges;
                for (unsigned edge = 0; edge < edges.size(); ++edge)
                {
                    if (true == visited[edges[edge].to])
                    {
                        // we already reach this node thru some other father
                        // this indicates that unfortunally a cycle is formed
                        hasCycle = true;
                        break; // we can stop searching
                    }
                    else
                    {
                        // mark and enqueue the vertex
                        visited[edges[edge].to] = true;
                        queue.push(edges[edge].to);
                    }
                }
            }
        }
    }
    
    return hasCycle == false;
}

// Private method that adds the TreeEdge to 'from' vertex.
// It performs no checks, call with caution
void Tree::doAddConnection(unsigned from, unsigned to)
{
    TreeVertex& fromVertex = this->vetex_[from];
    TreeEdge edge(from, to);
    
    fromVertex.edges.push_back(edge);
    this->nEdges_++;
}

//
// Public Methos
//

unsigned Tree::getRootVertex() const
{
    return 0; // The root is always the first indexed vertex
}

// Returns how many vertices are on tree
unsigned Tree::getVertexCount() const
{
    return this->vetex_.size();
}

// Returns how many edges are on tree
unsigned Tree::getEdgeCount() const
{
    return this->nEdges_;
}

// Clean all cuts on vertices
void Tree::cleanCuts()
{
    for (unsigned v = 0; v < this->vetex_.size(); v++)
    {
        this->vetex_[v].cleanEdgeCuts();
    }
}

// Sets the weight of vertex
void Tree::setVertexWeight(unsigned vertex, unsigned weight)
{
    assert(vertex < this->vetex_.size());
    
    this->vetex_[vertex].weight = weight;
}

// Returns the weight if vertex
unsigned Tree::getVertexWeight(unsigned vertex) const
{
    assert(vertex < this->vetex_.size());
    
    return this->vetex_[vertex].weight;
}

// Return the TreeEdge list of vertex
const std::vector<TreeEdge>& Tree::getEdgesOfVertex(unsigned vertex) const
{
    assert(vertex < this->vetex_.size());
    
    return this->vetex_[vertex].edges;
}

// Slow and dumb method.
void Tree::addConnection(unsigned from, unsigned to)
{
    // Pass 0: check for input
    assert(from < this->vetex_.size());
    assert(to   < this->vetex_.size());
    
    // Pass 1: check if it already has this edge
    const TreeVertex& fromVertex = this->vetex_[from];
    for (unsigned edge = 0; edge < fromVertex.edges.size(); ++edge)
    {
        assert(fromVertex.edges[edge].to != to); // same edge
    }
    
    // Pass 2: Check if it is an twin TreeEdge.
    //         (destination vertex has an Edge pointing to source vertex) 
    const TreeVertex& toVertex = this->vetex_[to];
    for (unsigned edge = 0; edge < toVertex.edges.size(); ++edge)
    {
        assert(toVertex.edges[edge].to != from); // twin edge
    }
    
    // Pass 3: Add the edge
    this->doAddConnection(from, to);
    
    // Pass 4: Check if it added a cycle on Tree
    assert(this->isTree() == true);
}

// Debug method that prints all tree
void Tree::dump() const
{
    log(LG_DEBUG, "tree has %u vertices and %u edges", this->vetex_.size(), this->nEdges_);
    
    for(unsigned v = 0; v < this->vetex_.size(); ++v)
    {
        const std::vector<TreeEdge>& edges = this->vetex_[v].edges;
        lognonl(LG_DEBUG, "  %2u [%3u, %3u]", this->vetex_[v].self, this->vetex_[v].weight, edges.size());
        
        for (unsigned e = 0; e < edges.size(); ++e)
        {
            lognonl(LG_DEBUG, " %u", edges[e].to);
        }
        log(LG_DEBUG, "%s", edges.size() == 0 ? " (leaf)" : "");
    }
}

// Writes the tree edge's to a GraphmlWriter
void Tree::writeEdges(GraphmlWriter& writer, bool writeLabels) const
{
    for(unsigned v = 0; v < this->vetex_.size(); ++v)
    {
        const std::vector<TreeEdge>& edges = this->vetex_[v].edges;
        for (unsigned e = 0; e < edges.size(); ++e)
        {
            if (writeLabels == true)
            { 
                char label[256] = { 0, };
                sprintf(label, "%d -> %d", edges[e].from, edges[e].to);

                writer.printEdge(edges[e].from, edges[e].to, edges[e].hasCut(), label);
            }
            else
            {
                writer.printEdge(edges[e].from, edges[e].to, edges[e].hasCut());
            }
        }
    }
}

// Writes a GraphML output to file
void Tree::writeToFile(const char * fileName) const
{
    std::ofstream outfile(fileName);
    
    if (outfile.good())
    {
        GraphmlWriter writer(&outfile);
        
        for (unsigned v = 0; v < this->vetex_.size(); ++v)
        {
            char label[256] = { 0, };
            
            sprintf(label, "%d/%d", v, this->vetex_[v].weight);
            
            writer.printNode(v, writer.getDefaultColorId(), label);
        }
        
        this->writeEdges(writer);   
    }
}

void Tree::writeToFile(const char * fileName, std::vector<PartitionComponent>& components) const
{
    std::ofstream outfile(fileName);
    
    if (outfile.good())
    {
        GraphmlWriter writer(&outfile);
        
        for (unsigned c = 0; c < components.size(); ++c)
        {
            std::set<unsigned>& vertices = components[c].vertices;
            for (std::set<unsigned>::const_iterator it = vertices.begin(); it != vertices.end(); it++)
            {
                unsigned v = *it;
                char label[256] = { 0, };
                
                sprintf(label, "%d/%d", v, this->vetex_[v].weight);
                
                writer.printNode(v, c, label);
            }
        }        
        this->writeEdges(writer);   
    }
}

// Debug method that checks for cycles and unconnection
void Tree::sanityTest() const
{
    unsigned nVertices = this->vetex_.size();
    bool visited[nVertices];
    
    // Inicialization
    for (unsigned mark = 0; mark < nVertices; ++mark)
    {
        visited[mark] = false;
    }
    
    if (this->nEdges_ != nVertices - 1)
    {
        log(LG_DEBUG, "sanity: is not a tree, has %u vertices and %u edges", nVertices, this->nEdges_);
    }
    
    // start BFS on root
    std::queue<unsigned> queue;
    
    // mark and enqueue the root
    visited[0] = true;
    queue.push(0);
    
    while(not queue.empty())
    {
	unsigned vertex = queue.front();
	queue.pop();
	
	const std::vector<TreeEdge>& edges = this->vetex_[vertex].edges;
	for (unsigned edge = 0; edge < edges.size(); ++edge)
	{
	    if (true == visited[edges[edge].to])
	    {
                log(LG_DEBUG, "sanity: found cycle that pass thru vertices %u %u", vertex, edges[edge].to);
	    }
	    else
	    {
		// mark and enqueue the vertex
		visited[edges[edge].to] = true;
		queue.push(edges[edge].to);
	    }
	}
    }
    
    // Check if tree is connected
    unsigned notVisitedCount = 0;
    for (unsigned mark = 0; mark < nVertices; ++mark)
    {
        if (not visited[mark])
        {
            notVisitedCount++;
        }
    }
    
    if (notVisitedCount > 0)
    {
        log(LG_DEBUG, "sanity: tree is not connected (cannot reach %d vertices on search starting on root)", notVisitedCount);
    }
}
