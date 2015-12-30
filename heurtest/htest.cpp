//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#include "htest.h"

#include "graph.h"
#include "generators.h"
#include "random.h"
#include "log.h"
#include "partition.h"
#include "randSpanningTree.h"
#include "improvedRandSpanningTree.h"
#include "cassioRandSpanningTree.h"
#include "graphml.h"
#include "timers.h"

#include <vector>
#include <fstream>
#include <algorithm>
#include <string>

#include <getopt.h>

void showBestSolution(const Graph& graph, Partition& partition, const Timer& timer, int bestKown = -1)
{
    if (bestKown != -1)
    {
	log(LG_INFO,  "graph with %u vertices and %u edges,"
		" lightest known cluster weight %d, best %d-partition found (%5.4f secs):",
		graph.nVertices(), graph.nEdges(),
		bestKown, partition.nComponents(), timer.getElapsedTime());
    }
    else
    {
        log(LG_INFO,  "graph with %u nodes and %u edges, best %d-partition found (%5.4f secs):",
            graph.nVertices(), graph.nEdges(), partition.nComponents(), timer.getElapsedTime());
    }
	    
    for (unsigned p = 0; p < partition.nComponents(); ++p)
    {
        std::set<unsigned>& vertices = partition.component(p).vertices;
	lognonl(LG_INFO, " cluster %u: weight %3u, %2u nodes [", p + 1, partition.component(p).weight, vertices.size());
    
	for (std::set<unsigned>::const_iterator c = vertices.begin(); c != vertices.end(); c++)
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

void writeSolution(const char * fileName, const Graph& graph, Partition& partition, const Graph& tree)
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
                    writer.printEdge(u + 1, v + 1, not tree.isConnected(u, v));
                }
            }
	}
    }
}


void usage(const char * progname)
{
    log(LG_ERROR, "usage: %s <options>", progname);
    log(LG_ERROR, "  warning: if you choose not to compare heuristics the rand_spanning_tree will be executed");
    log(LG_ERROR, "  input options:");
    log(LG_ERROR, "        to generate a random graph you must provide:");
    log(LG_ERROR, "          -v <vertices> (# of vertices)");
    log(LG_ERROR, "          -w <weight> (total weight of graph)");
    log(LG_ERROR, "          -d <density> (graph density in percent)");
    log(LG_ERROR, "          -c <clusters> (# of cluters in partition)");
    log(LG_ERROR, "        to read a graph from XML:");
    log(LG_ERROR, "          -i <input file> (path to XML file)");
    log(LG_ERROR, "  output options:");
    log(LG_ERROR, "        -D verbose output (debug)");
    log(LG_ERROR, "        -o <output file> writes the best solution to a file");
    log(LG_ERROR, "  rand_spanning_tree heuristic options:");
    log(LG_ERROR, "        -t <tries> (total random spanning trees to test)");
    log(LG_ERROR, "        -g <tries> (give up after <#tries> not improving the solution and requires -t ou -T)");
    log(LG_ERROR, "        -T <secs> (time in seconds allowed to process)");
    log(LG_ERROR, "  improved rand heuristic options:");
    log(LG_ERROR, "        -R use random connectons when impoving solution is not possible");
    log(LG_ERROR, "  test heuristic options:");
    log(LG_ERROR, "        -C compare all heuristics and prints a summary");
    log(LG_ERROR, "        -b use the best time to call rand_spanning_tree");
    
    exit(1);
}

int main(int argc, char **argv)
{
    const char * filename = 0;
    const char * outputfile = 0;
    bool debugmode = false;
    int tries = 0;
    int clusters = 0;
    int totalWeight = 0;
    int density = 0;
    int nVertices = 0;
    int giveup = 0;
    int bestWeight = -1;
    double time = 0;
    bool compare = false;
    bool tryRand = false;
    bool bestTime = false;
    
    int c = 0;
    while ((c = getopt(argc, argv, "DRCbi:o:t:c:w:d:v:g:T:")) != -1)
    {
        switch (c)
        {
            case 'D':
               debugmode = true;
               break;
            case 'C':
                compare = true;
                break;
            case 'R':
                tryRand = true;
                break;                
	    case 'b':
                bestTime = true;
                break;                
            case 'i':
                filename = optarg;
                break;
            case 'o':
                outputfile = optarg;
                break;
            case 't':
                tries = atoi(optarg);
                break;
            case 'c':
                clusters = atoi(optarg);
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
            case 'g':
                giveup = atoi(optarg);
                break;
            case 'T':
                time = atof(optarg);
                break;                
            case '?':
            default:
                usage(argv[0]);
                break;
        };
    }
    
    if (debugmode)
    {
        Logger::instance()->setPriorityMask(LG_DEBUG | LG_INFO | LG_NOTICE | LG_ERROR | LG_CRITICAL);
    }
    
    if (filename == 0 and 
        (not clusters or not density or not totalWeight or not nVertices))
    {
        usage(argv[0]);    
    }
    
    if (not tries and not time and not compare)
    {
        usage(argv[0]);
    }
    
//    if (density > 45) density = 45;

    if (filename != 0 and totalWeight < nVertices)
    {
        log(LG_DEBUG, "cannot generate random graph: weight is less than total vertices");
        return -1;
    }

    Graph graph = (filename != 0) ? readGraphFromXML(filename, clusters) : 
                                    generateBalancedGraph(nVertices, clusters, totalWeight, density);

    if (totalWeight > 0) bestWeight = totalWeight/clusters;

    Graph tree(graph.nVertices());
    
    Timer timerRandom;
    Timer timerImproved;
    Timer timerCassio;
    
    Partition partRandom, partImproved, partCassio, best;
    
    double runRandTime = time;
    
    if (compare)
    {
        timerImproved.start();
        partImproved = improvedRandSpanningTreeHeuristic(graph, clusters, tryRand);
        timerImproved.stop();
        
        timerCassio.start();
        partCassio = cassioRandSpanningTreeHeuristic(graph, clusters);
        timerCassio.stop();
        
        if (bestTime) runRandTime = std::min(timerImproved.getElapsedTime(), timerCassio.getElapsedTime());
        else runRandTime = std::max(timerImproved.getElapsedTime(), timerCassio.getElapsedTime());
    }

    timerRandom.start();
    partRandom = randSpanningTreeHeuristic(graph, clusters, tries, tree, runRandTime, giveup);
    timerRandom.stop();
    
    if (compare)
    {
	if (debugmode)
        {
            log(LG_INFO, "improved_rand_spanning_tree solution"); 
            showBestSolution(graph, partImproved, timerImproved, partImproved.lightestWeight());
            log(LG_INFO, "cassio_rand_spanning_tree solution"); 
	    showBestSolution(graph, partCassio, timerCassio, partCassio.lightestWeight());
	    log(LG_INFO, "rand_spanning_tree solution"); 
	    showBestSolution(graph, partRandom, timerRandom, partRandom.lightestWeight());
        }
        
        std::string winner;
        
        if (partImproved.lightestWeight() > partCassio.lightestWeight()) 
        {
            if (partImproved.lightestWeight() > partRandom.lightestWeight()) 
	       winner = "improved_rand_spanning_tree";
	    else if (partRandom.lightestWeight() > partImproved.lightestWeight())
	       winner = "rand_spanning_tree";
	    else
	       winner = "improved_rand_spanning_tree and rand_spanning_tree";
	}
        else if (partImproved.lightestWeight() == partCassio.lightestWeight())
        {
            if (partImproved.lightestWeight() > partRandom.lightestWeight()) 
                winner = "improved_rand_spanning_tree and cassio_rand_spanning_tree";
	    else if (partRandom.lightestWeight() > partImproved.lightestWeight())
	        winner = "rand_spanning_tree";
            else 
		winner = "tie";
        }
        else
        {
            if (partCassio.lightestWeight() > partRandom.lightestWeight()) 
                winner = "cassio_rand_spanning_tree";
	    else if (partRandom.lightestWeight() > partCassio.lightestWeight())
	        winner = "rand_spanning_tree";
	    else
	        winner = "cassio_rand_spanning_tree and rand_spanning_tree";
	}
               
        
	log(LG_INFO, "%u vertices %u edges %u weight %u-partition, best known %d, winner %s", 
	             graph.nVertices(), graph.nEdges(), graph.totalWeight(), clusters, bestWeight, winner.c_str());

        log(LG_INFO, "    improved_rand_spanning_tree solution: min %u max %u time %5.4f", 
                     partImproved.lightestWeight(), partImproved.heaviestWeight(), timerImproved.getElapsedTime());
        log(LG_INFO, "    cassio_rand_spanning_tree solution: min %u max %u time %5.4f", 
                     partCassio.lightestWeight(), partCassio.heaviestWeight(), timerCassio.getElapsedTime());
        log(LG_INFO, "    rand_spanning_tree solution: min %u max %u time %5.4f", 
                     partRandom.lightestWeight(), partRandom.heaviestWeight(), timerRandom.getElapsedTime());                     
    }
    else
    {
        showBestSolution(graph, partRandom, timerRandom, partRandom.lightestWeight());
    }
    
    if (outputfile)
    {
        writeSolution(outputfile, graph, best, tree);
    }
    
    return 0;
}

