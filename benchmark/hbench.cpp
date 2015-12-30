//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#include <cmath>

#include "graph.h"
#include "generators.h"
#include "random.h"
#include "log.h"
#include "partition.h"
#include "randSpanningTree.h"
#include "improvedRandSpanningTree.h"
#include "mixedImprovedRandSpanningTree.h"
#include "cassioRandSpanningTree.h"
#include "timers.h"

#include <vector>
#include <fstream>
#include <algorithm>
#include <string>
#include <map>
#include <iostream>

#include <getopt.h>

void usage(const char * progname)
{
    log(LG_ERROR, "usage: %s [-D] -v <# vertices> -d <%% density> -c <# clusters> -r <# runs> [-h <heuristc number>]", progname);
    
    log(LG_ERROR, "  heuristc numbers:");
    log(LG_ERROR, "     0 = quadratic mixed random heuristic O(v^5 c^2)");
    log(LG_ERROR, "     1 = linear mixed random heuristic O(v^4 c^2)");
    log(LG_ERROR, "     2 = quadratic random heuristic O(v^4 c^2)");
    log(LG_ERROR, "     3 = linear random heuristic O(v^3 c^2)");
    log(LG_ERROR, "     4 = cassio's heuristic O(e v^4 c^2)");
    log(LG_ERROR, "     5 = improved random heuristic O(v^3 c^3)");
    log(LG_ERROR, "     6 = improved heuristic O(v^3 c^2)");
    
    exit(1);
}

double calcRatio(int a, int b)
{
    return (double)a / (double) b;
}

struct HeuristicResults
{
    std::vector<double> minValues;
    std::vector<double> maxValues;
    double minRatio;
    double maxRatio;
    double time;
    
    HeuristicResults() : minRatio(0.0), maxRatio(0.0), time(0.0) {}
};

struct FinalHeuristicResults
{
    std::string heristicName;
    double minRatioAverage;
    double maxRatioAverage;
    double minSD;
    double maxSD;
    double timeAverage;
};

bool compareFinalHeuristicResults(FinalHeuristicResults p1, FinalHeuristicResults p2)
{
    return p1.minRatioAverage > p2.minRatioAverage;
}


void printResult(const char * heuristicName, const Partition& partition, int optimal, const Timer& timer)
{
    double minRatio = calcRatio(partition.lightestWeight(), optimal);
    double maxRatio = calcRatio(partition.heaviestWeight(), optimal);
    
    log(LG_INFO, "   %s min %d min_ratio %5.2f -- max %d max_ratio %5.2f  (%5.4f secs)", 
                 heuristicName, partition.lightestWeight(), minRatio, partition.heaviestWeight(), maxRatio, timer.getElapsedTime());
}

int main(int argc, char **argv)
{
    int clusters = 0;
    int density = 0;
    int nVertices = 0;
    int runs = 0;
    bool debugmode = false;
    int heuristic = -1;
    
    int c = 0;
    while ((c = getopt(argc, argv, "Dc:d:v:r:h:")) != -1)
    {
        switch (c)
        {
            case 'D':
               debugmode = true;
               break;
            case 'c':
                clusters = atoi(optarg);
                break;
            case 'd':
                density = atoi(optarg);
                break;
            case 'v':
                nVertices = atoi(optarg);
                break;
            case 'r':
                runs = atoi(optarg);
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
    
    if (debugmode)
    {
        Logger::instance()->setPriorityMask(LG_INFO | LG_NOTICE | LG_ERROR | LG_CRITICAL);
    }
    else
    {
        Logger::instance()->setPriorityMask(LG_NOTICE | LG_ERROR | LG_CRITICAL);
    }
    
    if (clusters == 0 or density == 0 or nVertices == 0 or runs == 0)
    {
        usage(argv[0]);
    }
    
    if (heuristic != -1 && (heuristic < 0 || heuristic > 6))
    {
        usage(argv[0]);
    }
    
    Random random;
    
    std::map<std::string, HeuristicResults> results;
    Timer timer;
    
    for (int r = 1; r <= runs; r++)
    {
        int optimalValue = random.rand(nVertices, 10 * nVertices);
        Graph graph = generateBalancedGraph(nVertices, clusters, clusters*optimalValue, density);
        Graph tree(graph.nVertices()); // needed by randSpanningTreeHeuristic
        
        log(LG_INFO, "round %d: graph %d vertices %d edges, optimal %d-partition is %d", 
                     r, graph.nVertices(), graph.nEdges(), clusters, optimalValue);
                     
        if (heuristic == -1 || (heuristic != -1 and heuristic == 3))
        {                     
	    timer.start();
	    Partition linearRandPartition = randSpanningTreeHeuristic(graph, clusters, nVertices, tree);
	    timer.stop();
	    results["linear_rand"].minValues.push_back(calcRatio(linearRandPartition.lightestWeight(), optimalValue));
	    results["linear_rand"].maxValues.push_back(calcRatio(linearRandPartition.heaviestWeight(), optimalValue));
	    results["linear_rand"].time += timer.getElapsedTime();
	    printResult("linear_rand", linearRandPartition, optimalValue, timer);
        }
        
        if (heuristic == -1 || (heuristic != -1 and heuristic == 2))
        {                             
	    timer.start();
	    Partition quadraticRandPartition = randSpanningTreeHeuristic(graph, clusters, nVertices * nVertices, tree);
	    timer.stop();
	    results["quadratic_rand"].minValues.push_back(calcRatio(quadraticRandPartition.lightestWeight(), optimalValue));
	    results["quadratic_rand"].maxValues.push_back(calcRatio(quadraticRandPartition.heaviestWeight(), optimalValue));
	    results["quadratic_rand"].time += timer.getElapsedTime();
	    printResult("quadratic_rand", quadraticRandPartition, optimalValue, timer);
        }
                
        if (heuristic == -1 || (heuristic != -1 and heuristic == 6))
        {
	    timer.start();
	    Partition improvedRandPartition = improvedRandSpanningTreeHeuristic(graph, clusters, false);
	    timer.stop();
	    results["improved"].minValues.push_back(calcRatio(improvedRandPartition.lightestWeight(), optimalValue));
	    results["improved"].maxValues.push_back(calcRatio(improvedRandPartition.heaviestWeight(), optimalValue));
	    results["improved"].time += timer.getElapsedTime();
	    printResult("improved", improvedRandPartition, optimalValue, timer);
        }
                
        if (heuristic == -1 || (heuristic != -1 and heuristic == 5))
        {                     
	    timer.start();
	    Partition randImprovedRandPartition = improvedRandSpanningTreeHeuristic(graph, clusters, true);
	    timer.stop();
	    results["improved_rand"].minValues.push_back(calcRatio(randImprovedRandPartition.lightestWeight(), optimalValue));
	    results["improved_rand"].maxValues.push_back(calcRatio(randImprovedRandPartition.heaviestWeight(), optimalValue));
	    results["improved_rand"].time += timer.getElapsedTime();
	    printResult("improved_rand", randImprovedRandPartition, optimalValue, timer);
        }
                
        if (heuristic == -1 || (heuristic != -1 and heuristic == 4))
        {                                     
	    timer.start();
	    Partition cassioPartition = cassioRandSpanningTreeHeuristic(graph, clusters);
	    timer.stop();
	    results["cassio"].minValues.push_back(calcRatio(cassioPartition.lightestWeight(), optimalValue));
	    results["cassio"].maxValues.push_back(calcRatio(cassioPartition.heaviestWeight(), optimalValue));
	    results["cassio"].time += timer.getElapsedTime();
	    printResult("cassio", cassioPartition, optimalValue, timer);
        }
        
        if (heuristic == -1 || (heuristic != -1 and heuristic == 1))
        {                             
	    timer.start();
	    Partition linearMixedImprovedRandPartition = mixedImprovedRandSpanningTreeHeuristic(graph, clusters, nVertices);
	    timer.stop();
	    results["linear_mixed_improved"].minValues.push_back(calcRatio(linearMixedImprovedRandPartition.lightestWeight(), optimalValue));
	    results["linear_mixed_improved"].maxValues.push_back(calcRatio(linearMixedImprovedRandPartition.heaviestWeight(), optimalValue));
	    results["linear_mixed_improved"].time += timer.getElapsedTime();
	    printResult("linear_mixed_improved", linearMixedImprovedRandPartition, optimalValue, timer);
        }
                
        if (heuristic == -1 || (heuristic != -1 and heuristic == 0))
        {                     
	    timer.start();
	    Partition quadraticMixedImprovedRandPartition = mixedImprovedRandSpanningTreeHeuristic(graph, clusters, nVertices * nVertices);
	    timer.stop();
	    results["quadratic_mixed_improved"].minValues.push_back(calcRatio(quadraticMixedImprovedRandPartition.lightestWeight(), optimalValue));
	    results["quadratic_mixed_improved"].maxValues.push_back(calcRatio(quadraticMixedImprovedRandPartition.heaviestWeight(), optimalValue));
	    results["quadratic_mixed_improved"].time += timer.getElapsedTime();
	    printResult("quadratic_mixed_improved", quadraticMixedImprovedRandPartition, optimalValue, timer);
        }
        
        if (not debugmode)
        {
            if (r % 10 == 0) 
            {
                std::cerr << '\r' << "                                                  ";
                std::cerr << '\r' << (100 * r) / runs << "% done ";
	    }
            else std::cerr << '.';
            std::cerr.flush();
        }
    }
    if (not debugmode)
    {
        std::cerr << '\r' << "                                                  ";
        std::cerr << '\r' << "100% done" << std::endl;
    }

    std::vector<FinalHeuristicResults> finalResults;
    for (std::map<std::string, HeuristicResults>::iterator r = results.begin(); r != results.end(); r++)
    {
        FinalHeuristicResults fResult;
        
        for (int v = 0; v < runs; v++)
        {
            r->second.minRatio += r->second.minValues[v];
	    r->second.maxRatio += r->second.maxValues[v];
        }
        r->second.minRatio = r->second.minRatio / (double) runs;
        r->second.maxRatio = r->second.maxRatio / (double) runs;
        
	fResult.minRatioAverage = r->second.minRatio;
        fResult.maxRatioAverage = r->second.maxRatio;
        
        // now we calculate de standard derivation
	for (int v = 0; v < runs; v++)
        {
            double minD = r->second.minValues[v] - r->second.minRatio;
            double maxD = r->second.maxValues[v] - r->second.maxRatio;
            
            fResult.minSD = minD * minD;
            fResult.maxSD = maxD * maxD;
        }
        fResult.minSD = fResult.minSD / (double) runs;
        fResult.maxSD = fResult.maxSD / (double) runs;
        
        fResult.minSD = sqrt(fResult.minSD);
        fResult.maxSD = sqrt(fResult.maxSD);
        
        fResult.timeAverage = r->second.time / (double) runs;
        fResult.heristicName = r->first;
        
        finalResults.push_back(fResult);
    }
    
    std::sort(finalResults.begin(), finalResults.end(), compareFinalHeuristicResults);
    
    log(LG_NOTICE, "Results for %d vertices, %d%% density, %d-partition, %d runs", nVertices, density, clusters, runs);
    log(LG_NOTICE, "%-25s %-8s  %-8s  %-8s  %-8s  %-8s", "Heuristic", "MinRatio", "MinSD", "MaxRatio", "MaxSD", "TimeAverage");
    for (unsigned r = 0; r < finalResults.size(); r++)
    {
        log(LG_NOTICE, "%-26s %-8.3f %-8.3f   %-8.3f %-8.3f    %-8.3f", 
                       finalResults[r].heristicName.c_str(), 
                       finalResults[r].minRatioAverage, 
                       finalResults[r].minSD,
                       finalResults[r].maxRatioAverage,
                       finalResults[r].maxSD,
                       finalResults[r].timeAverage);
    }

    return 0;
}
