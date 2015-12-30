//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#include "generators.h"
#include "random.h"
#include "uf.h"
#include "log.h"

#include <cstdlib>
#include <cassert>
#include <vector>
#include <queue>

// Generates a tree with nVertices with random weights (sum = totalWeight)
Graph generateRandomTree(unsigned nVertices, unsigned totalWeight)
{
    Random random;
    UF uf(nVertices);
    Graph graph(nVertices);

    log(LG_DEBUG, "called to build a tree with %d vertices and weight of %d", nVertices, totalWeight);
    
    assert(totalWeight >= nVertices);
    
    unsigned nEdges = 0, weight = 0;

    while (nEdges != nVertices - 1)
    {
        unsigned u = random.rand(nVertices);
        unsigned v = random.rand(nVertices);

        // we need to connect the graph without generating cycles
        if (uf.findElement(u) != uf.findElement(v))
        {
            // can connect
            graph.addConnection(u, v);
            nEdges++;

            uf.unionElements(u, v);
        }
    }

    // TODO: a better distribution
    // now we distribute the weights
#if 1
    for (unsigned v = 0; v < nVertices; v++)
    {
        graph.setVertexWeight(v, 1);
    }
    weight = nVertices;
    while (totalWeight - weight > 0)
    {
        unsigned v = random.rand(1, nVertices) - 1;
        graph.setVertexWeight(v, graph.getVertexWeight(v) + 1);
        weight++;
        log(LG_DEBUG, "weight = %d, totalWeight = %d", weight, totalWeight);
    }
#else    
    for (unsigned v = 0; v < nVertices - 1; v++)
    {
        unsigned wmax = (totalWeight - weight) / (nVertices - 1);
        unsigned w    = random.rand(1, wmax);
        graph.setVertexWeight(v, w);
        weight += w;
        log(LG_DEBUG, "w = %d, weight = %d", w, weight);
    }

    assert(((int)totalWeight - (int)weight) > 0);

    graph.setVertexWeight(nVertices - 1, totalWeight - weight);
#endif

    log(LG_DEBUG, "w = %d, weight = %d, totalWeight = %d", totalWeight - weight, weight, totalWeight);

    return graph;
}


Graph generateBalancedGraph(unsigned nVertices, unsigned nComponents, unsigned totalWeight, unsigned density)
{
    struct
    {
        unsigned size;
        unsigned weight;
    }
    components[nComponents], totalComps;

    log(LG_DEBUG, "called to construt a graph with %u clusters of weight %u", nComponents, totalWeight / nComponents);
    assert(nComponents * (totalWeight / nComponents) >= nVertices);

    totalComps.size = totalComps.weight = 0;
    Random random;
#if 1
    for (unsigned c = 0; c < nComponents; c++)
    {
        components[c].size = 1;
    }
    totalComps.size =  nComponents;
    while (nVertices - totalComps.size > 0)
    {
        unsigned c = random.rand(1, nComponents) - 1;
        if (components[c].size < totalWeight / nComponents)
        {
            components[c].size++;
            totalComps.size++;
        }
    }
#else
    for (unsigned c = 0; c < nComponents - 1; c++)
    {
        unsigned sizeMax   = (nVertices - totalComps.size) / nComponents;
        unsigned weightMax = (totalWeight - totalComps.weight) / nComponents;

        components[c].size = random.rand(1, sizeMax) + 1;
        components[c].weight = random.rand(1, weightMax) + 1;

        totalComps.size += components[c].size;
        totalComps.weight += components[c].weight;
    }
    assert(((int)totalWeight - (int)totalComps.weight) > 0);
    assert(((int)nVertices   - (int)totalComps.size) > 0);

    components[nComponents - 1].size   = nVertices - totalComps.size;
    components[nComponents - 1].weight = totalWeight - totalComps.weight;
#endif

    // We genetate nComponents as Trees
    Graph graph(nVertices);
    unsigned vertexIndexBase = 0;
    for (unsigned c = 0; c < nComponents; c++)
    {
        //
        // HACK: Attention: little hack to generate components with same size
        //
        Graph genTree = generateRandomTree(components[c].size, totalWeight / nComponents);// components[c].weight); // <------

        for (unsigned u = 0; u < genTree.nVertices(); u++)
        {
            for (unsigned v = 0; v < genTree.nVertices(); v++)
            {
                if (genTree.isConnected(u, v))
                {
                    graph.addConnection(vertexIndexBase + u, vertexIndexBase + v);
                }
            }
            graph.setVertexWeight(vertexIndexBase + u, genTree.getVertexWeight(u));
        }

        vertexIndexBase += components[c].size;
    }

    // now we connect the Trees
    vertexIndexBase = 0;
    for (unsigned c = 1; c < nComponents; c++)
    {
        unsigned vertexComp1 = random.rand(0, components[c - 1].size - 1);
        unsigned vertexComp2 = random.rand(0, components[c].size - 1);

        graph.addConnection(vertexIndexBase + vertexComp1, vertexIndexBase + components[c - 1].size + vertexComp2);
        vertexIndexBase += components[c - 1].size;
    }

    unsigned nEdges = graph.nEdges(); // now we have a big tree
    unsigned totalEdges = getTotalEdgesFromDensity(nVertices, density);

    // log(LG_INFO, "%d edges, but %d needed", nEdges, totalEdges);
    while (nEdges < totalEdges)
    {
        unsigned u = random.rand(nVertices);
        unsigned v = random.rand(nVertices);

        //log(LG_DEBUG, "nEdges %d, u %d, v %d, connected %d", nEdges, u, v, graph.isConnected(u, v));
        if (u != v and not graph.isConnected(u, v))
        {
            graph.addConnection(u, v);
            nEdges++;
        }
    }

    return graph;
}

Graph generateRandomGraph(unsigned nVertices, unsigned density, unsigned maxVertexWeight)
{
    Random random;
    
    Graph graph(nVertices);
    
    UF uf(nVertices);
    unsigned nEdges = 0;
    while (nEdges != nVertices - 1)
    {
        unsigned u = random.rand(nVertices);
        unsigned v = random.rand(nVertices);

        // we need to connect the graph without generating cycles
        if (uf.findElement(u) != uf.findElement(v))
        {
            // can connect
            graph.addConnection(u, v);
            nEdges++;

            uf.unionElements(u, v);
        }
    }

    for (unsigned w = 0; w < nVertices; w++)
    {
        graph.setVertexWeight(w, random.rand(1, maxVertexWeight*nVertices));
    }
    
    unsigned totalEdges = getTotalEdgesFromDensity(nVertices, density);
    
    while (nEdges < totalEdges)
    {
	unsigned u = random.rand(nVertices);
        unsigned v = random.rand(nVertices);

        if (u != v and not graph.isConnected(u, v))
        {
            graph.addConnection(u, v);
            nEdges++;
        }
    }
    
    return graph;
}

unsigned getTotalEdgesFromDensity(unsigned nVertices, unsigned density)
{
    // density is in interval 0 - 100
    // 0   = 0;
    // 100 = nVertices * (nVertices - 1) / 2;

    return (nVertices * (nVertices - 1) / 2) * density / 100;
}

