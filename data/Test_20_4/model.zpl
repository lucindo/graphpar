 
# SETS
# ----

# clusters
set C := { read "clusters.dat" as "<1n>" comment "#" };

# nodes 
set V := { read "nodes.dat" as "<1n>" comment "#" };

# arcs
set A := { read "arcs.dat" as "<2n,3n>" comment "#" };


# PARAMETERS
# ----------

# weight per node
param w[V] := read "nodes.dat" as "<1n> 2n" comment "#";

# max weight per cluster
param M[C] := read "clusters.dat" as "<1n> 3n" comment "#";


# VARIABLES
# ---------

# assignment of nodes to clusters
var z[V*C] binary;

# flow on arc per cluster
var f[A*C] integer >=0;

# flow from supernode
var F[V*C] integer >=0;

# start of flow per cluster
var s[V*C] binary;

# weight per cluster
var m[C] integer >= 0;


# OBJECTIVE
# ---------

maximize smallestCluster: m[1];
 

# CONSTRAINTS
# -----------

# order among cluster sizes: m1 <= m2 <= ...
subto clusterOrdering: forall <h> in C with h != 1 do
   m[h-1] <= m[h];

# every node is exactly one cluster
subto oneClusterPerNode: forall <i> in V do
   sum <h> in C do z[i,h] == 1;

# size per cluster
subto clusterSize: forall <h> in C do
   m[h] == sum <i> in V do w[i]*z[i,h];

# flow must start somewhere
subto flowStart: forall <h> in C do
   sum <i> in V do s[i,h] == 1;

# start of flow must belong to same cluster
subto nodeFlow: forall <i,h> in V*C do
   s[i,h] <= z[i,h];

# lower bound on inflow from supernode
subto inflowLowerBound: forall <i,h> in V*C do
   m[h] - M[h]*(1 - s[i,h]) <= F[i,h];

# upper bound on inflow from supernode
subto inflowUpperBound: forall <i,h> in V*C do
   F[i,h] <= M[h]*s[i,h];

subto inflowUpperBoundNew: forall <i,h> in V*C do
   F[i,h] <= w[i]*s[i,h] + sum <i,j> in A do f[i,j,h];

# flow conservation
subto flowConservation: forall <j,h> in V*C do
    sum <i,j> in A do f[i,j,h] + F[j,h] 
 == w[j]*z[j,h] + sum <j,k> in A do f[j,k,h];

# nodes with flow belong to cluster
subto nodesWithFlow: forall <j,h> in V*C do
   sum <i,j> in A do f[i,j,h] <= M[h]*z[j,h];

