//
// Copyright (C) 2005-2006 Renato Lucindo,
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#include "pttest.h"
#include "graph.h"
#include "uf.h"
#include "random.h"
#include "generators.h"
#include <cstdlib>
#include <cassert>
#include <vector>
#include <queue>

#if 0
void generateRandomTree(Tree& tree)
{
    Random random;
    unsigned nVertices = tree.getVertexCount() - 1;
    unsigned root = tree.getRootVertex();

    // Inicialization
    for (unsigned v = 0; v < nVertices + 1; v++)
    {
        tree.setVertexWeight(v, random.rand(1, 30) + 2);
    }

    tree.setVertexWeight(root, 0);

    for (unsigned v = 0; v < nVertices + 1; v++)
    {
        log(LG_DEBUG, "weight of %u is %u", v, tree.getVertexWeight(v));
    }

    Graph graph(nVertices);
    UF uf(nVertices);

    unsigned nEdges = 0;

    while (nEdges != nVertices - 1)
    {
        // we need to connect the graph without generating cycles
        unsigned u = random.rand(nVertices);
        unsigned v = random.rand(nVertices);

        if (uf.findElement(u) != uf.findElement(v))
        {
            // can connect
            graph.addConnection(u, v);
            nEdges++;

            uf.unionElements(u, v);
        }
    }

    unsigned vertexRoot = std::numeric_limits<unsigned>::max();

    // Find a leaf
    for (unsigned u = 0; u < nVertices; u++)
    {
        unsigned degree = 0;

        for (unsigned v = 0; v < nVertices; v++)
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

    assert(vertexRoot != std::numeric_limits<unsigned>::max());

    tree.addConnection(root, vertexRoot + 1);

    std::vector<bool> visited(nVertices, false);
    std::queue<unsigned> queue;

    queue.push(vertexRoot);
    visited[vertexRoot] = true;

    while (not queue.empty())
    {
        unsigned vertex = queue.front();
        queue.pop();

        for (unsigned u = 0; u < nVertices; u++)
        {
            if (graph.isConnected(vertex, u) and not visited[u])
            {
                queue.push(u);
                visited[u] = true;
                
                tree.addConnection(vertex + 1, u + 1);
            }
        }
    }
}
#endif

double percent(int fraction, int total)
{
    return (fraction * 100.0) / (double) total;
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        log(LG_ERROR, "usage: %s <#vertices> <rounds> <partition size ...>", argv[0]);
        return -1;
    }
    Logger::instance()->setPriorityMask(LG_DEBUG | LG_INFO | LG_NOTICE | LG_ERROR | LG_CRITICAL);
    int nVertices = atoi(argv[1]);
    int nRounds   = atoi(argv[2]);
    
    for (int r = 0; r < nRounds; r++)
    {
        log(LG_INFO, "Round %d:", r);
	Tree tree(nVertices + 1);
	//generateRandomTree(tree);
	Graph graph = generateRandomTree(nVertices, MAX_VERTEX_WEIGHT * nVertices);
	getTreeFromRandomTree(graph, &tree, false);

        tree.writeToFile("tree.graphml");

	for (int arg = 3; arg < argc; ++arg)
	{
	    Timer timer;
	    int nComp = atoi(argv[arg]);
	    unsigned iterations;
    
	    std::vector<PartitionComponent> partitonComponents;
	    timer.start();
	    partitionByPerlSchach(tree, nComp, partitonComponents, &iterations);
	    timer.stop();
    
            tree.writeToFile("treepart.graphml", partitonComponents);
            
            unsigned hComp = 0;
            unsigned lComp = std::numeric_limits<unsigned>::max();
            for (unsigned p = 0; p < partitonComponents.size(); ++p)
            {
                if (partitonComponents[p].weight > hComp) hComp = partitonComponents[p].weight;
                if (partitonComponents[p].weight < lComp) lComp = partitonComponents[p].weight;
            }
            
	    if (nComp != (int)partitonComponents.size())
	    {
		lognonl(LG_INFO, "error (needed %d-partition) on ", nComp);
	    }
	    log(LG_INFO, "test %d: tree with %u nodes, %d-partition found in %5.6f secs after %u iterations ([%u %u] %.2f%% inbalance)",
		    arg - 2, tree.getVertexCount(), partitonComponents.size(), timer.getElapsedTime(), iterations,
		    lComp, hComp, percent(hComp - lComp, hComp));
		    
            LogPriority priority;
            if (percent(hComp - lComp, hComp) > 50.0)
            {
                priority = LG_INFO;
            }
            else
            {
                priority = LG_DEBUG;
            }

	    for (unsigned p = 0; p < partitonComponents.size(); ++p)
	    {
		std::set<unsigned>& vertices = partitonComponents[p].vertices;
		lognonl(priority, " comp %u: weight %3u, %2u nodes [", p + 1, partitonComponents[p].weight, vertices.size());
    
		for (std::set<unsigned>::const_iterator c = vertices.begin(); c != vertices.end(); c++)
		{
		    if (c != vertices.begin())
		    {
			lognonl(priority, ", ");
		    }
		    lognonl(priority, "%2u|%2u", *c, tree.getVertexWeight(*c));
		}
		log(priority, "]");
	    }
	    log(priority, "");
	}
    }

    return 0;
}
