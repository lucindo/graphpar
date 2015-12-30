//
// Copyright (C) 2005-2006 Renato Lucindo,
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#include "toAmpl.h"
#include <iostream>

void toAmpl(const Graph& graph, Partition& partition, unsigned clusters)
{
    unsigned n = graph.nVertices();
    std::cout << "param n := " << n << ";" << std::endl;
    std::cout << "param c := " << clusters << ";" << std::endl;
    std::cout << "param w := " << std::endl;
    for(unsigned i=0; i<n; ++i)
        std::cout << " [" << i << "] " << graph.getVertexWeight(i) << std::endl;
    std::cout << ";" << std::endl;
    std::cout << "param arcs := " << std::endl;
    for(unsigned i=0; i<n; ++i)
        for(unsigned j=0; j<n; ++j)
        {
            std::cout << "[" << i << "," << j << "] ";
            if(i != j && graph.isConnected(i,j))
                std::cout << "1";
            else
                std::cout << "0";
            std::cout << std::endl;
        }
    std::cout << ";" << std::endl;

    partition.sort();
    
    for (unsigned p = 0; p < partition.nComponents(); ++p)
    {
        std::set<unsigned>& vertices = partition.component(p).vertices;
        for (std::set<unsigned>::const_iterator c = vertices.begin(); c != vertices.end(); c++)
                std::cout << "let E[" << *c << "," << (p+1) << "] := 1;" << std::endl;
    }
}
