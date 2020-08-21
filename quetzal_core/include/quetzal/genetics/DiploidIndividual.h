// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr> Florence Jornod <florence@jornod.com>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
************************************************************************/

#ifndef __DIPLOID_INDIVIDUAL_H_INCLUDED__
#define __DIPLOID_INDIVIDUAL_H_INCLUDED__

#include <map>
#include <string>
#include <set>

namespace quetzal{
namespace genetics{

template<typename Allele>
class DiploidIndividual
{
public:
	using allele_type = Allele;
	using locus_ID_type = std::string;

	DiploidIndividual() = default;

	DiploidIndividual(std::pair<allele_type, allele_type> const& locus, locus_ID_type const& name) {
		m_loci[name] = locus;
	}

	DiploidIndividual(locus_ID_type const& name, std::pair<allele_type, allele_type> const& locus) {m_loci[name] = locus; }
	DiploidIndividual(locus_ID_type const& name, allele_type allele_1, allele_type allele_2) {m_loci[name] = std::make_pair(allele_1, allele_2); }
	DiploidIndividual(allele_type allele_1, allele_type allele_2, locus_ID_type const& name) {m_loci[name] = std::make_pair(allele_1, allele_2); }

	DiploidIndividual<Allele>& add(locus_ID_type const& name, allele_type a1 , allele_type a2){
		m_loci[name] = std::make_pair(a1,a2);
		return *this;
	}

	DiploidIndividual<Allele>& add(std::pair<allele_type, allele_type> const& locus, locus_ID_type const& name){
		m_loci[name] = locus;
		return *this;
	}

	DiploidIndividual<Allele>& add(locus_ID_type const& name, std::pair<allele_type, allele_type> const& locus){
		m_loci[name] = locus;
		return *this;
	}

	unsigned int nGenotypedLoci() const {
		return m_loci.size();
	}

	std::pair<allele_type, allele_type> const& alleles(locus_ID_type const& locus) const {
		return m_loci.at(locus);
	}

	std::set<locus_ID_type> loci() const {
		std::set<locus_ID_type> loci;
		for(auto const& it : m_loci){
			loci.insert(it.first);
		}
		return loci;
	}

private:
	 std::map<locus_ID_type, std::pair<allele_type, allele_type>> m_loci;
};

}
}

#endif
