//
// Copyright (C) 2005-2006 Renato Lucindo,
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#include "graph.h"
#include "generators.h"
#include "log.h"
#include "timers.h"
#include "random.h"        

#include "randSpanningTree.h"
#include "improvedRandSpanningTree.h"
#include "cassioRandSpanningTree.h"
#include "mixedImprovedRandSpanningTree.h"                
        
#include <getopt.h>

double calcRatio(int a, int b)
{
    return (double)a / (double) b;
}

void usage(const char * progname)
{
    log(LG_ERROR, "usage: %s -r <runs> -s <# vertices start> [-e <# vertices end>] [-t <vertex increment>]", progname);
    
    exit(1);
}

int main(int argc, char **argv)
{
    unsigned vs, ve, runs, step = 10;
    
    vs = ve = 0;
    int c = 0;
    while ((c = getopt(argc, argv, "r:s:e:t:")) != -1)
    {
        switch (c)
        {
            case 'r':
                runs = atoi(optarg);
                break;
            case 's':
                vs = atoi(optarg);
                break;
            case 'e':
                ve = atoi(optarg);
                break;
            case 't':
                step = atoi(optarg);
                break;
	    case '?':
	    default:
                usage(argv[0]);
                break;
        }
    }

    if (vs == 0 or runs == 0)
    {
        usage(argv[0]);
    }

    if (ve == 0)
    {
        ve = vs;
    }

    Random random;

    unsigned rp, ip, cp, mp, tot;
    
    rp = ip = cp = mp = tot = 0;
    
    for (unsigned density = 30; density < 100; density += 30)
    {
	for (unsigned nVertices = vs; nVertices <= ve; nVertices += step)
	{	    
	    for (unsigned r = 0; r < runs; r++)
	    {
		Graph graph = generateRandomGraph(nVertices, density);
		
		graph.computeArticulationPoints();

                double totalTime = 0.025 * (double)graph.nEdges();
		// totalTime += 1.0;

                std::vector<unsigned> qs;
                
                qs.push_back(2);
                qs.push_back(nVertices/4);
                qs.push_back(nVertices/2);
                qs.push_back((3*nVertices)/4);
                
                for (unsigned q = 0; q < qs.size(); q++)
                {
                    log(LG_INFO, "%d %d %d for %5.3f", nVertices, density, qs[q], totalTime);
                    Graph tree(graph.nVertices());
		    Partition rpartition = randSpanningTreeHeuristic(graph, qs[q], nVertices*nVertices, tree, totalTime);
		    Partition ipartition = improvedRandSpanningTreeHeuristic(graph, qs[q], false, totalTime);
		    Partition cpartition = cassioRandSpanningTreeHeuristic(graph, qs[q], totalTime);
		    Partition mpartition = mixedImprovedRandSpanningTreeHeuristic(graph, qs[q], nVertices * nVertices, totalTime);
		    
		    unsigned b = rpartition.lightestWeight();
		    if (b < ipartition.lightestWeight()) b = ipartition.lightestWeight();
		    if (b < cpartition.lightestWeight()) b = cpartition.lightestWeight();
		    if (b < mpartition.lightestWeight()) b = mpartition.lightestWeight();
		    
		    if (b == rpartition.lightestWeight()) rp++;
		    if (b == ipartition.lightestWeight()) ip++;
		    if (b == cpartition.lightestWeight()) cp++;
		    if (b == mpartition.lightestWeight()) mp++;
		    
		    tot++;
                }   
	    }   
	}
    }    
    
    log(LG_INFO, "%d -> rp %d ip %d cp %d mp %d", tot, rp, ip, cp, mp);
    
    return 0;
}
