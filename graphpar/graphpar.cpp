//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#include "graphpar.h"

#include "log.h"
#include "generators.h"
#include "partition.h"
#include "graph.h"
#include "random.h"
#include "timers.h"
#include "toAmpl.h"
#include "graphml.h"

#include "randSpanningTree.h"
#include "improvedRandSpanningTree.h"
#include "mixedImprovedRandSpanningTree.h"
#include "cassioRandSpanningTree.h"
#include "chlebikovaApproximation.h"
#include "yoshikoTripartitionHeuristic.h"

#include <fstream>

#include <getopt.h>

void usage(const char * progname)
{
    log(LG_ERROR, "usage: %s <graph options> [output options] [-h <heuristic number>] [heuristic options]", progname);
    log(LG_ERROR, "  graph options:");
    log(LG_ERROR, "        to read a graph from XML:");
    log(LG_ERROR, "          -i <input file> (path to XML file)");    
    log(LG_ERROR, "        to generate a random graph you must provide:");
    log(LG_ERROR, "          -v <vertices> (# of vertices)");
    log(LG_ERROR, "          -d <density> (graph density in percent)");
    log(LG_ERROR, "          -c <clusters> (# of cluters in partition)");
    log(LG_ERROR, "          [-w <weight>] (weight of each cluster - optional)");    
    log(LG_ERROR, "  output options:");
    log(LG_ERROR, "        -I prints ampl model with inital solution");
    log(LG_ERROR, "        -C prints easy to parse solution");
    log(LG_ERROR, "        -y <file> (prints graphml output for yEd)");
    log(LG_ERROR, "  heuristc numbers:");
    log(LG_ERROR, "        0 = mixed heuristic               O(c^2.n^3.t + c^2.n.m.t)  [default]");
    log(LG_ERROR, "        1 = random heuristic              O(c^2.n^2.t)");
    log(LG_ERROR, "        2 = fc all 1-trees                O(c^2.n^3.m)");
    log(LG_ERROR, "        3 = fc simple                     O(c^2.n^2 + c^2.m.n)");
    log(LG_ERROR, "        4 = Chlebikova 3/4-approximation  O(n^3.c^2)                [only for 2-partitions]");
    log(LG_ERROR, "        5 = LW 3-partition heutistic O(n^4.c^2)                     [only for 3-partitions]");
    log(LG_ERROR, "    n = # of vertices");
    log(LG_ERROR, "    m = # of edges");
    log(LG_ERROR, "    c = # of clusters");
    log(LG_ERROR, "    t = # of tries");
    log(LG_ERROR, "  heuristc options:");
    log(LG_ERROR, "        -T <time> (max execution time in seconds)");
    log(LG_ERROR, "        -t <tries> (set try option of heuristics 0 and 1)           [default = n^2]");
    log(LG_ERROR, "        -r (allow random retrys for heuristic 3)                    [default = false]");


    
    exit(1);
}

const char * heuristicNames[] = {
    "mixed heuristic",
    "random heuristic",
    "fc all 1-trees",
    "fc simple",
    "Chlebikova 3/4-approximation",
    "Yoshiko 3-partition heutistic",
};

void displaySolution(const Graph& graph, Partition& partition, int clusters, const Timer& timer, int h, const char * prefix)
{
    log(LG_INFO, "%sgraph with %u vertices, %u edges. %d-partition found using %s (%5.4f secs):", 
        prefix, graph.nVertices(), graph.nEdges(), clusters, heuristicNames[h], timer.getElapsedTime());
        
    log(LG_INFO, "%s lightest cluster %u, heaviest cluster %u", prefix, partition.lightestWeight(), partition.heaviestWeight());
            
    partition.sort();
            
    for (unsigned p = 0; p < partition.nComponents(); ++p)
    {
        std::set<unsigned>& vertices = partition.component(p).vertices;
	lognonl(LG_INFO, "%s  cluster %2u: weight %3u, %2u nodes [", prefix, p + 1, partition.component(p).weight, vertices.size());
    
	for (std::set<unsigned>::const_iterator c = vertices.begin(); c != vertices.end(); c++)
	{
	    if (c != vertices.begin())
	    {
		lognonl(LG_INFO, ", ");
	    }
	    lognonl(LG_INFO, "%2u/%2u", (*c) + 1 , graph.getVertexWeight(*c));
	}
	log(LG_INFO, "]");
    }
}

void displayEasySolution(const Graph& graph, Partition& partition)
{
    log(LG_INFO, "%d", partition.nComponents());
    for (unsigned p = 0; p < partition.nComponents(); ++p)
    {
        std::set<unsigned>& vertices = partition.component(p).vertices;
	lognonl(LG_INFO, "%u %u", partition.component(p).weight, vertices.size());
    
	for (std::set<unsigned>::const_iterator c = vertices.begin(); c != vertices.end(); c++)
	{
	    lognonl(LG_INFO, " %u", (*c) + 1);
	}
	log(LG_INFO, "");
    }
}

void writeSolution(const char * fileName, const Graph& graph, Partition& partition)
{
    std::ofstream outfile(fileName);
    
    if (outfile.good())
    {
        GraphmlWriter writer(&outfile);
        writer.isTree(false);
        
        for (unsigned c = 0; c < partition.nComponents(); ++c)
        {
            std::set<unsigned>& vertices = partition.component(c).vertices;
            for (std::set<unsigned>::const_iterator it = vertices.begin(); it != vertices.end(); it++)
            {
                unsigned v = *it;
                char label[256] = { 0, };
                
                sprintf(label, "%d/%d", v + 1, graph.getVertexWeight(v));
                
                writer.printNode(v + 1, c + 5, label);
            }
        }      
        for (unsigned u = 0; u < graph.nVertices(); u++)
        {
            for (unsigned v = u + 1; v < graph.nVertices(); v++)
            {
                if (graph.isConnected(u, v))
                {
                    writer.printEdge(u + 1, v + 1, false);
                }
            }
	}
    }
}


int main(int argc, char **argv)
{
    int nVertices = 0;
    int clusters  = 0;
    int density   = 0;
    int clusterWeight = 0;
    const char * filename = 0;
    const char * outfilename = 0;
    bool dumpAmpl = false;
    bool easyParse = false;
    int heuristic = 0;
    double maxtime = 0.0;
    int tries = 0;
    bool allowRand = false;
    
    int c = 0;
    while ((c = getopt(argc, argv, "ICrt:T:y:i:c:w:d:v:h:")) != -1)
    {
        switch (c)
        {
            case 'I':
               dumpAmpl = true;
               break;
            case 'C':
               easyParse = true;
               break;
            case 'r':
               allowRand = true;
               break;               
            case 'T':
                maxtime = atof(optarg);
                break;                           
            case 'y':
                outfilename = optarg;
                break;
            case 't':
                tries = atoi(optarg);
                break;                       
            case 'i':
                filename = optarg;
                break;
            case 'c':
                clusters = atoi(optarg);
                break;
            case 'w':
                clusterWeight = atoi(optarg);
                break;
            case 'd':
                density = atoi(optarg);
                break;
            case 'v':
                nVertices = atoi(optarg);
                break;
            case 'h':
                heuristic = atoi(optarg);
                break;                
            case '?':
            default:
                usage(argv[0]);
                break;
        };
    }

    if (filename == 0  and (nVertices == 0 or clusters == 0 or density == 0)) usage(argv[0]);

    if (filename == 0 and clusterWeight == 0)
    {
        Random random;
        clusterWeight = random.rand(nVertices, 10 * nVertices);
    }
    
    if (heuristic < 0 or heuristic > 5) usage(argv[0]);
    
    Graph graph = (filename != 0) ? readGraphFromXML(filename, clusters) : 
                                    generateBalancedGraph(nVertices, clusters, clusterWeight*clusters, density);

    if (tries == 0)
    {
        tries = graph.nVertices() * graph.nVertices();
    }

    if (heuristic == 4) // Chlebikova 3/4-approximation
    {
        if (clusters != 2)
        {
            log(LG_ERROR, "Chlebikova 3/4-approximation only works to find bipartitions and you asked for a %d-partition", clusters);
            exit(2);
        }
        graph.computeArticulationPoints();
    }
    if (heuristic == 5) // Yoshiko 3-partition heutistic
    {
        if (clusters != 3)
        {
            log(LG_ERROR, "Yoshiko 3-partition heutistic only works to find 3-partitions and you asked for a %d-partition", clusters);
            exit(3);
        }
        graph.computeArticulationPoints();
    }


    Partition partition;
    Timer timer;
    
    timer.start();
    switch (heuristic)
    {
        case 0: partition = mixedImprovedRandSpanningTreeHeuristic(graph, clusters, tries, maxtime); break;
        case 1: 
        {
            Graph tree(graph.nVertices());
            partition = randSpanningTreeHeuristic(graph, clusters, tries, tree, maxtime);
            break;
	}
	case 2: partition = cassioRandSpanningTreeHeuristic(graph, clusters, maxtime); break;	
	case 3: partition = improvedRandSpanningTreeHeuristic(graph, clusters, allowRand, maxtime); break;
	case 4: partition = chlebikovaApproximation(graph); break;
	case 5: partition = yoshikoTripartitionHeuristic(graph, 3); break;
	default: log(LG_ERROR, "bug: cannot execute a heuristc [%d]", heuristic); exit(4);
    };
    timer.stop();
    
    if (not easyParse)
    {
	const char * out_prefix = "";
	if (dumpAmpl)
	{
	    toAmpl(graph, partition, clusters);
	    out_prefix = "# ";
	}
	
	displaySolution(graph, partition, clusters, timer, heuristic, out_prefix);
    }
    else
    {
        displayEasySolution(graph, partition);
    }
        
    if (outfilename)
    {
        writeSolution(outfilename, graph, partition);
    }

    return 0;
}
