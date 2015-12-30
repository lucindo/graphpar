//
// Copyright (C) 2005-2006 Renato Lucindo,
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#include "chlebikovaApproximation.h"
#include "graph.h"
#include "generators.h"
#include "log.h"
#include "timers.h"

#include <getopt.h>

void showBestSolution(const Graph& graph, Partition& partition, unsigned p, unsigned k, const Timer& timer, int bestKown = -1)
{
    if (bestKown != -1)
    {
        log(LG_INFO,  "graph with %u vertices and %u edges,"
            " lightest known cluster weight %d, best %d/%d %d-partition found (%5.4f secs):",
            graph.nVertices(), graph.nEdges(),
            bestKown, p, k, partition.nComponents(), timer.getElapsedTime());
    }
    else
    {
        log(LG_INFO,  "graph with %u nodes and %u edges, best %d/%d %d-partition found (%5.4f secs):",
            graph.nVertices(), graph.nEdges(), p, k, partition.nComponents(), timer.getElapsedTime());
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
    log(LG_ERROR, "        -2 (execute only if graph is biconnected)");
    log(LG_ERROR, "        -p <p> (set fraction p/k) [default: 1]");
    log(LG_ERROR, "        -k <k> (set fraction p/k) [default: 2]");
    
    exit(1);
}

int main(int argc, char **argv)
{
    unsigned nVertices, totalWeight, density;
    bool debugmode = false;
    bool check2connected = false;
    const char * filename = 0;
    unsigned p = 1;
    unsigned k = 2;

    nVertices = totalWeight = density = 0;
    int c = 0;
    while ((c = getopt(argc, argv, "D2w:d:v:i:p:k:")) != -1)
    {
        switch (c)
        {
                case 'D':
                debugmode = true;
                break;
                case '2':
                check2connected = true;
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
                case 'p':
                p = atoi(optarg);
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

    if (check2connected and graph.isBiconnected() == false)
    {
        for (unsigned v = 0; v < graph.nVertices(); v++)
            if (graph.isArticulationPoint(v))
            {
                log(LG_DEBUG, "vertex %u is an articulation point (degree %u)", v, graph.getVertexDegree(v));
            }

        log(LG_ERROR, "generated graph is not 2-connected, leaving");
        exit(2);
    }

    Timer timer;
    timer.start();
    // Partition partition = chlebikovaBlockApproximation(graph);
    Partition partition = chlebikovaApproximation(graph, p, k);
    timer.stop();

    showBestSolution(graph, partition, p, k, timer, filename ? (int)-1 : totalWeight/2);

    return 0;
}
