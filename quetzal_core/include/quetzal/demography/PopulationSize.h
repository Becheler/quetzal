// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __POPULATIONS_H_INCLUDED__
#define __POPULATIONS_H_INCLUDED__

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "assert.h"

namespace quetzal{
namespace demography {

/*!
 * \brief Population size distribution in time and space.
 *
 * \tparam Space    Demes identifiers.
 * \tparam Time     EqualityComparable, CopyConstructible.
 * \tparam Value    The type of the population size variable (int, unsigned int, double...)
 * \ingroup demography
 * \section Example
 * \snippet demography/test/PopulationSize/PopulationSize_test.cpp Example
 * \section Output
 * \include demography/test/PopulationSize/PopulationSize_test.output
 */
template<typename Space, typename Time, typename Value>
class PopulationSize
{

public:

	//! \typedef time type
	using time_type = Time;

	//! \typedef space type
	using coord_type = Space;

	//! \typedef type of the population size variable
	using value_type = Value;

	/**
	  * \brief Default constructor
		* \section Example
	  * \snippet demography/test/PopulationSize/PopulationSize_test.cpp Example
	  * \section Output
	  * \include demography/test/PopulationSize/PopulationSize_test.output
		*/
	PopulationSize() = default;

	/**
	  * \brief Checks if population size is defined at deme x at time t
		* \return True  if variable is defined, else false.
		* \section Example
	  * \snippet demography/test/PopulationSize/PopulationSize_test.cpp Example
	  * \section Output
	  * \include demography/test/PopulationSize/PopulationSize_test.output
		*/
	bool is_defined(coord_type const& x, time_type const& t) const;

	/**
	  * \brief Get population size value at deme x at time t
		* \section Example
	  * \snippet demography/test/PopulationSize/PopulationSize_test.cpp Example
	  * \section Output
	  * \include demography/test/PopulationSize/PopulationSize_test.output
		*/
	value_type get(coord_type const& x, time_type const& t) const;

	/**
	 * \brief Get population size at deme x at time t.
	 * \remark operator allows for more expressive mathematical style in client code
	 * \section Example
	 * \snippet demography/test/PopulationSize/PopulationSize_test.cpp Example
	 * \section Output
	 * \include demography/test/PopulationSize/PopulationSize_test.output
	 */
	value_type operator()(coord_type const& x, time_type const& t) const;

	/**
	 * \brief Population size at deme x at time t.
	 * \return a reference to the value, initialized with value_type default constructor
	 * \remark operator allows for more expressive mathematical style in client code
	 * \section Example
	 * \snippet demography/test/PopulationSize/PopulationSize_test.cpp Example
	 * \section Output
	 * \include demography/test/PopulationSize/PopulationSize_test.output
	 */
	value_type & operator()(coord_type const& x, time_type const& t);

	/**
	 * \brief Population size at deme x at time t.
	 * \return a reference to the value, initialized with value_type default constructor
	 * \remark operator allows for more expressive mathematical style in client code
	 * \section Example
	 * \snippet demography/test/PopulationSize/PopulationSize_test.cpp Example
	 * \section Output
	 * \include demography/test/PopulationSize/PopulationSize_test.output
	 */
	value_type & operator()(coord_type && x, time_type const& t);

	/**
	 * \brief Population size at deme x at time t.
	 * \return a reference to the value, initialized with value_type default constructor
	 * \remark operator allows for more expressive mathematical style in client code
	 * \section Example
	 * \snippet demography/test/PopulationSize/PopulationSize_test.cpp Example
	 * \section Output
	 * \include demography/test/PopulationSize/PopulationSize_test.output
	 */
	value_type & operator()(coord_type const& x, time_type && t);

	/**
	 * \brief Population size at deme x at time t.
	 * \return a reference to the value, initialized with value_type default constructor
	 * \remark operator allows for more expressive mathematical style in client code
	 * \section Example
	 * \snippet demography/test/PopulationSize/PopulationSize_test.cpp Example
	 * \section Output
	 * \include demography/test/PopulationSize/PopulationSize_test.output
	 */
	value_type & operator()(coord_type && x, time_type && t);

	/**
	  * \brief Set population size value at deme x at time t
		* \section Example
	  * \snippet demography/test/PopulationSize/PopulationSize_test.cpp Example
	  * \section Output
	  * \include demography/test/PopulationSize/PopulationSize_test.output
		*/
	void set(coord_type const& x, time_type const& t, value_type N);

	/**
	  * \brief Set population size value at deme x at time t
		* \section Example
	  * \snippet demography/test/PopulationSize/PopulationSize_test.cpp Example
	  * \section Output
	  * \include demography/test/PopulationSize/PopulationSize_test.output
		*/
	void set(coord_type && x, time_type const& t, value_type N);

	/**
		* \brief Set population size value at deme x at time t
		* \section Example
	  * \snippet demography/test/PopulationSize/PopulationSize_test.cpp Example
	  * \section Output
	  * \include demography/test/PopulationSize/PopulationSize_test.output
		*/
	void set(coord_type const& x, time_type && t, value_type N);

	/**
		* \brief Set population size value at deme x at time t
		* \section Example
	  * \snippet demography/test/PopulationSize/PopulationSize_test.cpp Example
	  * \section Output
	  * \include demography/test/PopulationSize/PopulationSize_test.output
		*/
	void set(coord_type && x, time_type && t, value_type N);

	/**
	  * \brief Return the demes at which the population size was defined at time t.
		* \section Example
		* \snippet demography/test/Populations/Populations_test.cpp Example
		* \section Output
		* \include demography/test/Populations/Populations_test.output
		*/
	std::vector<coord_type> definition_space(time_type const& t) const;

private:

	std::unordered_map<time_type, std::unordered_map<coord_type, value_type> > m_populations;

};


template<typename Space, typename Time, typename Value>
Value PopulationSize<Space, Time, Value>::operator()(Space const& x, Time const& t) const {
	return get(x,t);
}

template<typename Space, typename Time, typename Value>
Value & PopulationSize<Space, Time, Value>::operator()(Space const& x, Time const& t) {
	return m_populations[t][x];
}

template<typename Space, typename Time, typename Value>
Value & PopulationSize<Space, Time, Value>::operator()(Space && x, Time const& t) {
	return m_populations[t][std::move(x)];
}

template<typename Space, typename Time, typename Value>
Value & PopulationSize<Space, Time, Value>::operator()(Space const& x, Time && t) {
	return m_populations[std::move(t)][x];
}

template<typename Space, typename Time, typename Value>
Value & PopulationSize<Space, Time, Value>::operator()(Space && x, Time && t) {
	return m_populations[std::move(t)][std::move(x)];
}

template<typename Space, typename Time, typename Value>
bool PopulationSize<Space, Time, Value>::is_defined(Space const& x, Time const& t) const {
	return 	   (!m_populations.empty())
			&& (m_populations.find(t) != m_populations.end())
			&& (m_populations.at(t).find(x) != m_populations.at(t).end());
}

template<typename Space, typename Time, typename Value>
Value PopulationSize<Space, Time, Value>::get(Space const& x, Time const& t) const {
	assert( is_defined(x,t) );
	return m_populations.at(t).at(x);
}

template<typename Space, typename Time, typename Value>
void PopulationSize<Space, Time, Value>::set(Space const& x, Time const& t, value_type N) {
	m_populations[t][x] = N;
}

template<typename Space, typename Time, typename Value>
void PopulationSize<Space, Time, Value>::set(Space && x, Time const& t, value_type N) {
	m_populations[t][std::move(x)] = N;
}

template<typename Space, typename Time, typename Value>
void PopulationSize<Space, Time, Value>::set(Space const& x, Time && t, value_type N) {
	m_populations[std::move(t)][x] = N;
}

template<typename Space, typename Time, typename Value>
void PopulationSize<Space, Time, Value>::set(Space && x, Time && t, value_type N) {
	m_populations[std::move(t)][std::move(x)] = N;
}

template<typename Space, typename Time, typename Value>
std::vector<Space> PopulationSize<Space, Time, Value>::definition_space(Time const& t) const {
	std::vector<coord_type> v;
	for(auto const& it : m_populations.at(t) ){
		if(it.second > 0 ){v.push_back(it.first);}
	}
	return v;
}

} // namespace demography
} // namespace quetzal

#endif
