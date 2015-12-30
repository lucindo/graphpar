//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#include "improvedHeuristicCommon.h"

#include "random.h"
#include "partition.h"
#include "log.h"

#include <vector>
#include <stack>
#include <set>
#include <algorithm>

void removeCircuitFromTreeByEdge(Graph& tree, unsigned u, unsigned v)
{
    log(LG_DEBUG, "removeCircuitFromTreeByEdge remove the circuit present without touching edge (%u, %u)", v, u);
    
    // Idea:
    // Doing a DFS search starting on some vertex of circuit (u or v)
    // when we access the vertex again, we have on the queue all the vertex (and edges) of the circuit
    std::vector<bool> visited(tree.nVertices(), false);
    std::stack<unsigned> stack;
    
    visited[u] = true;
    visited[v] = true;
    stack.push(v);
    while (not stack.empty())
    {
        start:
        unsigned w = stack.top();
        
        // see if we can go down
        bool shouldStop = false;
        const std::set<unsigned>& adjs = tree.adjs_[w];
        for (std::set<unsigned>::const_iterator x = adjs.begin(); x != adjs.end(); x++)
	    if (*x != u and not visited[*x])
	    {
		visited[*x] = true;
		stack.push(*x);
		goto start;
	    }
	    else if (stack.size() > 1 and *x == u) // found the circuit!
	    {
		stack.push(*x);
		shouldStop = true;
		break;
	    }
        
        if (shouldStop) break;
        stack.pop();
    }
    
    assert(not stack.empty());
    
    log(LG_DEBUG, "removeCircuitFromTreeByEdge stack has %u elements", stack.size());
    
    std::vector<unsigned> vertices;
    while (not stack.empty())
    {
        unsigned e = stack.top();
        vertices.push_back(e);
        stack.pop();
        log(LG_DEBUG, "removeCircuitFromTreeByEdge stack has elemet %u", e);
    }
    
/*    for (unsigned s = 0; s < vertices.size() - 1; s++)
    {
        log(LG_DEBUG, "removeCircuitFromTreeByEdge circuit edge found (%u, %u)", vertices[s], vertices[s+1]);
        assert(tree.isConnected(vertices[s], vertices[s+1]));
    }*/
    
    tree.removeConnection(vertices[0], vertices[1]);
    // assert(tree.isConnected());
}

// Heart of the algorithm
// Try to connect some of the elements of the components on the tree
bool tryToConnect(const Graph& graph, Graph& tree, const std::vector<PartitionComponent>& components, unsigned light, unsigned heavy)
{
    log(LG_DEBUG, "trying to connect component %u(%u) to %u(%u)", light, components[light].weight, heavy, components[heavy].weight);
    bool connectionDone = false;
    
    const std::set<unsigned>& lightVertices = components[light].vertices;
    const std::set<unsigned>& heavyVertices = components[heavy].vertices;
    
    // Check if some of elements of 'light' has a direct connetion on some element of 'heavy' on tree
    for (std::set<unsigned>::const_iterator lv = lightVertices.begin(); lv != lightVertices.end(); lv++)
    {
        unsigned lvv = *lv;
        for (std::set<unsigned>::const_iterator hv = heavyVertices.begin(); hv != heavyVertices.end(); hv++)
        {
            unsigned hvv = *hv;
            
	    if (tree.isConnected(lvv, hvv))
            {
                log(LG_DEBUG, "vertex %u of light component is connected to vertex %u of heavy component on tree", lvv, hvv);
                return connectionDone;
            }
        }
    }
    
    // There is no connection on tree of elements of the two patition components
    
    // Now we have to check if the connection is possible on the graph
    for (std::set<unsigned>::const_iterator lv = lightVertices.begin(); lv != lightVertices.end(); lv++)
    {
        unsigned lvv = *lv;
        for (std::set<unsigned>::const_iterator hv = heavyVertices.begin(); hv != heavyVertices.end(); hv++)
        {
            unsigned hvv = *hv;
            
	    if (graph.isConnected(lvv, hvv))
            {
                log(LG_DEBUG, "is possible to connect vertex %u of light component to vertex %u of heavy component by the graph", lvv, hvv);
                
		tree.addConnection(lvv, hvv);
                connectionDone = true;
                
                removeCircuitFromTreeByEdge(tree, lvv, hvv);
                
                return connectionDone;
            }
        }
    }    
    
    log(LG_DEBUG, "impossible to connect component %u(%u) to %u(%u)", light, components[light].weight, heavy, components[heavy].weight);
    return connectionDone;
}

bool adjustTree(const Graph& graph, Graph& tree, Partition& partition, bool doSort)
{
    bool couldPerformAdjust = false;
    
    // we first transform the input to the corresponding to work with the Graph parameters
    std::vector<PartitionComponent> components(partition.nComponents());
    for (unsigned c = 0; c < partition.nComponents(); ++c)
    {
        components[c].weight = partition.component(c).weight;
	components[c].vertices.clear();
        for (std::set<unsigned>::const_iterator v = partition.component(c).vertices.begin(); 
             v != partition.component(c).vertices.end(); v++)
        {
                if (*v != 0) components[c].addVertex((*v) - 1);
	}
    }
    
    log(LG_DEBUG, "adjustTree called");
    
    if (doSort)
    {
	// we sort the components by weight
	std::sort(components.begin(), components.end(), comparePartitionComponents);
    }
    else
    {
        // we need to shuffle
        Random random;
        for (unsigned e = 0; e < components.size(); e++)
        {
            unsigned p = random.rand(components.size() - 1);
            std::swap(components[e], components[p]);
        }
    }
    
    for (unsigned l = 0; not couldPerformAdjust and l < components.size() - 1; ++l)
    {
        // for each component (from de lightest to the heaviest) we try
        // to make de adjustment of the tree
        for (unsigned h = components.size() - 1; not couldPerformAdjust and h > l; --h)
        {
            couldPerformAdjust = tryToConnect(graph, tree, components, l, h);
        }
    }
    
    log(LG_DEBUG, "adjustTree called returning %s", couldPerformAdjust ? "true" : "false");
    return couldPerformAdjust;
}
