//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#include "graph.h"
#include "uf.h"
#include "random.h"
#include "log.h"

#include <cassert>
#include <queue>
#include <algorithm>

Graph::Graph(unsigned nVertices)
    : nVertices_(nVertices), 
      nEdges_(0),
      weights_(nVertices, 0),
      articulationPoint_(nVertices, false),
      adjMatrix_(nVertices, std::vector<bool>(nVertices, false)),
      adjs_(nVertices),
      edgeMark_(nVertices, std::vector<short>(nVertices, 0))
{
}

Graph::Graph(const Graph& g)
    : nVertices_(g.nVertices()), 
      nEdges_(g.nEdges()),
      weights_(g.nVertices(), 0),
      articulationPoint_(g.nVertices(), false),
      adjMatrix_(g.nVertices(), std::vector<bool>(g.nVertices(), false)),
      adjs_(g.nVertices()),
      edgeMark_(g.nVertices(), std::vector<short>(g.nVertices(), 0))
{
    for (unsigned u = 0; u < nVertices_; u++)
    {
        for (unsigned v = 0; v < nVertices_; v++)
        {
            adjMatrix_[u][v] = g.adjMatrix_[u][v];
            edgeMark_[u][v] = g.edgeMark_[u][v];
        }
        adjs_[u] = g.adjs_[u];
        weights_[u] = g.weights_[u];
        articulationPoint_[u] = g.articulationPoint_[u];
    }
}

unsigned Graph::nVertices() const
{
    return nVertices_;
}

unsigned Graph::nEdges() const
{
    return nEdges_;
}


// connect to vertices
void Graph::addConnection(unsigned u, unsigned v)
{
    assert(u < nVertices_);
    assert(v < nVertices_);
    
    if (not adjMatrix_[u][v])
    {
        adjMatrix_[u][v] = adjMatrix_[v][u] = true;
        nEdges_++;
        adjs_[u].insert(v);
        adjs_[v].insert(u);
    }
}

// disconnect to vertices
void Graph::removeConnection(unsigned u, unsigned v)
{
    assert(u < nVertices_);
    assert(v < nVertices_);
    
    if (adjMatrix_[u][v])
    {
        adjMatrix_[u][v] = adjMatrix_[v][u] = false;
        nEdges_--;
	adjs_[u].erase(v);
        adjs_[v].erase(u);
    }
}

// verify if two vertices are connected
bool Graph::isConnected(unsigned u, unsigned v) const
{
    assert(u < nVertices_);
    assert(v < nVertices_);

    return adjMatrix_[u][v];
}

// set the vertex wheight
void Graph::setVertexWeight(unsigned v, unsigned weight)
{
    assert(v < nVertices_);
    
    weights_[v] = weight;
}

// get the vertex wheight
unsigned Graph::getVertexWeight(unsigned v) const
{
    assert(v < nVertices_);
    
    return weights_[v];
}

// gets the vertex degree
unsigned Graph::getVertexDegree(unsigned v) const
{
    assert(v < nVertices_);
    
    return adjs_[v].size();
}

unsigned Graph::totalWeight() const
{
    unsigned total = 0;
    for (unsigned w = 0; w < weights_.size(); ++w) total += weights_[w];
    
    return total;
}

unsigned Graph::heaviest(unsigned ith) const
{
    assert(weights_.size() - ith > 0 && weights_.size() - ith < nVertices_);
    std::vector<unsigned> ws = weights_;
    
    std::sort(ws.begin(), ws.end());
    
    unsigned id = ws.size() - ith;
    
    return ws[id];
}

void Graph::updatedEdges() const
{
    if (edges_.size() != this->nEdges_)
    {
        edges_.clear();
        edges_.resize(this->nEdges_);
        unsigned e = 0;
        for (unsigned u = 0; u < nVertices_; u++)
        {
            for (unsigned v = u + 1; v < nVertices_; v++)
            {
                if (adjMatrix_[u][v])
                {
                    edges_[e].u = u;
                    edges_[e].v = v;
                    e++;
                }
            }
        }        
    }
}

Graph Graph::getRandomSpanningTree() const
{
    Graph tree(nVertices_);
    UF uf(nVertices_);
    Random random;

    updatedEdges();
    
    unsigned nEdges = 0;

    for (unsigned u = 0; u < nVertices_; u++)
        tree.setVertexWeight(u, this->getVertexWeight(u));
        
    while (nEdges != nVertices_ - 1)
    {
        unsigned edge = random.rand(this->nEdges_);
        
        if (uf.findElement(edges_[edge].u) != uf.findElement(edges_[edge].v))
	{
	    // can connect
            tree.addConnection(edges_[edge].u, edges_[edge].v);
	    nEdges++;
    
            uf.unionElements(edges_[edge].u, edges_[edge].v);
	}
    }
    
    return tree;
}

bool Graph::isConnected() const
{
    std::vector<bool> visited(nVertices_, false);
    std::queue<unsigned> queue;
    
    visited[0] = true;
    queue.push(0);
    
    while (not queue.empty())
    {
        unsigned v = queue.front();
        queue.pop();
        
        const std::set<unsigned>& adjs = adjs_[v];
        for (std::set<unsigned>::const_iterator u = adjs.begin(); u != adjs.end(); u++)
            if (not visited[*u])
            {
                visited[*u] = true;
                queue.push(*u);
            }
    }
    
    bool visitedAll = true;
    for (unsigned v = 0; visitedAll and v < nVertices_; ++v)
        if (not visited[v]) visitedAll = false;
    
    return visitedAll;
}

void Graph::initializeDFS()
{
    visited_.resize(nVertices_);
    discover_.resize(nVertices_);
    pred_.resize(nVertices_);
    low_.resize(nVertices_);
    
    for (unsigned v = 0; v < nVertices_; v++)
    {
        visited_[v] = false;
        discover_[v] = 0;
        pred_[v] = -1;
        low_[v] = 0;
    }
}

void Graph::dfs()
{
    initializeDFS();
    time_ = 0;
    
    for (unsigned v = 0; v < nVertices_; v++)
    {
        if (not visited_[v])
        {
            dfsVisit(v);
        }
    }
}

void Graph::dfsVisit(unsigned v)
{
    visited_[v] = true;
    low_[v] = discover_[v] = ++time_;
    
    for (std::set<unsigned>::const_iterator w = adjs_[v].begin(); w != adjs_[v].end(); w++)
    {
        if (visited_[*w] == false)
        {
            pred_[*w] = v;
            dfsVisit(*w);
            
            // When dfsVisit(w) returns,
            // low_[w] stores the
            // lowest value it can climb up
            // for a subtree rooted at w.
            
            low_[v] = std::min(low_[v], low_[*w]);
        } 
        else if (*w != (unsigned)pred_[v]) 
        {
            // {v, w} is a back edge
            low_[v] = std::min(low_[v], discover_[*w]);
        }
    }
}

namespace boost
{
    struct edge_component_t
    {
        enum 
        { 
            num = 555 
        };
        typedef edge_property_tag kind;
    }
    edge_component;
}

void Graph::computeArticulationPoints()
{
#if 0
    dfs();
    for (unsigned v = 0; v < nVertices_; v++)
        if (pred_[v] == -1) 
        { 
            // v is a root
            // if 2 or more vertices has pred as a root, then the root is an
            // aticulantion point.
            unsigned totalPreds = 0;
            for (unsigned u = 0; u < nVertices_; u++)
            {
                if (pred_[u] == (int) v) totalPreds++;
            }
            
            if (totalPreds > 1) articulationPoint_[v] = true;
            
            // if (adjs_[v].size() > 1) articulationPoint_[v] = true;
        } 
        else
        {
            for (std::set<unsigned>::const_iterator w = adjs_[v].begin(); w != adjs_[v].end(); w++)
                if (low_[*w] >= discover_[v])
                {
                    articulationPoint_[v] = true;
                }
	}
#else
    articulationPoint_.clear();
    articulationPoint_.resize(nVertices_);
    for (unsigned v = 0; v < nVertices_; v++) articulationPoint_[v] = false;
    
    using namespace boost; 
    typedef adjacency_list<vecS, vecS, undirectedS, no_property, property < edge_component_t, std::size_t> > graph_t;
    typedef graph_traits<graph_t>::vertex_descriptor vertex_t;
    
    updatedEdges();
    
    graph_t g(nVertices_);
    
    log(LG_DEBUG, "graph: %d vertices", nVertices_);
    for (unsigned e = 0; e < edges_.size(); e++)
    {
        log(LG_DEBUG, "graph: adding edge %d <-> %d", edges_[e].u, edges_[e].v);
	add_edge(edges_[e].u, edges_[e].v, g);
    }

    property_map<graph_t, edge_component_t>::type component = get(edge_component, g);

    std::size_t nc = biconnected_components(g, component);
    log(LG_DEBUG, "graph: has %d 2-connected components", nc);
    
    std::vector<vertex_t> art_points;
    articulation_points(g, std::back_inserter(art_points));
    
    lognonl(LG_DEBUG, "graph: articulation points -> ");
    for (std::size_t i = 0; i < art_points.size(); ++i)
        lognonl(LG_DEBUG, "%d ", art_points[i]);
    log(LG_DEBUG, "");
    
    for (std::size_t i = 0; i < art_points.size(); ++i)
        articulationPoint_[art_points[i]] = true;
               
#endif
}

bool Graph::isArticulationPoint(unsigned v) const
{
    assert(v < nVertices_);
    
    return articulationPoint_[v];
}

bool Graph::isBiconnected()
{
    bool biconnected = true;
    computeArticulationPoints();
    
    for (unsigned v = 0; biconnected and v < nVertices_; v++)
    {
        if (articulationPoint_[v] == true)
        {
            biconnected = false;
        }
    }
    
    return biconnected;
}

Graph Graph::getSubgraph(const std::vector<unsigned>& vertices) const
{
    assert(vertices.size() <= nVertices_);
    
    Graph subg(vertices.size());
    
    for (unsigned v = 0; v < vertices.size(); v++)
    {
        assert(vertices[v] < nVertices_);
        subg.setVertexWeight(v, this->getVertexWeight(vertices[v]));
        
        const std::set<unsigned>& adjs = this->adjs_[vertices[v]];
        
        for (std::set<unsigned>::const_iterator u = adjs.begin(); u != adjs.end(); u++)
        {
            bool isInSubgraph = false;
            unsigned destination = 0;
            for (unsigned w = 0; not isInSubgraph and w < vertices.size(); w++)
            {
                if (vertices[w] == *u)
                {
                    isInSubgraph = true;
                    destination = w;
                }
            }
            
            if (isInSubgraph)
            {
                subg.addConnection(v, destination);
            }
        }
    }
    
    return subg;
}

Graph Graph::getSubgraph(const std::set<unsigned>& vertices, std::vector<unsigned>& vetexvector) const
{
    for (std::set<unsigned>::const_iterator v = vertices.begin(); v != vertices.end(); v++)
    {
        vetexvector.push_back(*v);
    }
    
    return getSubgraph(vetexvector);
}

unsigned Graph::getEdgeMark(unsigned u, unsigned v) const
{
    assert(u < nVertices_);
    assert(v < nVertices_);
    
    return edgeMark_[u][v];
}

void Graph::setEdgeMark(unsigned u, unsigned v, unsigned mark) const 
{
    assert(u < nVertices_);
    assert(v < nVertices_);
    
    edgeMark_[u][v] = edgeMark_[v][u] = static_cast<short>(mark);
}

unsigned Graph::weightOfCompoenent(unsigned startVertex, short forbiddenEdges, std::set<unsigned>& accessed) const
{
    assert(startVertex < nVertices_);
    std::vector<bool> visited(nVertices_, false);
    std::queue<unsigned> queue;
    unsigned componentWeight = 0;
    
    visited[startVertex] = true;
    queue.push(startVertex);
    
    while (not queue.empty())
    {
        unsigned v = queue.front();
        queue.pop();
        
        accessed.insert(v);
        componentWeight += getVertexWeight(v);
        
        const std::set<unsigned>& adjs = adjs_[v];
        for (std::set<unsigned>::const_iterator u = adjs.begin(); u != adjs.end(); u++)
            if (not visited[*u] and edgeMark_[*u][v] != forbiddenEdges)
            {
                visited[*u] = true;
                queue.push(*u);
            }
    }
    
    return componentWeight;
}


Graph readGraphFromXML(const char * filename, int& clusters)
{
    TiXmlDocument file(filename);
    
    if (not file.LoadFile())
    {
        log(LG_ERROR, "cannot load file [%s]", filename);
        exit(0);
    }
    
    TiXmlHandle hFile(&file);
    TiXmlElement* pElem;
    TiXmlHandle hRoot(0);

    // graph
    int nVertices;
    {
        pElem = hFile.FirstChildElement().Element();
	// should always have a valid root but handle gracefully if it does
	if (!pElem)
	{
	    log(LG_ERROR, "cannot load file [%s], no root found", filename);
            exit(0);
	}
	pElem->QueryIntAttribute("nodes", &nVertices);
        pElem->QueryIntAttribute("clusters", &clusters);
        
        // save this for later
	hRoot = TiXmlHandle(pElem);
    }
    
    Graph graph(nVertices);
    
    // nodes
    {
	pElem = hRoot.FirstChild("nodes").FirstChild().Element();
	for(; pElem; pElem=pElem->NextSiblingElement())
	{
	   // id="12" weight="1"
	   int id, weight;
	   pElem->QueryIntAttribute("id", &id);
	   pElem->QueryIntAttribute("weight", &weight);
	   
	   graph.setVertexWeight(id - 1, weight);
	}
    }
    
    // arcs
    {
    	pElem = hRoot.FirstChild("arcs").FirstChild().Element();
	for(; pElem; pElem=pElem->NextSiblingElement())
	{
	   // id="10" from="17" to="2" weight="11"
	   int from, to;
	   pElem->QueryIntAttribute("from", &from);
	   pElem->QueryIntAttribute("to", &to);
	   
	   graph.addConnection(from - 1, to -1);
	}
    }
    
    return graph;
}

