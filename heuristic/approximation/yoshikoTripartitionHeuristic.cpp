//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#include "yoshikoTripartitionHeuristic.h"
#include "chlebikovaApproximation.h"
#include "log.h"

#include <algorithm>

Partition yoshikoTripartitionHeuristic(Graph& graph, unsigned D)
{
    unsigned W = graph.totalWeight();
    unsigned weightToAdd = W/D;
    
    Partition best;
    
    log(LG_DEBUG, "will use %d/%d = %d", W, D, W/D);
    
    for (unsigned v = 0; v < graph.nVertices(); v++)
    {
        graph.setVertexWeight(v, graph.getVertexWeight(v) + weightToAdd);
        
        Partition bipart = chlebikovaApproximation(graph);
        
        graph.setVertexWeight(v, graph.getVertexWeight(v) - weightToAdd);
        
        log(LG_DEBUG, "vertex %d(+%d) -> chlebikovaApproximation: %d %d", v, weightToAdd, bipart.lightestWeight(), bipart.heaviestWeight());

        std::vector<PartitionComponent> components = bipart.getAllComponents();
        unsigned comV, semV;
        if (components[0].vertices.find(v) != components[0].vertices.end())
        {
            components[0].weight -= weightToAdd;
            comV = 0;
            semV = 1;
        }
        if (components[1].vertices.find(v) != components[1].vertices.end())
        {
            components[1].weight -= weightToAdd;
            comV = 1;
            semV = 0;
        }
        bipart = components;
 
        std::vector<unsigned> vertexMapping;
        Graph subg = graph.getSubgraph(bipart.component(semV).vertices, vertexMapping);
        
        subg.computeArticulationPoints();
        
        Partition bipartsg = chlebikovaApproximation(subg);

        log(LG_DEBUG, "vertex %d(+%d) -> chlebikovaApproximationSG(%d, %d): %d %d",
                     v, weightToAdd, subg.nVertices(), bipart.component(semV).vertices.size(), bipartsg.lightestWeight(), bipartsg.heaviestWeight());

        log(LG_DEBUG, "vertex %d(+%d) -> %d %d %d", v, weightToAdd, bipart.component(comV).weight, bipartsg.lightestWeight(), bipartsg.heaviestWeight());

        unsigned lweight = std::min(bipart.component(comV).weight, bipartsg.lightestWeight());
        
        // log(LG_INFO, "vertex %d -> %d %d %d", bipart.lightestWeight(), bipartsg.lightestWeight(), bipartsg.heaviestWeight());
        
        if (lweight > best.lightestWeight())
        {
            log(LG_DEBUG, "best found: %d %d %d (sg.w %d)", bipart.component(comV).weight, bipartsg.lightestWeight(), bipartsg.heaviestWeight(), subg.totalWeight());
            std::vector<PartitionComponent> components(3);

            lognonl(LG_DEBUG, "  p1 %d[%d]:", bipartsg.component(0).vertices.size(), bipartsg.component(0).weight);
            for (std::set<unsigned>::const_iterator i = bipartsg.component(0).vertices.begin(); i != bipartsg.component(0).vertices.end(); i++)
                    lognonl(LG_DEBUG, " %d", *i);
            log(LG_DEBUG, "");

            lognonl(LG_DEBUG, "  p1 %d[%d]:", bipartsg.component(1).vertices.size(), bipartsg.component(1).weight);
            for (std::set<unsigned>::const_iterator i = bipartsg.component(1).vertices.begin(); i != bipartsg.component(1).vertices.end(); i++)
                    lognonl(LG_DEBUG, " %d", *i);
            log(LG_DEBUG, "");

            components[0] = bipart.component(comV);
            components[1].weight = bipartsg.lightestWeight();
            components[2].weight = bipartsg.heaviestWeight();
            
            for (std::set<unsigned>::const_iterator i = bipartsg.lightest().vertices.begin(); i != bipartsg.lightest().vertices.end(); i++)
            {
                components[1].addVertex(vertexMapping[*i]);
            }
            
	    	for (std::set<unsigned>::const_iterator i = bipartsg.heaviest().vertices.begin(); i != bipartsg.heaviest().vertices.end(); i++)
            {
                components[2].addVertex(vertexMapping[*i]);
            }
            
            best = components;
        }
    }
    
    return best;
}
