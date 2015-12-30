//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#include "chlebikovaApproximation.h"
#include "log.h"

#include <set>
#include <map>
#include <algorithm>
#include <cassert>

// Helper struct used by chlebikovaBlockApproximation
struct Vertex
{
    unsigned id, weight;
};

// Helper functions used to sort Vertex
bool compareVertex(Vertex v1, Vertex v2)
{
    return v1.weight < v2.weight;
}
bool compareInvertedVertex(Vertex v1, Vertex v2)
{
    return v1.weight > v2.weight;
}


// Perform the approximation algorithm by Chlebikova on a 2-connected graph
// Algorithm (4/3-approximation):
// 0. Order V(G) in such a way that weight(v1) >= weight(v2) >= ...
// 1. V1 = v1 and V2 = V(G) \ v1
// 2. if weight(V1) >= 1/2 weight(V) then goto step 4 else goto step 3
// 3. V0 = {u in V2 | (V1 union u, V2 \ u) is a connected partition of G}
//    Choose u in V2 such that weight(u) = min_{u in V2}weight(u)
//    if weight(u) < weight(V) - 2 weight(V1)
//       then V1 = V1 union u; V2 = V2 \ u; goto step 2
//       else goto step 3
// 4. return Partition(V1, V2)
Partition chlebikovaBlockApproximation(const Graph& graph, unsigned P, unsigned K)
{
    double ratio = P/(double)K;
    // we want only 1/2 <= P/K < 1
    assert(ratio < 1.0); 
    assert(ratio >= 0.5);

    unsigned weightV1 = 0;
    unsigned weightV2 = 0;
    unsigned weightGraph = graph.totalWeight();

    unsigned beta = (P * weightGraph) / K;
    
    std::set<unsigned> V1, V2;
    
    std::vector<Vertex> vertices(graph.nVertices());
    for (unsigned v = 0; v < graph.nVertices(); v++)
    {
        vertices[v].id = v;
        vertices[v].weight = graph.getVertexWeight(v);
    }
    
    // We sort the vertices by weight on inverted order
    std::sort(vertices.begin(), vertices.end(), compareInvertedVertex);
    
    // V1 initialization
    V1.insert(vertices[0].id);
    weightV1 = vertices[0].weight;
    
    // V2 initialization
    for (unsigned v = 1; v < vertices.size(); v++)
    {
        V2.insert(vertices[v].id);
        weightV2 += vertices[v].weight;
    }
    
    // main loop
    while (weightV1 < beta)
    {
        std::vector<Vertex> possibles;
        
        // The possible vertices to go from V2 to V1 are some neighbor of V1 (direct connection)
        for (std::set<unsigned>::const_iterator u = V1.begin(); u != V1.end(); u++)
        {
            const std::set<unsigned>& adjs = graph.adjs_[*u];
            
            for (std::set<unsigned>::const_iterator v = adjs.begin(); v != adjs.end(); v++)
            {
                if (V2.find(*v) != V2.end())
                {
                    Vertex possible;
                    possible.id = *v;
                    possible.weight = graph.getVertexWeight(*v);
                    
                    possibles.push_back(possible);
                }
            }
        }    
        
        // sort by weight (ensure the min function)
        std::sort(possibles.begin(), possibles.end(), compareVertex);
        // we must choose the first vertex in possibles that not disconnects V2

        // we build a subgraph with all V2 vertices
        Graph V2Graph(V2.size());
        std::map<unsigned, unsigned> Graph2V2GraphMapping; // maps vertex ids from graph to V2Graph
        unsigned V2GraphId = 0;
        for (std::set<unsigned>::const_iterator u = V2.begin(); u != V2.end(); u++)
        {
            Graph2V2GraphMapping[*u] = V2GraphId++;;
        }
        assert(V2GraphId == V2.size());
        // we can use Graph2V2GraphMapping[Graph vertex id] to return a V2Graph vertex id
        
        // building the graph
        for (std::set<unsigned>::const_iterator u = V2.begin(); u != V2.end(); u++)
        {
            const std::set<unsigned>& adjs = graph.adjs_[*u];
            for (std::set<unsigned>::const_iterator v = adjs.begin(); v != adjs.end(); v++)
            {
                if (V2.find(*v) != V2.end())
                {
                    // u and v belongs only to V2 (vertex index from graph)
                    V2Graph.addConnection(Graph2V2GraphMapping[*u], Graph2V2GraphMapping[*v]);
                }
            }
        }
        // now we must find all articulation points on V2Graph
        V2Graph.computeArticulationPoints();
        
        int choosenOne = -1;
        for (unsigned p = 0; p < possibles.size(); p++)
        {
            unsigned pInV2Graph = Graph2V2GraphMapping[possibles[p].id];
            if (V2Graph.isArticulationPoint(pInV2Graph) == false)
            {
                choosenOne = p;
                break;
            }
        }
        
        if (choosenOne != -1)
        {
            if (possibles[choosenOne].weight < 2 * (beta - weightV1))
            {
                V1.insert(possibles[choosenOne].id);
                weightV1 += possibles[choosenOne].weight;
                
                V2.erase(possibles[choosenOne].id);
                weightV2 -= possibles[choosenOne].weight;;
            }
            else break;
        }
        else break;
    }
    
    // writing out the result
    std::vector<PartitionComponent> components(2);
    for (std::set<unsigned>::const_iterator i = V1.begin(); i != V1.end(); i++)
    {
        components[0].addVertex(*i);
    }
    for (std::set<unsigned>::const_iterator i = V2.begin(); i != V2.end(); i++)
    {
        components[1].addVertex(*i);
    }
    components[0].weight = weightV1;
    components[1].weight = weightV2;
    
    return components;
}

// helper structures
struct Block
{
    std::vector<unsigned> vertices;
    unsigned edgeComponentID;
};

namespace boost
{
    struct Edge_Component_t
    {
        enum 
        { 
            num = 555 
        };
        typedef edge_property_tag kind;
    }
    Edge_Component;
}


// Perform the approximation algorithm by Chlebikova
// on each block (biconnected component)
Partition chlebikovaApproximation(const Graph& graph, unsigned P, unsigned K)
{
    log(LG_DEBUG, "chlebikovaApproximation called (graph with %d vertices)", graph.nVertices());
    using namespace boost; 
    typedef adjacency_list<vecS, vecS, undirectedS, no_property, property < Edge_Component_t, std::size_t> > graph_t;
    typedef graph_traits<graph_t>::vertex_descriptor vertex_t;
    
    graph_t g(graph.nVertices());
    
    for (unsigned e = 0; e < graph.edges_.size(); e++)
    {
        log(LG_DEBUG, "adding edge %d <-> %d", graph.edges_[e].u, graph.edges_[e].v);
	add_edge(graph.edges_[e].u, graph.edges_[e].v, g);
    }

    property_map<graph_t, Edge_Component_t>::type component = get(Edge_Component, g);

    std::size_t num_comps = biconnected_components(g, component);
    
    log(LG_DEBUG, "graph has %d 2-connected components", num_comps);
    
    std::vector<vertex_t> art_points;
    articulation_points(g, std::back_inserter(art_points));
    
    lognonl(LG_DEBUG, "articulation points (graph): ");
    for (unsigned v = 0; v < graph.nVertices(); v++)
        if (graph.isArticulationPoint(v)) lognonl(LG_DEBUG, "%d ", v);
    log(LG_DEBUG, "");
         
    lognonl(LG_DEBUG, "articulation points (boost): ");
    for (std::size_t i = 0; i < art_points.size(); ++i)
        lognonl(LG_DEBUG, "%d ", art_points[i]);
    log(LG_DEBUG, "");
            
    std::vector<Block> blocks(num_comps);
    
    graph_traits<graph_t>::edge_iterator ei, ei_end;
    for (tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
    {
        unsigned blockid = component[*ei];
        unsigned u = source(*ei, g);
        unsigned v = target(*ei, g);
        
        bool inlist = false;
        for (unsigned k = 0; not inlist and k < blocks[blockid].vertices.size(); k++) 
            if (blocks[blockid].vertices[k] == u) inlist = true;    
        if (not inlist) blocks[blockid].vertices.push_back(u);
        
        inlist = false;
        for (unsigned k = 0; not inlist and k < blocks[blockid].vertices.size(); k++) 
            if (blocks[blockid].vertices[k] == v) inlist = true;
        if (not inlist) blocks[blockid].vertices.push_back(v);

        blocks[blockid].edgeComponentID = blockid;
        
        graph.setEdgeMark(u, v, blockid);
    }
    
    for (unsigned b = 0; b < blocks.size(); b++)
    {
        lognonl(LG_DEBUG, "Block %d has %d vertices: ", blocks[b].edgeComponentID, blocks[b].vertices.size());
        for (unsigned v = 0; v < blocks[b].vertices.size(); v++)
            lognonl(LG_DEBUG, "%d ", blocks[b].vertices[v]);
	log(LG_DEBUG, "");
    }
    
    // The code above was finished at 4h45 a.m. (after more than 16 hours programming straight)
    // At that time it seems to work. Amazing!
    Partition best;
    for (unsigned b = 0; b < blocks.size(); b++)
    {
        Graph sg = graph.getSubgraph(blocks[b].vertices);
        
        std::vector<std::set<unsigned> > articulationAppened(sg.nVertices());
        bool hasArticulation = false;
        // we have to recalculate the weight of articulation points on original graph
        for (unsigned v = 0; v < blocks[b].vertices.size(); v++)
            if (graph.isArticulationPoint(blocks[b].vertices[v]))
            {
                // we must sum all the weights of vertices on the searh begining on blocks[b].vertices[v]
                // that don't uses edges with mark blocks[b].edgeComponentID
                hasArticulation = true;
                
                unsigned articulationWeight = graph.weightOfCompoenent(blocks[b].vertices[v], 
                                                                       blocks[b].edgeComponentID, 
                                                                       articulationAppened[v]);
                sg.setVertexWeight(v, articulationWeight);
            }
            
        log(LG_DEBUG, "Block %d ", blocks[b].edgeComponentID);
            
        if (not hasArticulation and blocks.size() != 1)
        {
            log(LG_ERROR, "articulation point not found >> this should never happend!");
        }
        
        for (unsigned v = 0; v < sg.nVertices(); v++)
        {
            lognonl(LG_DEBUG, " vertex %d[%d]: ", v, sg.getVertexWeight(v));
            
            const std::set<unsigned>& adjs = sg.adjs_[v];
            for (std::set<unsigned>::const_iterator u = adjs.begin(); u != adjs.end(); u++)
                lognonl(LG_DEBUG, "%d ", *u);
	    
	    log(LG_DEBUG, "");
        }
        
        Partition p = chlebikovaBlockApproximation(sg, P, K);
        lognonl(LG_DEBUG, " lightest parition weight: %d", p.lightestWeight());
        if (p.lightestWeight() > best.lightestWeight())
        {
            lognonl(LG_DEBUG, " >> best so far :)");
            std::vector<PartitionComponent> components(2);
            
            for (unsigned pc = 0; pc < 2; pc++)
            {
                std::set<unsigned>::const_iterator v = p.component(pc).vertices.begin();
		for (; v != p.component(pc).vertices.end(); v++)
		{
		    unsigned vinsg = *v;
		    
		    components[pc].addVertex(blocks[b].vertices[vinsg]);
		    
		    std::set<unsigned>::const_iterator appended = articulationAppened[vinsg].begin();
		    for (; appended != articulationAppened[vinsg].end(); appended++)
		    {
			components[pc].addVertex(*appended);
		    }
		}
		components[pc].weight = p.component(pc).weight;
	    }
            best = components;
        }
        log(LG_DEBUG, "");
    }
    
    return best;
}

