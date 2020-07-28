// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

// g++ -o Partitioner_test Partitioner_test.cpp -std=c++14 -Wall

#include "Partitioner.h"
#include <vector>
#include "assert.h"
#include <iostream>

int main(){
	using fuzzy_transfer_distance::Partitioner;

	std::vector<unsigned int> set = {1,2,3,4,5};

	Partitioner partitioner(set);

	auto const& three_blocks_partitions = partitioner.construct_all_m_blocks_partitions_of_the_set_by_algorithm_u(3);
	assert(three_blocks_partitions.size() == 25);


	for(auto const& it : three_blocks_partitions){
		assert(it.size() == 5);
	}

	// http://www.computing-wisdom.com/computingreadings/fasc3b.pdf : page 28, 7.2.1.5
	std::vector<std::vector<unsigned int>> expectations = {
		{0,0,0,1,2},{0,0,1,1,2},{0,1,1,1,2},{0,1,0,1,2},{0,1,0,0,2},
		{0,1,1,0,2},{0,0,1,0,2},{0,0,1,2,2},{0,1,1,2,2},{0,1,0,2,2},
		{0,1,2,2,2},{0,1,2,1,2},{0,1,2,0,2},{0,1,2,0,1},{0,1,2,1,1},
		{0,1,2,2,1},{0,1,0,2,1},{0,1,1,2,1},{0,0,1,2,1},{0,0,1,2,0},
		{0,1,1,2,0},{0,1,0,2,0},{0,1,2,2,0},{0,1,2,1,0},{0,1,2,0,0}};

	std::vector<RestrictedGrowthString> expected_three_blocks_partitions;
	for(auto const& it : expectations){
		expected_three_blocks_partitions.emplace_back(it);
	}

	assert(three_blocks_partitions == expected_three_blocks_partitions);


	std::vector<unsigned int> other = {1,2,3,4};
	Partitioner partitioner2(other);
	auto const& ps = partitioner2.construct_all_m_blocks_partitions_of_the_set_by_algorithm_u(3);
	/* Expected :
	[0, 0, 1, 2]
	[0, 1, 1, 2]
	[0, 1, 0, 2]
	[0, 1, 2, 2]
	[0, 1, 2, 1]
	[0, 1, 2, 0]
	*/
	assert(ps.size() == 6);

	// Test cause of weird results...
	Partitioner partitioner3(std::set<int>({1,2,3,4}));
	auto all = partitioner3.construct_all_m_blocks_partitions_of_the_set_by_algorithm_u(3);
	assert(all.size() == 6);

	// limit cases
	Partitioner partitioner4(std::set<int>({1,2,3}));
	auto all2 = partitioner4.construct_all_m_blocks_partitions_of_the_set_by_algorithm_u(3);
	assert(all2.size() == 1);

	Partitioner partitioner5(std::set<int>({1,2,3,4}));
	auto all3 = partitioner5.construct_all_m_blocks_partitions_of_the_set_by_algorithm_u(4);
	assert(all3.size() == 1);
}
