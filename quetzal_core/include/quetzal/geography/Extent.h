// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __EXTENT_H_INCLUDED
#define __EXTENT_H_INCLUDED

namespace quetzal {
namespace geography {

/*!
 * \brief %Extent of a raster grid object
 *
 * \ingroup geography
 * \section Example
 * \snippet geography/test/Extent/Extent_test.cpp Example
 * \section Output
 * \include geography/test/Extent/Extent_test.output
 */
template<typename T>
class Extent{

public:

	//! \typedef type used to represent the values of longitude and latitude
	using value_type = T;

	/**
		* \brief Constructor
		*
		* \param lon_min the minimal longitude of the spatial grid
		* \param lon_max the mmaximal longitude of the spatial grid
		* \param lat_min the minimal latitude of the spatial grid
		* \param lat_max the minimal longitude of the spatial grid
		* \section Example
		* \snippet geography/test/Extent/Extent_test.cpp Example
		* \section Output
		* \include geography/test/Extent/Extent_test.output
		*/
	Extent(value_type lat_min, value_type lat_max, value_type lon_min, value_type lon_max):
	m_lat_min(lat_min),
	m_lat_max(lat_max),
	m_lon_min(lon_min),
	m_lon_max(lon_max)
	{}

	/**
		* \brief Equality comparison
		*
		* \param other The other Extent object to be compared
		* \return true if the two objects have same lon_min, lon_max, lat_min, lat_max. Else returns false.
		* \section Example
		* \snippet geography/test/Extent/Extent_test.cpp Example
		* \section Output
		* \include geography/test/Extent/Extent_test.output
		*/
	bool operator==(const Extent& other) const {
	    if(lon_min() == other.lon_min() &&
			   lon_max() == other.lon_max() &&
				 lat_min() == other.lat_min() &&
				 lat_max() == other.lat_max() )
			{
				return true;
			}
	    return false;
	  }

		/**
			* \brief Unequality comparison
			*
			* \param other The other Extent object to be compared
			* \return false if the two objects have same lon_min, lon_max, lat_min, lat_max. Else returns true.
			* \section Example
			* \snippet geography/test/Extent/Extent_test.cpp Example
			* \section Output
			* \include geography/test/Extent/Extent_test.output
			*/
		bool operator!=(const Extent& other) const {
		    return !(operator==(other));
		  }

		/**
			* \brief Get the minimal latitude of the spatial extent
			*
			* \section Example
			* \snippet geography/test/Extent/Extent_test.cpp Example
			* \section Output
			* \include geography/test/Extent/Extent_test.output
			*/
		value_type lat_min() const {return m_lat_min; }

		/**
			* \brief Get the maximal latitude of the spatial extent
			*
			* \section Example
			* \snippet geography/test/Extent/Extent_test.cpp Example
			* \section Output
			* \include geography/test/Extent/Extent_test.output
			*/
		value_type lat_max() const {return m_lat_max; }

		/**
			* \brief Get the minimal longitude of the spatial extent
			*
			* \section Example
			* \snippet geography/test/Extent/Extent_test.cpp Example
			* \section Output
			* \include geography/test/Extent/Extent_test.output
			*/
		value_type lon_min() const {return m_lon_min; }

		/**
			* \brief Get the maximal longitude of the spatial extent
			*
			* \section Example
			* \snippet geography/test/Extent/Extent_test.cpp Example
			* \section Output
			* \include geography/test/Extent/Extent_test.output
			*/
		value_type lon_max() const {return m_lon_max; }


		/**
			* \brief Set the minimal latitude of the spatial extent
			*
			* \return a reference on the Extent object
			*
			* \section Example
			* \snippet geography/test/Extent/Extent_test.cpp Example
			* \section Output
			* \include geography/test/Extent/Extent_test.output
			*/
		Extent& lat_min(value_type val) {
			m_lat_min = val;
			return *this;
		}

		/**
			* \brief Set the maximal longitude of the spatial extent
			*
			* \return a reference on the Extent object
			*
			* \section Example
			* \snippet geography/test/Extent/Extent_test.cpp Example
			* \section Output
			* \include geography/test/Extent/Extent_test.output
			*/
		Extent& lat_max(value_type val) {
			m_lat_max = val ;
			return *this;
		}

		/**
			* \brief Set the minimal longitude of the spatial extent
			*
			* \return a reference on the Extent object
			*
			* \section Example
			* \snippet geography/test/Extent/Extent_test.cpp Example
			* \section Output
			* \include geography/test/Extent/Extent_test.output
			*/
		Extent& lon_min(value_type val) {
			m_lon_min = val ;
			return *this;
		}

		/**
			* \brief Set the maximal longitude of the spatial extent
			*
			* \return a reference on the Extent object
			*
			* \section Example
			* \snippet geography/test/Extent/Extent_test.cpp Example
			* \section Output
			* \include geography/test/Extent/Extent_test.output
			*/
		Extent& lon_max(value_type val) {
			m_lon_max = val ;
			return *this;
		}

	private:
		value_type m_lat_min;
		value_type m_lat_max;
		value_type m_lon_min;
		value_type m_lon_max;

  };

} // namespace quetzal
} // namespace geography

#endif
