// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __RESOLUTION_H_INCLUDED
#define __RESOLUTION_H_INCLUDED

namespace quetzal {
namespace geography {
	/*!
	 * \brief %Resolution of a spatial grid
	 *
	 * \ingroup geography
	 * \section Example
	 * \snippet geography/test/Resolution/Resolution_test.cpp Example
	 * \section Output
	 * \include geography/test/Resolution/Resolution_test.output
	 */
	template<typename T>
	class Resolution{

	public:

	  //! \typedef type used to represent the values of  latitude and longitude
		using value_type = T;

	  /**
	    * \brief Constructor
	    *
			* \param lat the resolution of the spatial grid latitude
	    * \param lon the resolution of the spatial grid longitude
	    * \section Example
	    * \snippet geography/test/Resolution/Resolution_test.cpp Example
	    * \section Output
	    * \include geography/test/Resolution/Resolution_test.output
	    */
	  explicit Resolution(value_type lat, value_type lon) : m_lat(lat), m_lon(lon) {}

	  /**
	    * \brief Gets latitude resolution
	    *
	    * \section Example
	    * \snippet geography/test/Resolution/Resolution_test.cpp Example
	    * \section Output
	    * \include geography/test/Resolution/Resolution_test.output
	    */
	  value_type lat() const {
	    return m_lat;
	  }

		/**
	    * \brief Gets longitude resolution
	    *
	    * \section Example
	    * \snippet geography/test/Resolution/Resolution_test.cpp Example
	    * \section Output
	    * \include geography/test/Resolution/Resolution_test.output
	    */
	  value_type lon() const {
	    return m_lon;
	  }

		/**
	    * \brief Sets latitude resolution
	    *
			* \param value the new value for latitude resolution.
			* \return a reference on the Resolution object.
			*
	    * \section Example
	    * \snippet geography/test/Resolution/Resolution_test.cpp Example
	    * \section Output
	    * \include geography/test/Resolution/Resolution_test.output
	    */
	  Resolution& lat(value_type value) {
	    m_lat = value;
			return *this;
	  }

	  /**
	    * \brief Sets longitude resolution
	    *
			* \param value the new value for longitude resolution.
			* \return a reference on the Resolution object.
			*
	    * \section Example
	    * \snippet geography/test/Resolution/Resolution_test.cpp Example
	    * \section Output
	    * \include geography/test/Resolution/Resolution_test.output
	    */
	  Resolution& lon(value_type value) {
	    m_lon = value;
			return *this;
	  }

	  /**
	    * \brief EqualityComparable
	    *
	    * Checks if two resolutions objects are equals
	    * \return true if latitude and longitude have same resolution, else returns false.
	    * \section Example
	    * \snippet geography/test/Resolution/Resolution_test.cpp Example
	    * \section Output
	    * \include geography/test/Resolution/Resolution_test.output
	    */
	  bool operator==(const Resolution& other) const {
	      if(m_lat == other.lat() && m_lon == other.lon() ) return true;
	      return false;
	    }

		/**
	    * \brief Unequality comparison operator
	    *
	    * Checks if two resolutions objects are equals
	    * \return false if latitude and longitude have same resolution, else returns true.
	    * \section Example
	    * \snippet geography/test/Resolution/Resolution_test.cpp Example
	    * \section Output
	    * \include geography/test/Resolution/Resolution_test.output
	    */
	  bool operator!=(const Resolution& other) const {
	     return !(operator==(other));
	    }

	  private:
			value_type m_lat;
      value_type m_lon;

	  };

} // namespace quetzal
} // namespace geography

#endif
