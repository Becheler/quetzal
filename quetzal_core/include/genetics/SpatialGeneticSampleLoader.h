// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __GENETICSLOADER_H_INCLUDED__
#define __GENETICSLOADER_H_INCLUDED__


#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include "marker_traits.h"
#include "Allele.h"
#include "DiploidIndividual.h"
#include "SpatialGeneticSample.h"

using std::ifstream;
using std::string;
using std::cerr;

namespace quetzal {
namespace genetics {

template< typename Space, typename Marker>
class Loader
{
public:

	using coord_type = Space;
	using marker_type = Marker;
	using allelic_state_type = typename marker_type::value_type;
	using allele_type = Allele<allelic_state_type>;
	using individual_type = DiploidIndividual<allele_type>;
	using return_type = SpatialGeneticSample<coord_type, individual_type>;

	SpatialGeneticSample<coord_type, individual_type> read(std::string const& path){
		if( !is_readable(path)) {	throw std::string("In GeneticsLoader::read : unable to read file");}
		analyze(path);
		return m_sample;
	}

private:

	const unsigned int m_ploidy = 2;
	std::vector<std::string> m_loci_names;
	SpatialGeneticSample<coord_type, individual_type> m_sample;

	bool is_readable( std::string const& path ) const{
		ifstream file( path.c_str() );
		return !file.fail();
	}

// String extractions, dependant on file structure
	void analyze(std::string const& path) {
		std::ifstream file(path);
   		std::string line;
   		unsigned int current_line = 0;

   		while ( std::getline( file, line ) ){
   			if(current_line == 0){
   				m_loci_names = read_loci_names(erase_coordinates(line));
   			}else{
   				auto x = read_coordinates(line);
   				auto ind = read_individual(erase_coordinates(line));
   				m_sample.add(x, ind);
   			}
   			++current_line;
    	}
	}

	std::string& erase_coordinates(std::string &line) const{
		for(int i = 0; i<=1; ++i){
			std::size_t found = line.find(' ');
			line.erase(0, ++found);
		}
		return line;
	}

	std::vector<std::string> read_loci_names(std::string const& line) const{
		std::vector<std::string> names;
		std::istringstream iss( line );
		std::string elem;
    while ( std::getline( iss, elem, ' ' ) )
		{
			names.push_back(elem);
    }
		assert(!names.empty());
	  return names;
	}

	coord_type read_coordinates(std::string const& line) const {
	    double lat = 0.;
	    double lon = 0.;
	    std::istringstream iss( line );
	    string elem;
	    unsigned int pos = 0;
	    while ( std::getline( iss, elem, ' ' ) && pos <= 1 ) {
	    	if(pos == 0){
	    		lat = std::stod(elem);
	    	}else if( pos == 1){
	    		lon = std::stod(elem);
	    	}
	    	++pos;
	    }
	    coord_type x(lat, lon);
	    return x;
	}


	individual_type read_individual(std::string const& line) const{

	    std::istringstream iss( line );
	    string elem;

	    individual_type individual;

	    std::pair<allele_type, allele_type> alleles;

	    unsigned int i_locus = 0;
	    unsigned int i_allele = 0;
	    while( std::getline(iss, elem, ' ') ){

	    	if(i_allele == 0){

	    		alleles.first = std::stod(elem);
	    		++i_allele;

	    	}else if( i_allele == 1){

	    		alleles.second = std::stod(elem);
	    		std::string locus_name = m_loci_names.at(i_locus);
		    	individual.add(locus_name, alleles);

				i_allele = 0;
				++i_locus;
	    	}

		}
		return individual;
	}
};

}
}

#endif
