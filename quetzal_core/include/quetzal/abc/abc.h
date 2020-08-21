// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __ABC_H_INCLUDED__
#define __ABC_H_INCLUDED__

#include "ReferenceTable.h"

#include <tuple>
#include <vector>
#include <type_traits> // result_of
#include <stdexcept> // std::domain_error

namespace quetzal {
namespace abc {

	template<typename T>
  struct identity {
      T operator()(T&& a) const { return std::move(a); }
			const T& operator()(T const& a) const { return a; }
  };


	/*!
	 * \brief ABC object for Approximate Bayesian Computations.
	 *
	 * Associates a generative (simulation) model with a possibly multidimensional
	 * prior distribution.
	 *
	 * Simulation models are abstracted to the concept of \ref abc "GenerativeModel".
	 * \tparam ModelType a type that meets the requirements of \ref abc "GenerativeModel".
	 * \tparam PriorType a type that meets the requirements of \ref abc "PriorDistribution"
	 * \remark ABC objects can be used with the helper function `make_ABC`
	 *
	 * \ingroup abc
	 * \section Example
	 * \snippet abc/test/test.cpp Example
	 * \section Output
	 * \include abc/test/test.output
	 */
	template<class ModelType, class PriorType>
	class ABC{

	public:

		//! \typedef model type used for simulating data
		using model_type = ModelType;

		//! \typedef model parameter type
		using param_type = typename model_type::param_type;

		//! \typedef simulated data type
		using simulated_data_type = typename model_type::result_type;

		//! \typedef prior distribution type for sampling model parameters
		using prior_type = PriorType;

	private:

		mutable prior_type priors;
		mutable model_type generative_model;

		/*!
		 * \brief A sample in the prior predictive distribution
		 *
		 * In some ABC procedures, a parameter \f$\theta\f$ is generated from the prior \f$\pi(\theta)\f$,
		 * a data \f$z\f$ is simulated from the likelyhood and a dimension reduction function \f$\eta\f$
		 * is computed on \f$z\f$. Each couple \f$\{\theta, z\}\f$ is represented in the compute_distance_to
		 * by the PriorPredictiveDistributionSample. The reference table contains all the sampled couples.
		 *
		 */
		template<class ParamType, class DataType>
		class PriorPredictiveDistributionSample{

		public:

			//! \typedef parameter type
			using param_type = ParamType;

			//! \typedef simulated data_type
			using data_type = DataType;

		private:

			param_type m_param;
			data_type m_data;

		public:
			PriorPredictiveDistributionSample(param_type const& p, data_type const& d) :
			m_param(p),
			m_data(d)
			{}

			PriorPredictiveDistributionSample(param_type && p, data_type&& d) noexcept :
			m_param(std::move(p)),
			m_data(std::move(d))
			{}

			PriorPredictiveDistributionSample(param_type const& p, data_type&& d) noexcept :
			m_param(p),
			m_data(std::move(d))
			{}

		//! Get the parameter used for generating the associated data
		const param_type & param() const {return m_param; }

		//! Get the data generated
		const data_type & data() const {return m_data; }

		}; // end class PriorPredictiveDistributionSample


	public:

	 /**
		 * \brief Constructor
		 * \param m the model to be used
		 * \param p the prior distribution
		 * \section Example
		 * \snippet abc/test/test.cpp Example
	 	 * \section Output
	 	 * \include abc/test/test.output
		 */
		ABC(model_type const& m, prior_type const& p)  :
		priors(p),
		generative_model(m)
		{}

		/**
			* \brief Rubin (1984) likelyhood-free rejection sampler
			*
			* A parameter \f$\theta\f$ is generated from the prior \f$\pi(\theta)\f$, a data is simulated,
			* and the parameter is accepted if the simulated data is equal to the observed data \f$y\f$,
			* assuming observed data takes values in a finite or countable set \f$D\f$
			* as defined in
			* [Approximate Bayesian Computations methods, Marin et al 2011](https://link.springer.com/article/10.1007%2Fs11222-011-9288-2?LI=true),
			* Algorithm 1.
			*
			* \param N the number of parameters to accept.
			* \param y the observed data from which to draw inference.
			* \param g a random generator that must meet the requirements of [UniformRandomBitGenerator](http://en.cppreference.com/w/cpp/concept/UniformRandomBitGenerator) concept.
			*
			* \return a vector of accepted parameter values.
			*
			* \remark ObservedDataType and simulated_data_type should be [EqualityComparable](http://en.cppreference.com/w/cpp/concept/EqualityComparable)
			*
			* \section Example
			* \snippet abc/test/rubin_test.cpp Example
			* \section Output
			* \include abc/test/rubin_test.output
			*/
		template<class ObservedDataType, typename Generator>
		std::vector<param_type> rubin_rejection_sampler(unsigned int N, ObservedDataType const& y, Generator& g) const {
			std::vector<param_type> sample;
			sample.reserve(N);
			while(sample.size() != N)
			{
				auto params = param_type(priors(g));
				if( y == generative_model(g, params))
				{
					sample.push_back(params);
				}
			}
			return sample;
		}


		/**
			* \brief Pritchard & al (1999) likelyhood free rejection sampler.
			*
			* An extension of the Rubin sampler to the case of continuous sample spaces.
			*
			* A parameter \f$\theta\f$ is generated from the prior \f$\pi(\theta)\f$, a data is simulated,
			* and the parameter is accepted if the simulated data is equal to the observed data \f$y\f$,
			* assuming observed data takes values in a continuous set \f$D\f$
			* as defined in
			* [Approximate Bayesian Computations methods, Marin et al 2011](https://link.springer.com/article/10.1007%2Fs11222-011-9288-2?LI=true),
			* Algorithm 2.
			*
			* \param N the number of parameters to accept.
			* \param y the observed data from which to draw inference.
			* \param eta a function \f$\eta\f$ on \f$D\f$ defining a statistics (sufficient or not).
			* \param rho a distance \f$\rho > 0\f$ on \f$\eta(D)\f$
			* \param epsilon a tolerance level \f$\epsilon > 0\f$
			* \param g a random generator that must meet the requirements of [UniformRandomBitGenerator](http://en.cppreference.com/w/cpp/concept/UniformRandomBitGenerator) concept.
			*
			* \return a vector of accepted parameter values.
			*
			* \section Example
			* \snippet abc/test/pritchard_test.cpp Example
			* \section Output
			* \include abc/test/pritchard_test.output
			*/
		template<class ObservedDataType, class DistanceType, class StatisticsType, class T, class Generator>
		std::vector<param_type> pritchard_rejection_sampler(
			unsigned int N,
			ObservedDataType const& y,
			StatisticsType const& eta,
			DistanceType const& rho,
			T const& epsilon,
			Generator& g
		) const {
			std::vector<param_type> sample;
			sample.reserve(N);

			while(sample.size() != N)
			{
				auto params = param_type(priors(g));
				if( rho(eta(y), eta(generative_model(g, params))) <= epsilon )
				{
					sample.push_back(params);
				}
			}
			return sample;
		}


		/**
			* \brief Pritchard & al (1999) likelyhood free rejection sampler.
			*
			* An extension of the Rubin sampler to the case of continuous sample spaces.
			*
			* A parameter \f$\theta\f$ is generated from the prior \f$\pi(\theta)\f$, a data is simulated,
			* and the parameter is accepted if the simulated data is equal to the observed data \f$y\f$,
			* assuming observed data takes values in a continuous set \f$D\f$
			* as defined in
			* [Approximate Bayesian Computations methods, Marin et al 2011](https://link.springer.com/article/10.1007%2Fs11222-011-9288-2?LI=true),
			* Algorithm 2.
			*
			* \param n the number of parameters to accept.
			* \param y the observed data from which to draw inference.
			* \param rho a distance \f$\rho > 0\f$ on \f$D\f$
			* \param epsilon a tolerance level \f$\epsilon > 0\f$
			* \param g a random generator that must meet the requirements of [UniformRandomBitGenerator](http://en.cppreference.com/w/cpp/concept/UniformRandomBitGenerator) concept.
			*
			* \return a vector of accepted parameter values.
			*
			* \remark the dimension reduction function \f$\eta\f$ is here defined as the identity.
			*
			* \section Example
			* \snippet abc/test/pritchard_identity_test.cpp Example
			* \section Output
			* \include abc/test/pritchard_identity_test.output
			*/
		template<class ObservedDataType, class DistanceType, class T, class Generator>
		std::vector<param_type> pritchard_rejection_sampler(unsigned int N, ObservedDataType const& y, DistanceType const& rho,	T const& epsilon,	Generator& g) const {
			return pritchard_rejection_sampler(N, y, identity<simulated_data_type>(), rho, epsilon, g);
		}


		/**
			* \brief Generation of reference table from prior predictive distribution
			*
			*
			* A parameter \f$\theta\f$ is generated from the prior \f$\pi(\theta)\f$,
			* a data \f$z\f$ is simulated from the likelyhood and a dimension reduction function \f$\eta\f$
			* is computed on \f$z\f$. The set of simulated statistics is called the
			* reference table.
			*
			* See Algorithm 1 in [ABC Random Forests for Bayesian Parameter Inference (Marin et al 2016)](https://arxiv.org/abs/1605.05537)
			*
			* \param N the number of parameters to accept.
			* \param eta a function \f$\eta\f$ on \f$D\f$ defining a statistics (sufficient or not).
			* \param g a random generator that must meet the requirements of [UniformRandomBitGenerator](https://en.cppreference.com/w/cpp/named_req/UniformRandomBitGenerator) concept.
			*
			* \return a ReferenceTable
			*
			* \section Example
			* \snippet abc/test/ReferenceTable_test.cpp Example
			* \section Output
			* \include abc/test/ReferenceTable_test.output
			*/
		template<class StatisticsType, typename Generator>
		auto sample_prior_predictive_distribution(unsigned int N, StatisticsType const& eta, Generator& g) const {

			using sumstat_type = typename std::result_of_t<StatisticsType(simulated_data_type&&)>;

			ReferenceTable<PriorPredictiveDistributionSample, param_type, sumstat_type> table;
			unsigned int i = 0;
			while(i < N)
			{
				auto params = param_type(priors(g));
				try{
					table.emplace_back(params, eta(generative_model(g, params)));
					++i;
				}
				catch (std::domain_error& e){
					std::cerr << e.what() << std::endl;
				}
			}

			return table;
		}

		/**
			* \brief Generation of reference table from prior predictive distribution
			*
			*
			* A parameter \f$\theta\f$ is generated from the prior \f$\pi(\theta)\f$,
			* a data \f$z\f$ is simulated from the likelyhood. The set of simulated statistics is called the
			* reference table.
			*
			* See Algorithm 1 in [ABC Random Forests for Bayesian Parameter Inference (Marin et al 2016)](https://arxiv.org/abs/1605.05537)
			*
			* \param N the number of parameters to accept.
			* \param g a random generator that must meet the requirements of [UniformRandomBitGenerator](http://en.cppreference.com/w/cpp/concept/UniformRandomBitGenerator) concept.
			*
			* \return a ReferenceTable
			*
			* \remark the dimension reduction function \f$\eta\f$ is here defined as the identity.
			*
			* \section Example
			* \snippet abc/test/ReferenceTable_test.cpp Example
			* \section Output
			* \include abc/test/ReferenceTable_test.output
			*/
		template<typename Generator>
		auto sample_prior_predictive_distribution(unsigned int N, Generator& g) const {
			using simulated_data_type = typename std::result_of<ModelType(Generator&, const param_type&)>::type;
			return sample_prior_predictive_distribution(N, identity<simulated_data_type>(), g);
		}

	}; // end class ABC definition


	/**
		* \brief Build an ABC object
		*
		* \param model a model that must meet the requirements of \ref abc 'GenerativeModel'
		* \param prior a prior distribution that must meet the requirements of \ref abc 'PriorDistribution'
		*
		* \return an ABC object.
		*
		* \ingroup abc
		* \section Example
 	  * \snippet abc/test/test.cpp Example
 	  * \section Output
 	  * \include abc/test/test.output
		*/
	template<class ModelType, class PriorType>
	auto make_ABC(ModelType const& model,	PriorType const& prior)
	{
			return ABC<ModelType, PriorType>(model, prior);
	}

} // namespace abc
} // namespace quetzal

#endif
