#include <iostream>
#include <map>
#include <vector>
#include <utility>

int main()
{
	unsigned size, partition, iteractions;
	double time;

	std::map<unsigned, std::map<unsigned, std::pair<double, unsigned> > > stats;
	
	while (std::cin >> size >> partition >> time >> iteractions)
	{
		stats[size][partition].first += time;
		stats[size][partition].second++;
	}

	std::map<unsigned, std::map<unsigned, std::pair<double, unsigned> > >::const_iterator it;
	for (it = stats.begin(); it != stats.end(); it++)
	{
		std::map<unsigned, std::pair<double, unsigned> >::const_iterator i;
		for (i = it->second.begin(); i != it->second.end(); i++)
		{
			std::cout << it->first << " " << i->first << " " << i->second.first / i->second.second << std::endl;
		}
	}
	
	return 0;
}
