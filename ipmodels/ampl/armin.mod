option solver cplexamp;
option cplex_options 'mipdisplay=3';

param n integer;
param c integer;

set V := 0..n-1;
set C := 1..c;

param arcs {V, V} binary;

param w {V} >= 0;

param M{C} := sum {i in V} w[i];

# VARIABLES
# ---------

# assignment of nodes to clusters
var E{V, C} binary;

# flow on arc per cluster
var f{V, V, C} integer >=0;

# flow from supernode
var F{V, C} integer >=0;

# start of flow per cluster
var s{V, C} binary;

# weight per cluster
var m{C} integer >= 0;

data ip.dat;

# OBJECTIVE
# ---------

maximize smallestCluster: m[1];
 

# CONSTRAINTS
# -----------

# order among cluster sizes: m1 <= m2 <= ...
subject to clusterOrdering {h in C: h != 1}: m[h-1] <= m[h];

# every node is exactly one cluster
subject to oneClusterPerNode {i in V}: sum {h in C} E[i,h] = 1;

# size per cluster
subject to clusterSize {h in C}: m[h] = sum {i in V} w[i]*E[i,h];

# flow must start somewhere
subject to flowStart {h in C}: sum {i in V} s[i,h] = 1;

# start of flow must belong to same cluster
subject to nodeFlow {i in V, h in C}: s[i,h] <= E[i,h];

# lower bound on inflow from supernode
subject to inflowLowerBound {i in V, h in C}: m[h] - M[h]*(1 - s[i,h]) <= F[i,h];

# upper bound on inflow from supernode
subject to inflowUpperBound {i in V, h in C}: F[i,h] <= M[h]*s[i,h];

subject to inflowUpperBoundNew {i in V, h in C}:
   F[i,h] <= w[i]*s[i,h] + sum {j in V: arcs[i,j]=1} f[i,j,h];

# flow conservation
subject to flowConservation {j in V, h in C}:
    sum {i in V: arcs[i,j]=1} f[i,j,h] + F[j,h] = w[j]*E[j,h] + sum {k in V: arcs[j,k]=1} f[j,k,h];

# nodes with flow belong to cluster
subject to nodesWithFlow {j in V, h in C}:
   sum {i in V: arcs[i,j]=1} f[i,j,h] <= M[h]*E[j,h];

solve;

print "Solution found:";
for {k in C} {
  printf "Component %d: ", k;
  for {i in V: E[i,k] = 1} {
    printf "%d ", i;
  }
  printf "\n";
}
