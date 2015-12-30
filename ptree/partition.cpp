//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#include "partition.h"
#include "log.h"

#include <queue>
#include <limits>
#include <functional>

// Helper function used to sort PartitionComponent elements of a container
bool comparePartitionComponents(PartitionComponent p1, PartitionComponent p2)
{
    return p1.weight < p2.weight;
}

// Performs the Perl & Schach max-min q-partition algorithm
// Input:
//  - Tree's reference
//  - q (number of components of parition)
// Output:
//  - Vector of partition components

namespace std
{
    template <>
    struct less<const TreeEdge *> : public binary_function<const TreeEdge *, const TreeEdge *, bool>
    {
        bool operator()(const TreeEdge * const & x, const TreeEdge * const & y) const
        {
            if (x->from == y->from) return x->to < y->to;
            return x->from < y->from;
        }
    };
}

void partitionByPerlSchach(Tree& tree, unsigned q, std::vector<PartitionComponent>& components, unsigned * iterations)
{
    // The algorithm has the following steps:
    //  1. Assign all q - 1 cuts to the root's edge 
    //  2. Wmin = weight of ligthtest component
    //  3. Find a shift that minimizes the weight W of down component
    //     resulted from the cut shift.
    //  4. If W >= Wmin do the shift and go to 2
    //  5. Terminate (Wmin is the weight of lightest component)
    
    assert(q > 1);  
    
    unsigned iter = 0;
    unsigned k = q - 1; // total cuts needed
    std::vector<const TreeEdge *> edgeCuts(k, 0); // pointers to edges with cuts (for fast access)
    
    // Pass 1:
    initializeCuts(tree, k, edgeCuts);
    
    // Pass 2:
    initializeComponentsWeight(tree, components);
    
    int Wmin = weightOfLightestComponent(components);
    
    // Pass 2 and 3:
    while (42)
    {
        iter++;
        int W = -1;
        
        // auxiliary vars to posterior shift
        const TreeEdge * sourceEdge      = 0;
        const TreeEdge * destinationEdge = 0;
        
        // log(LG_DEBUG, "searching for good shifts");
        // Searching for an good cut shift
        std::set<const TreeEdge *> edgesSeen;
        for (unsigned ec = 0; ec < edgeCuts.size(); ++ec)
        {
            const TreeEdge * sEdge = edgeCuts[ec];
            unsigned sourceVertex = sEdge->to;

            log(LG_DEBUG, "analising edge %d of %d (%u -> %u)", ec + 1, edgeCuts.size(), sEdge->from, sEdge->to);

            if (edgesSeen.find(sEdge) != edgesSeen.end())
            {
                log(LG_DEBUG, "edge %d (%u -> %u) already seen", ec + 1, sEdge->from, sEdge->to);
                continue;
            }
            edgesSeen.insert(sEdge);

            CutRemoveGuard removeCutGuard(sEdge);

            // we analise all possible shifts from this edge
            const std::vector<TreeEdge>& edges = tree.getEdgesOfVertex(sourceVertex);
            for (unsigned d = 0; d < edges.size(); d++)
            {
                const TreeEdge * dEdge = &edges[d];
                assert(sourceVertex == dEdge->from); // must always be true

                if (not dEdge->hasCut()) // a shift is possible
                {
                    CutAddGuard addCutGuard(dEdge);

                    unsigned downComponentVertex = dEdge->to;
                    std::set<unsigned> downComponentVertices;
                    int downComponentWeight = weightOfComponent(tree, downComponentVertex, downComponentVertices);

                    lognonl(LG_DEBUG, "possible shift found to edge (%u -> %u) of weight %u (actual %d Wmin %d)",
                            dEdge->from, dEdge->to, downComponentWeight, W, Wmin);

                    if (downComponentWeight > W) // best shift so far
                    {
                        lognonl(LG_DEBUG, " [best so far]");
                        W = downComponentWeight;
                        sourceEdge = sEdge;
                        destinationEdge = dEdge;
                    }
                    log(LG_DEBUG, "");
		}
            }
        }

        // Pass 4:
        if (W >= Wmin)
        {
            // Perform the shift on edges
            sourceEdge->removeCut();
            destinationEdge->addCut();

            log(LG_DEBUG, "shiftting cut from (%u -> %u) to (%u -> %u)", sourceEdge->from, sourceEdge->to, destinationEdge->from, destinationEdge->to);

            // we replace one entry of sourceEdge to a destinationEdge
            for (unsigned ec = 0; ec < edgeCuts.size(); ++ec)
            {
                if (edgeCuts[ec] == sourceEdge)
                {
                    edgeCuts[ec] = destinationEdge;
                    break;
                }
            }

            // We need to recalculate the weight of all components
            // down of destinationEdge->from
            recalculateWeightOfComponents(tree, components, destinationEdge->from);

            Wmin = weightOfLightestComponent(components);
	}
	else break; // Pass 5
    }

    if (iterations) *iterations = iter;
    log(LG_DEBUG, "components %d, Wmin %d, iterations %d", components.size(), Wmin, iter);
}

// Assign all cuts on out edge of tree's root
void initializeCuts(Tree& tree, unsigned q, std::vector<const TreeEdge *>& edgeCuts)
{
    const std::vector<TreeEdge>& rootEdges = tree.getEdgesOfVertex(tree.getRootVertex());
    
    assert(rootEdges.size() == 1); // should always be one out edge from the root

    tree.cleanCuts(); // we need a clean tree to execute the algorithm

    const TreeEdge& edge = rootEdges[0];
    for (unsigned c = 0; c < q; c++)
    {
        edge.addCut();
        edgeCuts[c] = &edge;
    }
}

// Initalize the components vector
void initializeComponentsWeight(Tree& tree, std::vector<PartitionComponent>& components)
{
    // On starting we have only two components. The root and the rest of tree.
    // We calculate the weight of these components and assing to components vector
    // this way: position 0 is the root component, positions 1 receive 
    // a copy of data calculated from subtree starting on root's child.
    
    components.resize(2);
    
    // Root component
    components[0].rootVertex = tree.getRootVertex();
    components[0].weight = weightOfComponent(tree, components[0].rootVertex, components[0].vertices);

    // Root's child component
    const std::vector<TreeEdge>& rootEdges = tree.getEdgesOfVertex(tree.getRootVertex());
    
    unsigned rootChild = rootEdges[0].to;
    std::set<unsigned> rootChildPatitionVertices;
    unsigned rootChildPatitionWeight = weightOfComponent(tree, rootChild, rootChildPatitionVertices);
    
    // Raw copy to component's vector
    components[1].rootVertex = rootChild;
    components[1].weight = rootChildPatitionWeight;
    components[1].vertices = rootChildPatitionVertices;
}

// Returns the weight of lightest component
unsigned weightOfLightestComponent(std::vector<PartitionComponent>& components)
{
    unsigned weight = std::numeric_limits<unsigned>::max();
    
    for (unsigned c = 0; c < components.size(); ++c)
    {
        if (components[c].weight < weight)
        {
            weight = components[c].weight;
        }
    }
    
    log(LG_DEBUG, "weight of lightest of %u components is %u", components.size(), weight);
    
    return weight;
}

// Calculate the weight of componet starting on vertex 'startingVertex'
// Add all component vertices on the set reference
unsigned weightOfComponent(Tree& tree, unsigned startingVertex, std::set<unsigned>& vertices)
{
    unsigned weight = 0;
    unsigned nVertices = tree.getVertexCount();
    bool visited[nVertices];
    
    vertices.clear();
    // Inicialization
    for (unsigned mark = 0; mark < nVertices; ++mark)
    {
        visited[mark] = false;
    }
    
    // start BFS on startingVertex
    std::queue<unsigned> queue;
    
    // mark and enqueue the root
    visited[startingVertex] = true;
    queue.push(startingVertex);
    weight += tree.getVertexWeight(startingVertex);
    vertices.insert(startingVertex);
    
    while(not queue.empty())
    {
	unsigned vertex = queue.front();
	queue.pop();
	
	const std::vector<TreeEdge>& edges = tree.getEdgesOfVertex(vertex);
	for (unsigned edge = 0; edge < edges.size(); ++edge)
	{
	    // The second condition is aways true. Should put 
	    // an esle statemante and log error on it
	    if (not edges[edge].hasCut() and not visited[edges[edge].to]) 
	    {
		// mark and enqueue the vertex
		visited[edges[edge].to] = true;
		queue.push(edges[edge].to);
		
		weight += tree.getVertexWeight(edges[edge].to);
		vertices.insert(edges[edge].to);
	    }
	}
    }

    return weight;
}

// Recalculate the weight of all comonents afected by the shift
void recalculateWeightOfComponents(Tree& tree, std::vector<PartitionComponent>& components, unsigned /*startingVertex*/)
{
    // 'startingVertex' moved of component. 
    // TODO: recalculate only the necessary components
    //       for now we aways get the worst case
    
    components.clear(); // we will recalculate all
    
    unsigned nVertices = tree.getVertexCount();
    std::vector<bool> visited(nVertices, false);
    std::set<unsigned> vertexStart;
    
    vertexStart.insert(0);
    
    lognonl(LG_DEBUG, "cuts on edges: ");
    for (unsigned v = 0; v < tree.getVertexCount(); v++)
        for (unsigned e = 0; e < tree.getEdgesOfVertex(v).size(); e++)
            if (tree.getEdgesOfVertex(v)[e].hasCut())
            {
                lognonl(LG_DEBUG, "(%u -> %u) ", tree.getEdgesOfVertex(v)[e].from, tree.getEdgesOfVertex(v)[e].to);
                vertexStart.insert(tree.getEdgesOfVertex(v)[e].to);
	    }
    log(LG_DEBUG, "");

    // This method can be called with a disconnected tree
    // so we need to start the search on all possivel subtrees
    for (std::set<unsigned>::const_iterator v = vertexStart.begin(); v != vertexStart.end(); ++v)
    {
        if (not visited[*v])
        {
            PartitionComponent component;
            
            component.rootVertex = *v;
            unsigned& weight = component.weight;
            std::set<unsigned>& vertices = component.vertices;
            
            // start BFS on vertex v
            std::queue<unsigned> queue;
            
            // mark and enqueue the vertex
            visited[*v] = true;
            queue.push(*v);
            
	    weight += tree.getVertexWeight(*v);
	    vertices.insert(*v);
            
            while(not queue.empty())
            {
                unsigned vertex = queue.front();
                queue.pop();
                
                const std::vector<TreeEdge>& edges = tree.getEdgesOfVertex(vertex);
                for (unsigned edge = 0; edge < edges.size(); ++edge)
                {
		    // The second condition is aways true. Should put 
		    // an esle statemante and log error on it
		    if (not edges[edge].hasCut() and not visited[edges[edge].to]) 
		    {
			// mark and enqueue the vertex
			visited[edges[edge].to] = true;
			queue.push(edges[edge].to);
			
			weight += tree.getVertexWeight(edges[edge].to);
			vertices.insert(edges[edge].to);
		    }
                }
            }
            components.push_back(component);
        }
        else
        {
            log(LG_ERROR, "vertex %u already visited (should never happend)", *v);
        }
    }
    
    lognonl(LG_DEBUG, "calculated the weight of %u components [", components.size());
    for (unsigned c = 0; c < components.size(); c++)
    {
        if (c > 0) lognonl(LG_DEBUG, ", ");
        lognonl(LG_DEBUG, "w %u s %u r %d", components[c].weight, components[c].vertices.size(), components[c].rootVertex);
    }
    log(LG_DEBUG, "]");
}

// Gets a Tree (input for the Perl & Schach max-min tree partitioning) from the Graph representation
void getTreeFromRandomTree(const Graph& graph, Tree * tree, bool shouldAlloc)
{
    if (shouldAlloc)
    {
        tree = new Tree(graph.nVertices() + 1);
    }
    unsigned root = tree->getRootVertex();
    unsigned vertexRoot = std::numeric_limits<unsigned>::max();

    // TODO: imporve this search
    // Find a leaf
#if 0    
    for (unsigned u = 0; u < graph.nVertices(); u++)
    {
        unsigned degree = 0;

        for (unsigned v = 0; v < graph.nVertices(); v++)
        {
            if (graph.isConnected(u, v) == true)
            {
                degree++;
            }
        }

        if (degree == 1)
        {
            vertexRoot = u;
            break;
        }
    }
#else
    for (unsigned u = 0; u < graph.nVertices(); u++)
    {
        if (graph.getVertexDegree(u) == 1)
        {
            vertexRoot = u;
            break;        
        }
    }
#endif

    assert(vertexRoot != std::numeric_limits<unsigned>::max());

    tree->addConnection(root, vertexRoot + 1);

    std::vector<bool> visited(graph.nVertices(), false);
    std::queue<unsigned> queue;

    queue.push(vertexRoot);
    visited[vertexRoot] = true;

    while (not queue.empty())
    {
        unsigned vertex = queue.front();
        queue.pop();

        for (unsigned u = 0; u < graph.nVertices(); u++)
        {
            if (graph.isConnected(vertex, u) and not visited[u])
            {
                queue.push(u);
                visited[u] = true;

                tree->addConnection(vertex + 1, u + 1);
            }
        }
    }

    // we copy the weigths
    tree->setVertexWeight(root, 0);
    for (unsigned v = 0; v < graph.nVertices(); v++)
    {
        tree->setVertexWeight(v + 1, graph.getVertexWeight(v));
    }
}
