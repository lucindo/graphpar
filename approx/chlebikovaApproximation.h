//
// Copyright (C) 2005-2006 Renato Lucindo, 
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

#if !defined(chlebikovaApproximation_H)
#define chlebikovaApproximation_H

#include "graph.h"
#include "partition.h"

Partition chlebikovaBlockApproximation(const Graph& graph, unsigned P = 1, unsigned K = 2);
Partition chlebikovaApproximation(const Graph& graph, unsigned P = 1, unsigned K = 2);

#endif // chlebikovaApproximation_H
