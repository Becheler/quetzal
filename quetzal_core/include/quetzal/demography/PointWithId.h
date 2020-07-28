// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __POINT_WITH_ID_H_INCLUDED__
#define __POINT_WITH_ID_H_INCLUDED__

namespace quetzal {
namespace demography{
namespace strategy {

/*!
 * Find the index of a point in a vector
 * @param  p    the point to find
 * @param  vect the vector in points in which to search for the point index
 * @return      the index of the point in the vector
 */
template<typename Point>
size_t getIndexOfPointInVector(Point const& p, std::vector<Point> const& vect) {
	auto it = std::find(vect.begin(), vect.end(), p);
	assert(it != vect.end());
	return std::distance(vect.begin(), it);
}

/*!
 * Class to associate an indexing semantics to a collection of points
 *
 * @details it is mainly used when geographic coordinates are stored in a vector
 *          and that a pairwise matrix has to be computed: PointWithId class make
 *          easy to switch between geographic and indexing semantics.
 * @tparam Point the class for which an indexing semantics is needed
 */
template<typename Point>
class PointWithId {

public:

	PointWithId(std::vector<Point> const& vect, size_t index) :
		m_vector{vect}, m_index{index}
	{}

	PointWithId(std::vector<Point> const& vect, Point const& p) :
		m_vector{vect}, m_index{getIndexOfPointInVector(p, vect)}
	{}

	Point  getPoint() const { return m_vector[m_index]; }

	size_t getId()    const { return m_index; }

	bool operator==(PointWithId const& other) const {
		return getPoint() == other.getPoint();
	}

	bool operator!=(PointWithId const& other) const {
		return !(*this == other);
	}

private:
	std::vector<Point> const& m_vector;
	size_t                    m_index;
};

}
}
}

#endif
