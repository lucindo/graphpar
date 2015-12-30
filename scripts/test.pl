#!/usr/bin/perl 

sub round {
    my($number) = shift;
    return int($number + .5 * ($number <=> 0));
}

@density = (30, 60, 90);
for ($v = 10; $v < 101; $v += 10) 
{
	@sizes = (2, round($v/4), round($v/2), round((3*$v)/4) );

	foreach (@density) {
		$d = $_;
		foreach (@sizes) {
			# bin/hbench [-D] -v <# vertices> -d <% density> -c <# clusters> -r <# runs>
			$filename = "data/benchmark/cap3_${v}_${d}_${_}.txt";
			print "Will exec: $v vertices, $d density: getting $_ partitions (20 runs) -> $filename\n";
			`./run bin/hbench -v $v -d $d -c $_ -r 20 >> $filename`;
		}
	}
}
