// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/ 


#ifndef __PARTITIONER_H_INCLUDED__
#define __PARTITIONER_H_INCLUDED__

#include <vector>
#include <numeric> //iota
#include <string>
#include <list>
#include "assert.h"

#include "RestrictedGrowthString.h"

namespace fuzzy_transfer_distance {
	
class Partitioner
{

	using id_type = unsigned int;
	using set_type = std::vector<id_type>;

public:

	using RGS_type = std::vector<unsigned int>; // Restricted Growth String

	template<typename T>
	Partitioner(T const& set) : m_set(enumerate_elements(set)){}

	// http://www.computing-wisdom.com/computingreadings/fasc3b.pdf : response to exercice 17, page 68, 7.2.1.5
	std::vector<RestrictedGrowthString> construct_all_m_blocks_partitions_of_the_set_by_algorithm_u(unsigned int nb_blocks){

		std::vector<RestrictedGrowthString> result;

		if(nb_blocks == 1){

			result.emplace_back(RGS_type(m_set.size(), 0));

		}else if(nb_blocks == m_set.size()){
			RGS_type v(m_set.size()) ;
			std::iota (std::begin(v), std::end(v), 0);
			result.emplace_back(v);

		}else{
			assert( (1 < nb_blocks) && (nb_blocks < m_set.size() ) );
			auto n = m_set.size();
			a = RGS_type(n+1, 0);
			for(unsigned int j = 1; j <= nb_blocks; ++j ){ a[n - nb_blocks + j] = j -1;	}

			f(nb_blocks, n, 0);

			
			for(auto& it : m_partitions){
				it.erase(it.begin());
				result.emplace_back(it);
			}

			m_partitions.clear();
		}
		

		return result;
	}

private:

	RGS_type a;
	std::vector<RGS_type> m_partitions;
	set_type m_set;

	void visit(){
		m_partitions.push_back(a);
	}

	void f(unsigned int mu, unsigned int nu, unsigned int sigma){
		if(mu == 2){
			visit();
		}else{
			f(mu-1, nu-1, (mu+sigma)%2);
		}

		if(nu == mu+1){
			a[mu] = mu -1;
			visit();
			while(a[nu] > 0){
				a[nu] = a[nu] - 1;
				visit();
			}
		}else if(nu > mu + 1){
			if((mu + sigma)%2 == 1){
				a[nu - 1] = mu - 1;
			}else{
				a[mu] = mu -1;
			}

			if( (a[nu] + sigma) %2 == 1){
				b(mu, nu - 1, 0);
			}else{
				f(mu, nu  -1, 0);
			}

			while(a[nu] > 0){
				a[nu] = a[nu] - 1;
				if( (a[nu] + sigma) %2 == 1){
					b(mu, nu - 1, 0);
				}else{
					f(mu, nu - 1, 0);
				}
			}
		}
	}

	void b(unsigned int mu, unsigned int nu, unsigned int sigma){
		if(nu == mu + 1){
			while( a[nu] < mu - 1){
				visit();
				a[nu] = a[nu] + 1;
			}
			visit();
			a[mu] = 0;
		}else if( nu > mu + 1){
			if( (a[nu] + sigma) %2 == 1){
				f(mu, nu - 1, 0);
			}else{
				b(mu, nu -1, 0);
			}

			while( a[nu] < mu -1 ){
				a[nu] = a[nu] + 1;
				if( (a[nu] + sigma) %2 == 1){
					f(mu, nu - 1, 0);
				}else{
					b(mu, nu - 1, 0);
				}
			}

			if( (mu + sigma) %2 == 1){
				a[nu - 1] = 0;
			}else{
				a[mu] = 0;
			}
		}

		if(mu == 2) {
			visit();
		}else{
			b(mu -1, nu-1, (mu +sigma)%2);
		}
	}

	template<typename T>
	set_type enumerate_elements(T const& set) const {
	    std::vector<id_type> v(set.size());
	    std::iota(v.begin(), v.end(), 1);
	    return v;
	}

};

} // namespace fuzzy_transfer_distance



#endif