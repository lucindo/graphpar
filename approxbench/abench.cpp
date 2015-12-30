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
        
#include <getopt.h>

double calcRatio(int a, int b)
{
    return (double)a / (double) b;
}

double calcTheoricRatio(int W, int w3)
{
    double t = (double) W / (double) w3;
    double r = 0.;
    
    assert(t >= 3.);
    
    if (t < 4)
    {
        r = (double) (t - 1.) / (double) (2 * t - 4);
    }
    else
    {
        r = (double) (t - 1.) / (double) (t);
    }    
    
    return r;
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

    for (unsigned density = 30; density < 100; density += 30)
    {
	log(LG_INFO, "density & vertices & runs & approx & theoric");
	for (unsigned nVertices = vs; nVertices <= ve; nVertices += step)
	{
	    double aratio = 0.;
	    double tratio = 0.;
	    
	    for (unsigned r = 0; r < runs; r++)
	    {
		int optimalValue = random.rand(nVertices, 5 * nVertices);
		Graph graph = generateBalancedGraph(nVertices, 2, 2*optimalValue, density);
		
		graph.computeArticulationPoints();

		Partition apartition = chlebikovaApproximation(graph);

		aratio += calcRatio(apartition.lightestWeight(), optimalValue);
		tratio += calcTheoricRatio(graph.totalWeight(), graph.heaviest(3));
	    }

	    aratio /= (double) runs;
	    tratio /= (double) runs;

	    log(LG_INFO, "   %d   &  %5d   &  %2d  &  %5.3f &  %5.3f ", density, nVertices, runs, aratio, tratio);
	}
    }
        
    return 0;
}
