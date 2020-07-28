// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __POPULATION_FLOW_H_INCLUDED__
#define __POPULATION_FLOW_H_INCLUDED__

#include <unordered_map>
#include <cassert>
#include <functional>
#include <algorithm>
#include <iostream>     // output operator

namespace quetzal{
namespace demography {


/*!
 * \brief Defines a template class to store the demographic flows number across a landscape along time.
 *
 * \tparam Space    Demes identifiers.
 * \tparam Time     EqualityComparable, CopyConstructible.
 * \tparam Value    The type of the population size variable (int, unsigned int, double...)
 * \ingroup demography
 * \section Example
 * \snippet demography/test/Flow/Flow_test.cpp Example
 * \section Output
 * \include demography/test/Flow/Flow_test.output
 */
template<typename Space, typename Time, typename Value>
class Flow
{

public:

	//! \typedef time type
	using time_type = Time;

	//! \typedf space type
	using coord_type = Space;

	//! \typedef type of the population size variable
	using value_type = Value;

	/**
	  * \brief Default constructor
		* \section Example
	  * \snippet demography/test/Flow/Flow_test.cpp Example
	  * \section Output
	  * \include demography/test/Flow/Flow_test.output
		*/
	Flow() = default;

	/**
	  * \brief Retrieves value of the flux from deme i to deme j at time t.
		* \section Example
	  * \snippet demography/test/Flow/Flow_test.cpp Example
	  * \section Output
	  * \include demography/test/Flow/Flow_test.output
		*/
	value_type flux_from_to(coord_type const& from, coord_type const& to, time_type t) const
	{
		assert(m_flows.find(key_type(t,from,to)) != m_flows.end());
		return m_flows.at(key_type(t,from,to));
	}

	/**
	  * \brief Retrieves value of the flux from deme i to deme j at time t.
		* \return a reference on the value, initialized with value_type default constructor
		* \section Example
	  * \snippet demography/test/Flow/Flow_test.cpp Example
	  * \section Output
	  * \include demography/test/Flow/Flow_test.output
		*/
	void set_flux_from_to(coord_type const& from, coord_type const& to, time_type t, value_type v){
		m_flows[key_type(t, from, to)] = v;
		m_reverse_flows[reverse_key_type(to, t)][from] = v;
	}


	void add_to_flux_from_to(coord_type const& from, coord_type const& to, time_type t, value_type v){
		m_flows[key_type(t, from, to)] += v;
		m_reverse_flows[reverse_key_type(to, t)][from] += v;
	}

	/**
	  * \brief Retrieves the distribution of the value of the flux converging to deme x at time t.
		* \return a const reference on an unordered_map with giving the origin deme and the value
		*         giving the value of the flux
		* \section Example
	  * \snippet demography/test/Flow/Flow_test.cpp Example
	  * \section Output
	  * \include demography/test/Flow/Flow_test.output
		*/
	std::unordered_map<coord_type, value_type> const & flux_to(coord_type const& x, time_type t) const
	{
		assert(flux_to_is_defined(x,t));
		return m_reverse_flows.at(reverse_key_type(x, t));
	}

	/**
	  * \brief Check if the distribution of the value of the flux converging to deme x at time t is defined
		* \return true if the distribution is defined, else returns false
		* \section Example
	  * \snippet demography/test/Flow/Flow_test.cpp Example
	  * \section Output
	  * \include demography/test/Flow/Flow_test.output
		*/
	bool flux_to_is_defined(coord_type const& to, time_type const& t) const
	{
		auto it = m_reverse_flows.find(reverse_key_type(to, t));
		return it != m_reverse_flows.end();
	}

	/**
		* \brief A class representing the spatio-temporal coordinates of the flow vector (time, origin and destination)
		* \return An iterator on an object of type map_type
		* \section Example
		* \snippet demography/test/Flow/Flow_test.cpp Example
		* \section Output
		* \include demography/test/Flow/Flow_test.output
		*/
	struct key_type
	{
		//! time of the dispersal event
		time_type time;

		//! origin of the flow
		coord_type from;

		//! destination of the flow
		coord_type to;

		key_type(time_type const& t, coord_type const& origin, coord_type const& destination):
		time(t),
		from(origin),
		to(destination)
		{}

		bool operator==(key_type const& other) const
		{
			return (
							 other.time == this->time &&
							 other.from == this->from &&
							 other.to == this->to
						 );
		}
	};

	/**
	  * \brief Read-only access to the migration history.
		* \return An iterator on an object of type map_type
		* \section Example
	  * \snippet demography/test/Flow/Flow_test.cpp Example
	  * \section Output
	  * \include demography/test/Flow/Flow_test.output
		*/
	auto begin() const
	{
		return m_flows.cbegin();
	}

	/**
	  * \brief Read-only access to the migration history.
		* \return An iterator on an object of type map_type
		* \section Example
	  * \snippet demography/test/Flow/Flow_test.cpp Example
	  * \section Output
	  * \include demography/test/Flow/Flow_test.output
		*/
	auto end() const
	{
		return m_flows.cend();
	}


private:

	struct reverse_key_type{
		time_type time;
		coord_type to;

		reverse_key_type(coord_type const& x, time_type const& t):
		time(t),
		to(x)
		{}

		bool operator==(reverse_key_type const& other) const
		{
			return ( other.time == this->time && other.to == this->to );
		}

	};

	struct reverse_key_hash : public std::unary_function<reverse_key_type, std::size_t>
	{
	 std::size_t operator()(const reverse_key_type& k) const
	 {
		 size_t res = 17;
		 res = res * 31 + std::hash<time_type>()( k.time );
		 res = res * 31 + std::hash<coord_type>()( k.to );
		 return res;
	 }
	};



	struct key_hash : public std::unary_function<key_type, std::size_t>
	{
	 std::size_t operator()(const key_type& k) const
	 {
		 size_t res = 17;
		 res = res * 31 + std::hash<time_type>()( k.time );
		 res = res * 31 + std::hash<coord_type>()( k.from );
		 res = res * 31 + std::hash<coord_type>()( k.to );
		 return res;
	 }
	};

public:

	//! \typedef type of the historical database
	using map_type = std::unordered_map<const key_type, value_type, key_hash>;

private:

	using reverse_flow_type =  std::unordered_map<const reverse_key_type, std::unordered_map<coord_type, value_type>, reverse_key_hash>;
	map_type m_flows;
	reverse_flow_type m_reverse_flows;

};


template<typename Space, typename Time, typename Value>
std::ostream& operator <<(std::ostream& stream, const Flow<Space, Time, Value>& flows)
{
	stream << "time" <<  "\t" << "from" <<  "\t"  << "to" << "\t" << "flow" << "\n";
  for(auto const& it : flows){
    stream << it.first.time << "\t" << it.first.from <<  "\t" << it.first.to << "\t" << it.second << "\n";
  }
    return stream;
}


} // demography
} // quetzal

#endif
