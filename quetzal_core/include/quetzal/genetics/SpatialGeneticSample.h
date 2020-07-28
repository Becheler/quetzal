// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr> Florence Jornod <florence@jornod.com>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
************************************************************************/

#ifndef __SPATIALGENETICSAMPLE_H_INCLUDED__
#define __SPATIALGENETICSAMPLE_H_INCLUDED__

#include <string>
#include <set>
#include <vector>
#include <map>
#include <assert.h>

namespace quetzal{
namespace genetics{

template<typename Space, typename Individual>
class SpatialGeneticSample
{

public:
	using coord_type = Space;
	using individual_type = Individual;
	using locus_ID_type = typename individual_type::locus_ID_type;
	using value_type = typename individual_type::allele_type::value_type;

	SpatialGeneticSample( std::map<coord_type, std::vector<individual_type>> const& data) :
	m_loci(extract_loci(data)),
	m_locations(localize(data)),
	m_dictionnary(data)
	{}

	SpatialGeneticSample() = default;

	void add(coord_type const& x, Individual const& ind) {
		if(m_loci.empty()){
			m_loci = ind.loci();
		}else{
			assert(ind.loci() == m_loci);
		}
		m_dictionnary[x].push_back(ind);
		m_locations.insert(x);
	}

	void add(Individual const& ind, coord_type const& x) {
		add(x, ind);
	}

	auto size(coord_type const& x) const {
		assert( m_dictionnary.count(x) > 0 );
		return m_dictionnary.at(x).size();
	}

	auto size() const{
		unsigned int size = 0;
		for(auto const& it : m_dictionnary)
			size += it.second.size();

		return size;
		}

	template<typename T>
	SpatialGeneticSample<coord_type, individual_type>& reproject(T const& projection) {

		std::map<coord_type, std::vector<individual_type>> new_dico;
		std::set<coord_type> new_sites;

		for(auto const& it : m_dictionnary){
			auto x = projection.reproject_to_centroid(it.first);
			new_dico.insert(std::make_pair(x, it.second));
			new_sites.insert(x);
		}

		new_dico.swap(m_dictionnary);
		new_sites.swap(m_locations);
		return *this;
	}


	std::set<coord_type> const& get_sampling_points() const {
		return m_locations;
	}

	std::set<typename individual_type::locus_ID_type> const& loci() const {
		return m_loci;
	}

	std::vector<individual_type> const& individuals_at(coord_type const& x) const {
		return m_dictionnary.at(x);
	}

	// TODO take ploidy into account
	auto nb_gene_copies_discarding_NA(locus_ID_type const& locus) const
	{
		std::map<coord_type, std::map<value_type, unsigned int>> counts;
		for(auto const& x : get_sampling_points())
		{
			for(auto const& individual : individuals_at(x))
			{
				auto alleles = individual.alleles(locus);

				if(alleles.first.get_allelic_state() > 0)
				{
					counts[x][alleles.first.get_allelic_state()] += 1;
				}

				if(alleles.second.get_allelic_state() > 0)
				{
					counts[x][alleles.second.get_allelic_state()] += 1;
				}
			}
		}
		return counts;
	}

	unsigned int allelic_richness(locus_ID_type const& locus) const {
		std::set<value_type> set;
		auto freq = frequencies_discarding_NA(locus);
		for(auto const& it1 : freq){
			for(auto const& it2 : it1.second){
				set.insert(it2.first);
			}
		}
		return set.size();
	}

	auto frequencies_discarding_NA(locus_ID_type const& locus) const {

		std::map<coord_type, std::map<value_type, double>> freq;

		auto counts = nb_gene_copies_discarding_NA(locus);

		auto get = [](auto const& a, auto const& b){return b.second;};

		for(auto const& it1 : counts)
		{
			double sum = std::accumulate(it1.second.begin(), it1.second.end(), 0.0, get);
			assert(sum > 0.0);

			for(auto const& it2 : it1.second)
			{
				freq[it1.first][it2.first] = static_cast<double>(it2.second)/sum;
			}
		}
		return freq;
	}

private:

	std::set<typename individual_type::locus_ID_type> m_loci;
	std::set<coord_type> m_locations;
	std::map<coord_type, std::vector<individual_type>> m_dictionnary;

	template<typename T>
	std::set<coord_type> localize(T const& data) const {
		std::set<coord_type> locations;
		for(auto const& it : data){
			locations.insert(it.first);
		}
		return(locations);
	}

	std::set<typename individual_type::locus_ID_type> extract_loci(std::map<coord_type, std::vector<individual_type>> const& data) const {
		std::set<typename individual_type::locus_ID_type> loci = data.cbegin()->second.front().loci();
		assert( !loci.empty());
		for(auto const& it1 : data){
			assert( ! it1.second.empty() );
			for(auto const& it2 : it1.second){
				assert(it2.loci() == loci );
			}
		}
		return loci;
	}

};
}
}


#endif
