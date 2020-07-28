// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef RESTRICTED_GROWTH_STRING
#define RESTRICTED_GROWTH_STRING

#include <set>
#include <vector>
#include <iostream>

class RestrictedGrowthString
{

public:
	using block_type = unsigned int;

	RestrictedGrowthString(std::vector<block_type> const& rgs) : m_rgs(rgs), m_blocks_id(retrieve_unique_blocks(rgs))
	{}

	auto nBlocks() const { return m_blocks_id.size();}

	auto size() const { return m_rgs.size();}

	auto begin() const {return m_rgs.cbegin();}

	auto end() const {return m_rgs.cend();}

	bool operator==(RestrictedGrowthString const& other) const {
		return (this->m_rgs == other.m_rgs);
	}

	auto at(unsigned int n) const {
		return m_rgs.at(n);
	}

private:
	std::vector<block_type> m_rgs;
	std::set<block_type> m_blocks_id;

	std::set<block_type> retrieve_unique_blocks(std::vector<block_type> const& rgs){
		std::set<block_type> ids;
		for(auto const& it : rgs){
			ids.insert(it);
		}
		return ids;
	}

};


#endif
