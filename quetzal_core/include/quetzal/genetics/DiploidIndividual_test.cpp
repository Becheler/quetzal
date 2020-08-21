// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
************************************************************************/

//	g++ -o DiploidIndividual_test -Wall -std=c++14 DiploidIndividual_test.cpp

#include <assert.h>
#include <iostream>
#include <string>
#include <vector>
#include <set>

#include "DiploidIndividual.h"

int main(){

	using marker_type = unsigned int;

	quetzal::genetics::DiploidIndividual<marker_type> ind;
	assert( ind.nGenotypedLoci() == 0 );

	auto alleles = std::make_pair(180,360);
	std::string locus_name = "L1";

	ind.add(locus_name, alleles);

	assert( ind.nGenotypedLoci() == 1);

	std::set<std::string> loci = ind.loci();
	assert( ind.loci() == std::set<std::string>({"L1"}));

	std::vector<std::string> names = {"L2", "L3", "L4"};

	for(auto const& it : names){
		ind.add(it, alleles);
	}

	assert( ind.nGenotypedLoci() == 4);
	assert( ind.loci() == std::set<std::string>({"L1", "L2", "L3", "L4"}));

	return 0;
}
