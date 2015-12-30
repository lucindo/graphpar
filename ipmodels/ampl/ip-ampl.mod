option solver cplexamp;
option cplex_options 'mipdisplay=3';

param n integer;
param c integer;

set nodes := 0..n-1;
param arcs {nodes, nodes} binary;

param w {nodes} >= 0;

set comp := 1..c;

var E{nodes, comp} binary;
var F{nodes, nodes, comp} >= 0;
var R{nodes, comp};
var S{nodes, comp} >=0, <= 1;
var P{comp} >= 0;

data ip.dat;

maximize obj: P[1];

subject to rpeso {k in comp}: P[k] = sum {i in nodes} w[i]*E[i,k];

subject to rtam {k in comp: k != 1}: P[k-1] <= P[k];

subject to r {i in nodes}: sum {k in comp} E[i,k] = 1;

subject to estr2novo {i in nodes, k in comp}: S[i,k] >= E[i,k] - sum {j in nodes: i>j} E[j,k];

subject to estr3novo {k in comp}: sum {i in nodes} S[i,k] = 1;

subject to FlowStartlimitnovo {i in nodes, k in comp}: sum {j in nodes: arcs[i,j]=1} F[j,i,k] <= (1-S[i,k])*n;

subject to RestStartnovo {i in nodes, k in comp}: R[i,k] <= n*E[i,k] - S[i,k]*n*2;
subject to Restnovo {i in nodes, k in comp}: R[i,k] >= E[i,k] - S[i,k]*(n+1);

subject to Balancenovo {z in nodes, k in comp}:
   sum {i in nodes: arcs[i,z]=1} F[i,z,k] = R[z,k] + sum {j in nodes: arcs[z,j]=1} F[z,j,k];

subject to samecomplimit {i in nodes, j in nodes, k in comp}: F[i,j,k] <= E[i,k]*n;
subject to samecomplimit1 {i in nodes, j in nodes, k in comp}: F[i,j,k] <= E[j,k]*n;

solve;

print "Solution found:";
for {k in comp} {
  printf "Component %d: ", k;
  for {i in nodes: E[i,k] = 1} {
    printf "%d ", i;
  }
  printf "\n";
}

#for {i in nodes, k in comp} if E[i,k] then {
#  printf "vertex=%d comp=%d : ", i, k;
#  for {j in nodes} if arcs[i,j] then printf "%d ", j;
#  printf "\n";
#}

#for {k in comp, i in nodes, j in nodes} print 'F[',i, j, k,']=', F[i,j,k];
#for {k in comp, i in nodes} print 'R[',i,k, ']=', R[i,k];
