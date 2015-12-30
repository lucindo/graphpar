//
// Copyright (C) 2005-2006 Renato Lucindo,
// Institute of Mathematics and Statistics (IME-USP),
// University of Sao Paulo, Sao Paulo, Brazil.
// All rights reserved. E-mail: <lucindo@gmail.com>.
//

// code due Cassio P. de Campos <cassio@ime.usp.br>

#if !defined(toAmpl_H)
#define toAmpl_H

#include "graph.h"
#include "partition.h"

void toAmpl(const Graph& graph, Partition& partition, unsigned clusters);

#endif // toAmpl_H
