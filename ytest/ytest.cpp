//
// Copyright (C) 2005-2006 Renato Lucindo,
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#include "yoshikoTripartitionHeuristic.h"
#include "graph.h"
#include "generators.h"
#include "log.h"
#include "timers.h"
#include "random.h"

#include <getopt.h>

double calcRatio(int a, int b)
{
    return (double)a / (double) b;
}

void showBestSolution(const Graph& graph, Partition& partition, unsigned k, const Timer& timer, int bestKown = -1)
{
    if (bestKown != -1)
    {
        log(LG_INFO,  "graph with %u vertices and %u edges,"
            " lightest known cluster weight %d, best %d-partition found using W/%d (%5.4f secs):",
            graph.nVertices(), graph.nEdges(),
            bestKown, partition.nComponents(), k, timer.getElapsedTime());
            
	log(LG_INFO, " ratio: %5.3f", calcRatio(partition.lightestWeight(), bestKown));
    }
    else
    {
        log(LG_INFO,  "graph with %u nodes and %u edges, best %d-partition using W/%d found (%5.4f secs):",
            graph.nVertices(), graph.nEdges(), partition.nComponents(), k, timer.getElapsedTime());
    }

    for (unsigned p = 0; p < partition.nComponents(); ++p)
    {
        std::set
            <unsigned>& vertices = partition.component(p).vertices;
        lognonl(LG_INFO, " cluster %u: weight %3u, %2u nodes [", p + 1, partition.component(p).weight, vertices.size());

        for (std::set
                    <unsigned>::const_iterator c = vertices.begin(); c != vertices.end(); c++)
            {
                if (c != vertices.begin())
                {
                    lognonl(LG_INFO, ", ");
                }
                lognonl(LG_INFO, "%2u", (*c) + 1);//, graph.getVertexWeight(*c));
            }
        log(LG_INFO, "]");
    }
}

void usage(const char * progname)
{
    log(LG_ERROR, "usage: %s [<config options] <input options>", progname);
    log(LG_ERROR, "  input options:");
    log(LG_ERROR, "        to read a graph from XML:");
    log(LG_ERROR, "          -i <input file> (path to XML file)");
    log(LG_ERROR, "        to generate a random graph you must provide:");
    log(LG_ERROR, "          -v <vertices> (# of vertices)");
    log(LG_ERROR, "          -d <density> (graph density in percent)");
    log(LG_ERROR, "          -w <weight> (graph total weight)");
    log(LG_ERROR, "  config options:");
    log(LG_ERROR, "        -D (enables debug mode)");
    log(LG_ERROR, "        -k <k> (set fraction W/k) [default: 3]");
    
    exit(1);
}

int old_main(int argc, char **argv)
{
    unsigned nVertices, totalWeight, density;
    bool debugmode = false;
    const char * filename = 0;
    unsigned k = 3;

    nVertices = totalWeight = density = 0;
    int c = 0;
    while ((c = getopt(argc, argv, "Dw:d:v:i:k:")) != -1)
    {
        switch (c)
        {
	    case 'D':
                debugmode = true;
                break;
	    case 'w':
                totalWeight = atoi(optarg);
                break;
	    case 'd':
                density = atoi(optarg);
                break;
	    case 'v':
                nVertices = atoi(optarg);
                break;
	    case 'i':
                filename = optarg;
                break;
	    case 'k':
                k = atoi(optarg);
                break;                                
	    case '?':
	    default:
                usage(argv[0]);
                break;
        }
    }

    if (filename == 0 and (nVertices == 0 or totalWeight == 0 or density == 0))
    {
        usage(argv[0]);
    }

    if (debugmode)
    {
        Logger::instance()->setPriorityMask(LG_DEBUG | LG_INFO | LG_NOTICE | LG_ERROR | LG_CRITICAL);
    }

    int clusters; // dummy
    Graph graph = (filename != 0) ? readGraphFromXML(filename, clusters) :
                                    generateBalancedGraph(nVertices, 2, totalWeight, density);

    graph.computeArticulationPoints();

    Timer timer;
    timer.start();
    Partition partition = yoshikoTripartitionHeuristic(graph, 3);
    timer.stop();

    showBestSolution(graph, partition, 3, timer, filename ? (int)-1 : totalWeight/3);

    Timer timer2;
    timer2.start();
    Partition partition2 = yoshikoTripartitionHeuristic(graph, 4);
    timer2.stop();

    showBestSolution(graph, partition2, 4, timer2, filename ? (int)-1 : totalWeight/3);

    
    return 0;
}

void usage2(const char * progname)
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
        usage2(argv[0]);
    }

    if (ve == 0)
    {
        ve = vs;
    }

    Random random;

    for (unsigned density = 30; density < 100; density += 30)
    {
	log(LG_INFO, "density & vertices & runs &   W/3  &   W/4 ");
	for (unsigned nVertices = vs; nVertices <= ve; nVertices += step)
	{
	    double ratio3 = 0.;
	    double ratio4 = 0.;
	    
	    for (unsigned r = 0; r < runs; r++)
	    {
		int optimalValue = random.rand(nVertices, 5 * nVertices);
		Graph graph = generateBalancedGraph(nVertices, 3, 3*optimalValue, density);
		
		graph.computeArticulationPoints();

		Partition partition3 = yoshikoTripartitionHeuristic(graph, 3);
		Partition partition4 = yoshikoTripartitionHeuristic(graph, 4);

		ratio3 += calcRatio(partition3.lightestWeight(), optimalValue);
		ratio4 += calcRatio(partition4.lightestWeight(), optimalValue);
	    }

	    ratio3 /= (double) runs;
	    ratio4 /= (double) runs;

	    log(LG_INFO, "   %d   &  %5d   &  %2d  &  %5.3f &  %5.3f ", density, nVertices, runs, ratio3, ratio4);
	}
    }
        
    return 0;
}

