// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
************************************************************************/

#ifndef __DISCRETE_DISTRIBUTION_H_INCLUDED__
#define __DISCRETE_DISTRIBUTION_H_INCLUDED__

#include <assert.h>
#include <vector>
#include <random>

namespace quetzal {
namespace random {
/*!
 * \brief Sampling (non) arithmetic values in discrete probability distribution.
 *
 * Produces random State objects, where the probability of each individual State
 * \f$i\f$ is defined as \f$w = i/S\f$, that is the weight of the ith integer
 * divided by the sum of all \f$n\f$ weights.
 *
 * \ingroup random
 * \tparam State is the support of the probability distribution.
 * \section Example
 * \snippet random/test/DiscreteDistribution/DiscreteDistribution_test.cpp Example
 * \section Output
 * \include random/test/DiscreteDistribution/DiscreteDistribution_test.output
 */
template<typename State>
class DiscreteDistribution{

private:

	class Param{
	private:
		std::vector<State> m_support;
		std::vector<double> m_weights;
		using distribution_type = DiscreteDistribution<State>;

	public:
		/*! DefaultConstructible */
		Param():
		m_support(std::vector<State>(1)),
		m_weights(std::vector<double>(1,1.0))
		{}

		/*! CopyConstructible */
		Param(Param const& other) = default;

		Param(std::vector<State> const& support, std::vector<double> const& weights):
		m_support(support),
		m_weights(weights)
		{}

		Param(std::vector<State> && support, std::vector<double> && weights):
		m_support(std::move(support)),
		m_weights(std::move(weights))
		{}

		/*! CopyAssignable */
		Param& operator=(Param const& other) = default;
		/*! EqualityComparable */
		bool operator==(Param const& other){
			return (m_support == other.m_support && m_weights == other.m_weights) ;
		}

		std::vector<State> const& support() const { return m_support; }
		std::vector<double> const& weights() const { return m_weights; }

	};

	using SelfType = DiscreteDistribution<State>;
	Param m_param;

public:
	/*! \typedef State */
	using result_type = State;

	/*! \typedef the type of the parameter set, see
	 *  <a href="http://en.cppreference.com/w/cpp/concept/RandomNumberDistribution">RandomNumberDistribution</a>.
	 */
	using param_type = Param;

	/**
	  * \brief Default constructor
		* \remark result_type should be DefaultConstructible
		*
		* Construct a probability distribution which is a Dirac in the default-constructed object.
		* \section Example
	  * \snippet random/test/DiscreteDistribution/initialization_test.cpp Example
	  * \section Output
	  * \include random/test/DiscreteDistribution/initialization_test.output
		*/
	DiscreteDistribution() = default;

	/**
	  * \brief Constructor
		* \section Example
	  * \snippet random/test/DiscreteDistribution/initialization_test.cpp Example
	  * \section Output
	  * \include random/test/DiscreteDistribution/initialization_test.output
		*/
	explicit DiscreteDistribution(const param_type & p ) : m_param(p) {}

	/**
	  * \brief Copy constructor.
		* \section Example
	  * \snippet random/test/DiscreteDistribution/initialization_test.cpp Example
	  * \section Output
	  * \include random/test/DiscreteDistribution/initialization_test.output
		*/
	DiscreteDistribution(DiscreteDistribution<State> const& other) = default;

	/**
	  * \brief Move constructor.
		* \section Example
	  * \snippet random/test/DiscreteDistribution/initialization_test.cpp Example
	  * \section Output
	  * \include random/test/DiscreteDistribution/initialization_test.output
		*/
	DiscreteDistribution(DiscreteDistribution<State>&& other) = default;

	/**
		* \brief Constructor.
		* \section Example
		* \snippet random/test/DiscreteDistribution/initialization_test.cpp Example
		* \section Output
		* \include random/test/DiscreteDistribution/initialization_test.output
		*/
	DiscreteDistribution(std::vector<result_type> const& support, std::vector<double> const& weights) :
	m_param(support, weights)
	{}

	/**
		* \brief Constructor.
		* \section Example
		* \snippet random/test/DiscreteDistribution/initialization_test.cpp Example
		* \section Output
		* \include random/test/DiscreteDistribution/initialization_test.output
		*/
	DiscreteDistribution(std::vector<result_type> && support, std::vector<double> && weights) noexcept :
	m_param(std::move(support), std::move(weights))
	{}

	/**
	  * \brief Copy assignment operator.
		* \section Example
	  * \snippet random/test/DiscreteDistribution/initialization_test.cpp Example
	  * \section Output
	  * \include random/test/DiscreteDistribution/initialization_test.output
		*/
	DiscreteDistribution<State>& operator=(DiscreteDistribution<State> const& other) = default;

	/**
	  * \brief Move assignment operator.
		* \section Example
	  * \snippet random/test/DiscreteDistribution/initialization_test.cpp Example
	  * \section Output
	  * \include random/test/DiscreteDistribution/initialization_test.output
		*/
	DiscreteDistribution<State>& operator=(DiscreteDistribution<State>&& other) = default;

  /** \brief Generates random objects that are distributed according to the associated parameter set. The entropy is acquired by calling g.operator().
		* \tparam Generator must meet the requirements of <a href="http://en.cppreference.com/w/cpp/concept/UniformRandomBitGenerator">UniformRandomBitGenerator</a>.
		* \param g an uniform random bit generator object
		* \section Example
		* \snippet random/test/DiscreteDistribution/DiscreteDistribution_test.cpp Example
		* \section Output
		* \include random/test/DiscreteDistribution/DiscreteDistribution_test.output
		*/
	template<typename Generator>
	result_type operator()(Generator& g) const
	{
		std::discrete_distribution <int> d(m_param.weights().cbegin(), m_param.weights().cend());
		return m_param.support()[d(g)];
	}

	/** \brief Generates random numbers that are distributed according to params
	  * The entropy is acquired by calling g.operator().
		* \tparam Generator must meet the requirements of <a href="http://en.cppreference.com/w/cpp/concept/UniformRandomBitGenerator">UniformRandomBitGenerator</a>.
		* \param g an uniform random bit generator object
		* \param params	distribution parameter set to use instead of the associated one
		* \section Example
		* \snippet random/test/DiscreteDistribution/DiscreteDistribution_test.cpp Example
		* \section Output
		* \include random/test/DiscreteDistribution/DiscreteDistribution_test.output
		*/
	template<typename Generator>
	result_type operator()(Generator& g, const param_type & params) const
	{
		std::discrete_distribution <int> d(params.weights().cbegin(), params.weights().cend());
		return params.support()[d(g)];
	}

};

} // namespace random
} // namespace quetzal

#endif
