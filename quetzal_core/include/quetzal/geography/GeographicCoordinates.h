// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __GEOGRAPHIC_COORDINATES_H_INCLUDED__
#define __GEOGRAPHIC_COORDINATES_H_INCLUDED__

#include <assert.h>
#include <cmath> // trigonometry
#include <string> // error
#include <ostream>
#include <unordered_map>

namespace quetzal {
namespace geography {

/*!
 * \brief Geographic coordinates.
 *
 * Geographic coordinates (longitude and latitude) in decimal degree.
 *
 * \ingroup geography
 * \section Example
 * \snippet geography/test/GeographicCoordinates/GeographicCoordinates_test.cpp Example
 * \section Output
 * \include geography/test/GeographicCoordinates/GeographicCoordinates_test.output
 */
class GeographicCoordinates{

public:

	//! \typedef latitude and longitude value type
	using decimal_degree = double;

	//! \typedef the great circle distance value type
	using km = double;

	GeographicCoordinates() = default;

	/*!
	 * \brief Constructor
	 *
	 * Default Construct a GeographicCoordinate with given longitude and latitude.
	 *
	 * \param lat the latitude value
	 * \param lon the longitude value
	 * \remark Exception will be thrown if the given values are aberrant.
	 * \section Example
	 * \snippet geography/test/GeographicCoordinates/GeographicCoordinates_test.cpp Example
	 * \section Output
	 * \include geography/test/GeographicCoordinates/GeographicCoordinates_test.output
	 */
	 GeographicCoordinates(decimal_degree lat, decimal_degree lon) : m_lat(lat), m_lon(lon){
		if( (m_lat > 90.) || (m_lat < -90.) || (m_lon < -180.) || (m_lon > 180.))
			throw std::string("invalid coordinates" );
	};

	/*!
	 * \brief Compute great-circle distance between two points on the Earth.
	 *
	 * Compute great circle distance in kilometers to an other geographic coordinate.
	 * Implementation follows a direct translation of
	 * <a href="http://en.wikipedia.org/wiki/Haversine_formula">Haversine formula</a>.
	 *
	 * \param other the other coordinate
	 * \return the distance in kilometers
	 * \section Example
	 * \snippet geography/test/GeographicCoordinates/GeographicCoordinates_test.cpp Example
	 * \section Output
	 * \include geography/test/GeographicCoordinates/GeographicCoordinates_test.output
	 */
	km great_circle_distance_to(GeographicCoordinates const& other) const {
		km d = distanceEarth(this->m_lat, this->m_lon, other.m_lat, other.m_lon);
		assert(d >= 0.);
		return d;
	}


	/*!
	 * \brief Gets latitude.
	 *
	 * Gets latitude value.
	 *
	 * \return the latitude value
	 * \section Example
	 * \snippet geography/test/GeographicCoordinates/GeographicCoordinates_test.cpp Example
	 * \section Output
	 * \include geography/test/GeographicCoordinates/GeographicCoordinates_test.output
	 */
	 decimal_degree lat() const {return m_lat;}

	 /*!
 	 * \brief Gets latitude.
 	 *
 	 * Gets latitude value reference.
 	 *
 	 * \return a reference the latitude value
 	 * \section Example
 	 * \snippet geography/test/GeographicCoordinates/GeographicCoordinates_test.cpp Example
 	 * \section Output
 	 * \include geography/test/GeographicCoordinates/GeographicCoordinates_test.output
 	 */
	 decimal_degree& lat() {return m_lat;}


	 /*!
		* \brief Set latitude.
		*
		* Set latitude value.
		*
		* \return a reference on the modified object
		* \section Example
		* \snippet geography/test/GeographicCoordinates/GeographicCoordinates_test.cpp Example
		* \section Output
		* \include geography/test/GeographicCoordinates/GeographicCoordinates_test.output
		*/
	 GeographicCoordinates& lat(decimal_degree value) {
		 m_lat = value;
		 return *this;
	 }

	/*!
	 * \brief Get longitude.
	 *
	 * Get longitude value.
	 *
	 * \return the longitude value
	 * \section Example
	 * \snippet geography/test/GeographicCoordinates/GeographicCoordinates_test.cpp Example
	 * \section Output
	 * \include geography/test/GeographicCoordinates/GeographicCoordinates_test.output
	 */
	decimal_degree lon() const {return m_lon;}

	/*!
	 * \brief Get reference on longitude.
	 *
	 * Get reference on longitude value.
	 *
	 * \return a reference on the longitude value
	 * \section Example
	 * \snippet geography/test/GeographicCoordinates/GeographicCoordinates_test.cpp Example
	 * \section Output
	 * \include geography/test/GeographicCoordinates/GeographicCoordinates_test.output
	 */
	decimal_degree& lon() {return m_lon;}

	/*!
	 * \brief Set longitude.
	 *
	 * Set longitude value.
	 *
	 * \return a reference on the modified object
	 * \section Example
	 * \snippet geography/test/GeographicCoordinates/GeographicCoordinates_test.cpp Example
	 * \section Output
	 * \include geography/test/GeographicCoordinates/GeographicCoordinates_test.output
	 */
	GeographicCoordinates& lon(decimal_degree value) {
		m_lon = value;
		return *this;
	}

	/*!
	 * \brief Comparison operator
	 *
	 * Establishes a strict total order relation between geographic coordinates.
	 *
	 * \remark allow GeographicCoordinates to be used in map
	 * \section Example
	 * \snippet geography/test/GeographicCoordinates/GeographicCoordinates_test.cpp Example
	 * \section Output
	 * \include geography/test/GeographicCoordinates/GeographicCoordinates_test.output
	 */
	bool operator<(const GeographicCoordinates& other) const {

    	if(m_lat < other.m_lat) return true;
  		if(m_lat > other.m_lat) return false;

    	//lat == coord.lat

    	if(m_lon < other.m_lon) return true;
    	if(m_lon > other.m_lon) return false;

    	//lat == coord.lat && lon == coord.lon

    	return false;
	}

	/*!
	 * \brief Equality comparison operator
	 *
	 * Compares two coordinates by their latitude and longitude.
	 *
	 * \return true if longitude and latitude are equal, else returns false.
	 * \section Example
	 * \snippet geography/test/GeographicCoordinates/GeographicCoordinates_test.cpp Example
	 * \section Output
	 * \include geography/test/GeographicCoordinates/GeographicCoordinates_test.output
	 */
	bool operator==(const GeographicCoordinates& other) const {
    	if(m_lat == other.m_lat && m_lon == other.m_lon) return true;
    	return false;
    }

		/*!
		 * \brief Unequality comparison operator
		 *
		 * Compares two coordinates by their latitude and longitude.
		 *
		 * \return false if longitude and latitude are equal, else returns true.
		 * \section Example
		 * \snippet geography/test/GeographicCoordinates/GeographicCoordinates_test.cpp Example
		 * \section Output
		 * \include geography/test/GeographicCoordinates/GeographicCoordinates_test.output
		 */
		bool operator!=(const GeographicCoordinates& other) const {
	    	return !(operator==(other));
	    }

private:

	decimal_degree m_lat;
	decimal_degree m_lon;


	// Converts decimal degrees to radians
	double deg2rad(double deg) const {
		double pi = 3.14159265358979323846;
 		return (deg * pi / 180);
	}


	// Converts radians to decimal degrees
	double rad2deg(double rad) const {

		double pi = 3.14159265358979323846;
		return (rad * 180 / pi);
	}

	/**
 	* Returns the distance between two points on the Earth.
	* Direct translation from http://en.wikipedia.org/wiki/Haversine_formula
 	* @param lat1d Latitude of the first point in degrees
	* @param lon1d Longitude of the first point in degrees
 	* @param lat2d Latitude of the second point in degrees
 	* @param lon2d Longitude of the second point in degrees
 	* @return The distance between the two points in kilometers
 	*/
	km distanceEarth(double lat1d, double lon1d, double lat2d, double lon2d) const {

	double earthRadiusKm = 6371.0;
  	double lat1r, lon1r, lat2r, lon2r, u, v;

  	lat1r = deg2rad(lat1d);
  	lon1r = deg2rad(lon1d);
  	lat2r = deg2rad(lat2d);
  	lon2r = deg2rad(lon2d);

  	u = sin((lat2r - lat1r)/2);
  	v = sin((lon2r - lon1r)/2);

	km d = 2.0 * earthRadiusKm * asin(sqrt(u * u + cos(lat1r) * cos(lat2r) * v * v));

	assert(d >= 0);

  	return d;
	}

};


std::ostream& operator <<(std::ostream& stream, GeographicCoordinates const& coord)
{
    stream << "(" << coord.lat() << " " << coord.lon() << ")";
    return stream;
}


template <class T>
inline void hash_combine(std::size_t & seed, const T & v)
{
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

} // namespace geography

} // namespace quetzal

namespace std
{
  template<> struct hash<quetzal::geography::GeographicCoordinates>
  {
    inline size_t operator()(const quetzal::geography::GeographicCoordinates & v) const
    {
      size_t seed = 0;
      quetzal::geography::hash_combine(seed, v.lon());
      quetzal::geography::hash_combine(seed, v.lat());
      return seed;
    }
  };
}

namespace quetzal{
namespace geography{

	struct MyHash
  {
  	std::size_t operator()(const std::pair<GeographicCoordinates, GeographicCoordinates>& param) const
  	{
  		static const auto hacher = std::hash<GeographicCoordinates>();
  		const std::size_t h1 = hacher(param.first);
  		const std::size_t h2 = hacher(param.second);
  		return h1 ^ (h2 << 1);
  	}
  };

	GeographicCoordinates::km memoized_great_circle_distance(GeographicCoordinates const& x, GeographicCoordinates const& y)
	{
		static std::unordered_map<std::pair<GeographicCoordinates, GeographicCoordinates>, GeographicCoordinates::km, MyHash> cache;

		const auto pair = std::make_pair(x, y);
		const auto it = cache.find(pair);
		if(it != cache.end())
			return it->second;
		const auto pair_itNewResult_true = cache.emplace(
			std::piecewise_construct,
			std::forward_as_tuple(pair),
			std::forward_as_tuple(x.great_circle_distance_to(y))
		);
		return pair_itNewResult_true.first->second;
	}
}
}

#endif
