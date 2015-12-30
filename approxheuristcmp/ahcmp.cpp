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

#include "chlebikovaApproximation.h"
#include "mixedImprovedRandSpanningTree.h"                
        
#include <getopt.h>

double calcRatio(int a, int b)
{
    return (double)a / (double) b;
}

void usage(const char * progname)
{
    log(LG_ERROR, "usage: %s [-b] -r <runs> -s <# vertices start> [-e <# vertices end>] [-t <vertex increment>]", progname);
    
    exit(1);
}

int main(int argc, char **argv)
{
    unsigned vs, ve, runs, step = 10;
    bool blindTest = false;
    
    vs = ve = 0;
    int c = 0;
    while ((c = getopt(argc, argv, "r:s:e:t:b")) != -1)
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
            case 'b':
                blindTest = true;
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

    if (not blindTest)
    {
	for (unsigned density = 30; density < 100; density += 30)
	{
	    log(LG_INFO, "density & vertices & runs & awins & approx & hwins & heurist & tie");
	    for (unsigned nVertices = vs; nVertices <= ve; nVertices += step)
	    {
		int hwins = 0, awins = 0, tie = 0;
		double aratio = 0.;
		double hratio = 0.;
		
		for (unsigned r = 0; r < runs; r++)
		{
		    int optimalValue = random.rand(nVertices, 5 * nVertices);
		    Graph graph = generateBalancedGraph(nVertices, 2, 2*optimalValue, density);
		    
		    graph.computeArticulationPoints();
    
		    Partition apartition = chlebikovaApproximation(graph);
		    Partition hpartition = mixedImprovedRandSpanningTreeHeuristic(graph, 2, graph.nVertices() * graph.nVertices());
    
		    aratio += calcRatio(apartition.lightestWeight(), optimalValue);
		    hratio += calcRatio(hpartition.lightestWeight(), optimalValue);
    
		    if (apartition.lightestWeight() > hpartition.lightestWeight())
		    {
			awins++;
		    }
		    else if (hpartition.lightestWeight() > apartition.lightestWeight())
		    {
			hwins++;
		    }
		    else
		    {
			tie++;
		    }
		}
    
		aratio /= (double) runs;
		hratio /= (double) runs;
    
		log(LG_INFO, "   %d   &  %5d   &  %2d  &  %2d   &  %5.3f &  %2d   &  %5.3f  & %2d", density, nVertices, runs, awins, aratio, hwins, hratio, tie);
	    }
	}
    }
    else
    {
	for (unsigned density = 30; density < 100; density += 30)
	{
	    log(LG_INFO, "density & vertices & runs & awins & hwins & tie & adiff & hdiff");
	    for (unsigned nVertices = vs; nVertices <= ve; nVertices += step)
	    {
		int hwins = 0, awins = 0, tie = 0;
		double adiff = 0.;
		double hdiff = 0.;
		
		for (unsigned r = 0; r < runs; r++)
		{
		    Graph graph = generateRandomGraph(nVertices, density);
		    
		    graph.computeArticulationPoints();
    
		    Partition apartition = chlebikovaApproximation(graph);
		    Partition hpartition = mixedImprovedRandSpanningTreeHeuristic(graph, 2, graph.nVertices() * graph.nVertices());
    
		    if (apartition.lightestWeight() > hpartition.lightestWeight())
		    {
			awins++;
			hdiff += (double)(apartition.lightestWeight() - hpartition.lightestWeight()) / (double) graph.totalWeight();
		    }
		    else if (hpartition.lightestWeight() > apartition.lightestWeight())
		    {
			hwins++;
			adiff += (double)(hpartition.lightestWeight() - apartition.lightestWeight()) / (double) graph.totalWeight();
		    }
		    else
		    {
			tie++;
		    }
		}
                
		if (awins) hdiff /= (double) awins;
                if (hwins) adiff /= (double) hwins;
    
		log(LG_INFO, "   %d   &  %5d   &  %2d  &  %2d   &  %2d   & %2d  & %5.3f & %5.3f ", density, nVertices, runs, awins, hwins, tie, adiff, hdiff);
	    }
	}    
    }
    
    return 0;
}
