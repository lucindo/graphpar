eval '(exit $?0)' && eval 'exec perl -w $0 ${1+"$@"}'
    & eval 'exec perl -w $0 $argv:q'
    if 0;

while(<>)
{
	# test 1: graph with 100 vertices and 99 edges, optimal component size 3764, 2-partition found after 18 iterations [3764 3764] - 0 0.00
	if ($_ =~ m/with\s+(\d+).+and\s+(\d+).+size\s+(\d+),\s+(\d+)-partition.+after\s+(\d+).+\-\s+(\d+)\s+(\d+\.\d+)/)
	{
		print "$1 $2 $3 $4 $5 $6 $7\n";
	}
}
