// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
************************************************************************/

//	g++ -o SpatialGeneticsample1_test -Wall -std=c++14 SpatialGeneticsample1_test.cpp

#include <assert.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <vector>

#include "DiploidIndividual.h"
#include "SpatialGeneticSample.h"

int main(){

	using marker_type = unsigned int;
	using coord_type = unsigned int;
	using individual_type = quetzal::genetics::DiploidIndividual<marker_type>;
	quetzal::genetics::SpatialGeneticSample<coord_type, individual_type> sample1;

	auto alleles = std::make_pair(180,360);
	std::vector<std::string> names = {"L1", "L2", "L3", "L4"};

	individual_type ind;

	for(auto const& it : names){
		ind.add(it, alleles);
	}

	coord_type x1 = 1;
	coord_type x2 = 2;

	sample1.add(x1, ind);
	sample1.add(x2, ind);
	sample1.add(x2, ind);
	sample1.add(x2, ind);

	assert(sample1.size() == 4);
	assert(sample1.size(x1)== 1 && sample1.size(x2) ==3 );

	assert(sample1.loci() == std::set<std::string>({"L1", "L2", "L3", "L4"} ));
	assert(sample1.get_sampling_points() == std::set<coord_type>({x1, x2}));
	assert(sample1.individuals_at(x2).size() == 3);

	std::map<coord_type, std::vector<individual_type>> data = {	{x1, {ind}}, {x2, {ind, ind, ind}} };

	quetzal::genetics::SpatialGeneticSample<coord_type, individual_type> sample2(data);
	assert(sample1.loci() == sample2.loci());
	assert(sample1.get_sampling_points() == sample2.get_sampling_points());
	assert(sample2.individuals_at(x2).size() == 3);

	return 0;
}
