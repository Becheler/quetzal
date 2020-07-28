// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __DISCRETE_LANDSCAPE_H_INCLUDED
#define __DISCRETE_LANDSCAPE_H_INCLUDED

#include <map>
#include <set>
#include <assert.h>
#include <stdexcept>
#include <algorithm>
#include <tuple>      // std::forward_as_tuple
#include <functional> // std::cref

#include "EnvironmentalQuantity.h"

namespace quetzal {
	namespace geography {

/*!
 * \brief Discretized representation of a landscape and its ecological
 *        quantities through time and space
 *
 * A geographic explicit landscape is represented by a set of geographic coordinates
 * at which are defined the values of a defined set of environmental quantities (typically bioclimatic variables).
 * This geo-spatialized database is constructed from raster files and gives strong
 * guarantees on data consistency.
 *
 * \tparam Key      Ecological quantities identifier
 * \tparam Time     Time type
 * \ingroup geography
 * \section Example
 * \snippet geography/test/DiscreteLandscape/DiscreteLandscape_test.cpp Example
 * \section Output
 * \include geography/test/DiscreteLandscape/DiscreteLandscape_test.output
 */
template<typename Key, typename Time>
class DiscreteLandscape
{

	using geo_data_type = EnvironmentalQuantity<Time>;

public:

	//! \typedef ecological quantities identifier type
	using key_type = Key;

	//! \typedef coordinate type
	using coord_type = typename geo_data_type::coord_type;

	//! \typedef time type
	using time_type = Time;

	//! \typedef decimal degree type
	using decimal_degree = typename geo_data_type::decimal_degree;

private:

	std::map<key_type, geo_data_type> m_quantities;

	void build(std::map<key_type, std::string> const& files, std::set<Time> times){
		for(auto const& it : files){
			m_quantities.emplace( std::piecewise_construct,
				                    std::forward_as_tuple(it.first),
														std::forward_as_tuple(it.second, std::vector<Time>(times.begin(), times.end())));
		}
	}

public:

	template<typename F, typename G>
	void export_to_geotiff(F f, time_type const& t1, time_type const& t2, G g, std::string const& filename) const {
		m_quantities.cbegin()->second.export_to_geotiff(f,g, t1, t2, filename);
	}

	void export_to_shapefile(std::map<coord_type, unsigned int> counts, std::string const& filename) const {
		m_quantities.cbegin()->second.export_to_shapefile(counts, filename);
	}

	void export_to_shapefile(std::vector<coord_type> points, std::string const& filename) const {
		m_quantities.cbegin()->second.export_to_shapefile(points, filename);
	}
	
	/**
	  * \brief Constructor
		*
		* \param files a map giving as keys the identifier of the ecological quantity,
		*        and as value a std::string giving the raster file to read.
		* \param times the time values associated to each layer of the file: here a raster file represents
		*        an environmental quantity where the i-th layer represents the values across space at the i-th time.
		* \remark error will be thrown if the datasets do not have the exactly same geographical properties
		*         (origin, extent, resolution, number of layers).
		* \remark error will be thrown if data do not have same geographic definition space
		*         (coordinates of cells with defined value)
		* \remark All quantities types are casted to double.
		* \section Example
		* \snippet geography/test/DiscreteLandscape/DiscreteLandscape_test.cpp Example
		* \section Output
		* \include geography/test/DiscreteLandscape/DiscreteLandscape_test.output
		*/
	DiscreteLandscape(std::map<key_type, std::string> const& files, std::set<Time> const& times){

		build(files, times);

		auto have_same_origin = [this](auto const& it){return it.second.origin() == m_quantities.cbegin()->second.origin();};
		if( ! all_of(m_quantities.cbegin(), m_quantities.cend(), have_same_origin)){
			throw std::runtime_error("all ecological quantities dataset must have same origin");
		}

		auto have_same_extent = [this](auto const& it){return it.second.extent() == m_quantities.cbegin()->second.extent();};
		if( ! all_of(m_quantities.cbegin(), m_quantities.cend(), have_same_extent)){
			throw std::runtime_error("all ecological quantities dataset must have same extent");
		}

		auto have_same_resolution = [this](auto const& it){return it.second.resolution() == m_quantities.cbegin()->second.resolution();};
		if( ! all_of(m_quantities.cbegin(), m_quantities.cend(), have_same_resolution)){
			throw std::runtime_error("all ecological quantities dataset must have same resolution");
		}

		auto have_same_depth = [this](auto const& it){return it.second.depth() == m_quantities.cbegin()->second.depth();};
		if( ! all_of(m_quantities.cbegin(), m_quantities.cend(), have_same_depth)){
			throw std::runtime_error("all ecological quantities dataset must have same depth");
		}

		auto have_same_domain = [this](auto const& it){
			auto const& domain = it.second.geographic_definition_space();
			return std::equal(domain.cbegin(),
												domain.cend(),
												m_quantities.cbegin()->second.geographic_definition_space().cbegin());
		};
		if( ! all_of(m_quantities.cbegin(), m_quantities.cend(), have_same_domain)){
			throw std::runtime_error("all ecological quantities dataset must have same domain");
		}
	}

	/**
	  * \brief Retrieves the number of ecological quantities.
		* \return the number of ecological quantities.
		* \section Example
		* \snippet geography/test/DiscreteLandscape/DiscreteLandscape_test.cpp Example
		* \section Output
		* \include geography/test/DiscreteLandscape/DiscreteLandscape_test.output
		*/
	auto quantities_nbr() const {
		return m_quantities.size();
	}

	/**
		* \brief Accesses the specified ecological quantity
		* \return a function-object with signature `value_type fun(coord_type const& x, time_type t)`
		*         which when inovked will return the value of the specified quantity.
		* \param k the identifier of teh quantity to be retrieved
		* \remark the function-obect is invalidated if the DiscreteLandscape is destroyed.
		* \remark the returned lightweight object can be used for mathematical function composition using the expressive module.
		* \section Example
		* \snippet geography/test/DiscreteLandscape/DiscreteLandscape_test.cpp Example
		* \section Output
		* \include geography/test/DiscreteLandscape/DiscreteLandscape_test.output
		*/
	auto operator [](key_type const& k) const {
		assert(m_quantities.find(k) != m_quantities.end());
		auto c = std::cref(m_quantities.at(k));
		return [c](coord_type const& x, time_type t){return c.get().at(x,t); };
	}

	/**
		* \brief read the times at which quantity is defined.
		* \return the ordered times that have been specified at construction
		* \section Example
		* \snippet geography/test/DiscreteLandscape/DiscreteLandscape_test.cpp Example
		* \section Output
		* \include geography/test/DiscreteLandscape/DiscreteLandscape_test.output
		*/
		const std::vector<time_type>& temporal_definition_space() const {
		return m_quantities.cbegin()->second.temporal_definition_space();
	}

		/**
		* \brief The geographic coordinates of the centroids of the demes for which
		* values are defined for all ecological quantities at all times.
		*
		* Returns the geographic coordinates of the centroids of the demes for which
		* values are defined for all ecological quantities at all times.
		* Values can be read from other coordinates, but this function provides a natural
		* way to construct the geographic support for a demic structure (for example for demographic simulations).
		*
		* \section Example
		* \snippet geography/test/DiscreteLandscape/DiscreteLandscape_test.cpp Example
		* \section Output
		* \include geography/test/DiscreteLandscape/DiscreteLandscape_test.output
		*/
	const std::vector<coord_type> & geographic_definition_space() const {
		return m_quantities.cbegin()->second.geographic_definition_space();
	}

	/**
		* \brief Retrieves the geographic coordinate of the top left corner of the landscape
		* \return the geographic coordinate of the top left corner of the landscape
		* \section Example
		* \snippet geography/test/DiscreteLandscape/DiscreteLandscape_test.cpp Example
		* \section Output
		* \include geography/test/DiscreteLandscape/DiscreteLandscape_test.output
		*/
	const coord_type & origin() const {
		return m_quantities.cbegin()->second.origin();
	}

	/**
		* \brief Gets the resolution (pixel dimensions) of the grid landscape
		* \return a Resolution object
		* \section Example
		* \snippet geography/test/DiscreteLandscape/DiscreteLandscape_test.cpp Example
		* \section Output
		* \include geography/test/DiscreteLandscape/DiscreteLandscape_test.output
		*/
	const Resolution<decimal_degree> & resolution() const {
		return m_quantities.cbegin()->second.resolution();
	}

	/**
		* \brief Gets the geographic extent of the landscape
		* \return an Extent object
		* \section Example
		* \snippet geography/test/DiscreteLandscape/DiscreteLandscape_test.cpp Example
		* \section Output
		* \include geography/test/DiscreteLandscape/DiscreteLandscape_test.output
		*/
	const Extent<decimal_degree> & extent() const {
		return m_quantities.cbegin()->second.extent();
	}

	/**
		* \brief  Check if a given geographic coordinate is in the landscpae spatial extent
		* \return true if `c` is in the landscape limits, else false
		* \param c the coordinate to be tested
		* \section Example
		* \snippet geography/test/DiscreteLandscape/DiscreteLandscape_test.cpp Example
		* \section Output
		* \include geography/test/DiscreteLandscape/DiscreteLandscape_test.output
		*/
	bool is_in_spatial_extent(coord_type const& c) const {
		return m_quantities.cbegin()->second.is_in_spatial_extent(c);
	}

	/**
		* \brief Reprojects a coordinate to the centroid of the landscape cell to which it belongs
		* \return the geograhic coordinate of the cell centroid
		* \param c the coordinate to be reprojected
		* \remark the given coordinate must be in the landscape spatial extent
		* \section Example
		* \snippet geography/test/DiscreteLandscape/DiscreteLandscape_test.cpp Example
		* \section Output
		* \include geography/test/DiscreteLandscape/DiscreteLandscape_test.output
		*/
	coord_type reproject_to_centroid(coord_type const& c) const {
		return m_quantities.cbegin()->second.reproject_to_centroid(c);
	}

	std::vector<coord_type> four_nearest_defined_cells(coord_type const& x) const {
		auto const& X = geographic_definition_space();

		auto x0 = reproject_to_centroid(x);
	  assert( std::find(X.begin(), X.end(), x0) != X.end());

		auto res = resolution();
	  std::vector<coord_type> v;

	  coord_type x1(x0);
	  x1.lon() += res.lon();
		if(is_in_spatial_extent(x1)){
			x1 = reproject_to_centroid(x1);
		  if(std::find(X.begin(), X.end(), x1) != X.end()){
				v.push_back(x1);
			}
		}

	  coord_type x2(x0);
	  x2.lat() += res.lat();
		if(is_in_spatial_extent(x2)){
			x2 = reproject_to_centroid(x2);
		  if(std::find(X.begin(), X.end(), x2) != X.end()){
				v.push_back(x2);
			}
		}

	  coord_type x3(x0);
	  x3.lon() -= res.lon();
		if(is_in_spatial_extent(x3)){
			x3 = reproject_to_centroid(x3);
		  if(std::find(X.begin(), X.end(), x3) != X.end()){
				v.push_back(x3);
			}
		}

	  coord_type x4(x0);
	  x4.lat() -= res.lat() ;
		if(is_in_spatial_extent(x4)){
			x4 = reproject_to_centroid(x4);
		  if(std::find(X.begin(), X.end(), x4) != X.end()){
				v.push_back(x4);
			}
		}

	  return v;
	}


};

} // geography
} // quetzal

#endif
