// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __OCCUPANCY_SPECTRUM_DISTRIBUTION_H_INCLUDED__
#define __OCCUPANCY_SPECTRUM_DISTRIBUTION_H_INCLUDED__

#include "Generator.h"

#include <random> // discrete_distribution
#include <utility> // std::forward, std::move
#include <vector>
#include <cmath> // std::pow
#include <assert.h>
#include <iostream> // << operator implementation

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/random.hpp> // boost::multiprecision::pow

namespace quetzal{
namespace coalescence{
namespace occupancy_spectrum {

  using boost::multiprecision::cpp_int;
  using boost::multiprecision::cpp_dec_float_50;

  struct return_always_true {
      bool operator()(double /*param*/) const
      {
        return true;
      }
  };

  struct identity {
      using spectrum_type = Generator::occupancy_spectrum_type;
      static spectrum_type handle(spectrum_type&& M_j)
      {
          return std::move(M_j);
      }
  };

  struct truncate_tail {
    using spectrum_type = Generator::occupancy_spectrum_type;
    static spectrum_type handle(spectrum_type&& M_j)
    {
      auto first = --(M_j.end());
      while (first != M_j.begin() && *first == 0)
      {
        --first;
      }
      M_j.erase(++first, M_j.end());
      return std::move(M_j);
    }
  };

  /*!
   * \ingroup coal_spectrum
   * \brief Generates random occupancy spectrum that are distributed according
   *        to the associated probability function given by von Mises (1939).
   *        The entropy is acquired by calling g.operator().
   * \remark The probability of an occupancy spectrum is described in the book
   *         "Urn models and their application: an approach
   *         to modern discrete probability theory" (by John and Kotz, 1977, p.115).
   * \tparam UnaryPredicate Function object class acting as a predicate, taking
   *         the probability of the spectrum as argument and returning a boolean.
   *         The signature should be `bool UnaryPredicate::operator()(double)`
   *         The number of possible configurations grows very fast with n and m,
   *         so memorizing all of them quickly becomes intractable.
   *         As most of the configurations have extremely low output probability,
   *         you can choose to filter the spectrum according to their sampling
   *         probability, keeping only those which probability is superior to a
   *         given threshold (or other criterion). If the predicate is evaluated
   *         to true, the spectrum and its probability will be saved in the
   *         distribution. If the predicate is evaluated to false, the spectrum
   *         and its probability will be discarded. By default, ReturnAlwaysTrue
   *         policy is used, so all spectra are kept.
   * \tparam SpectrumHandler Function object class modifying the spectrum to optimize speed and
   *         memory use. The signature should be
   *         `OccupancySpectrum SpectrumHandler::operator()(OccupancySpectrum &&)`.
   *         By default the Identity policy wil not modify the spectra.
   *         The RetrieveLastEmptyUrns will retrieve all the last urns that are
   *         empty, making later iteration over a spectrum faster.
   * \tparam Int A big integer class for probability computation
   * \tparam FLoat A big float class for probability computation
   * \section Example
   * \snippet coalescence/occupancy_spectrum/test/OccupancySpectrumDistribution/OccupancySpectrumDistribution_test.cpp Example
   * \section Output
   * \include coalescence/occupancy_spectrum/test/OccupancySpectrumDistribution/OccupancySpectrumDistribution_test.output
   */
  template
  <
  class UnaryPredicate = return_always_true,
  class SpectrumHandler = identity,
  class Int = cpp_int,
  class Float = cpp_dec_float_50
  >
  class OccupancySpectrumDistribution {

    using SelfType = OccupancySpectrumDistribution<UnaryPredicate, SpectrumHandler, Int, Float>;
    using spectrum_type = typename Generator::occupancy_spectrum_type;
    using support_type = std::vector<spectrum_type>;
    using probabilities_type = std::vector<double>;

    unsigned int m_k;
    unsigned int m_N;
    UnaryPredicate m_pred;
    support_type m_support;
    probabilities_type m_probas;
    mutable std::discrete_distribution<size_t> m_dist;

  public:

    /**
  	  * \brief Default constructor
  	  * \remark Position and Tree must be default constructible
      * \section Example
  		* \snippet coalescence/occupancy_spectrum/test/OccupancySpectrumDistribution/initialization_test.cpp Example
  		* \section Output
      * \include coalescence/occupancy_spectrum/test/OccupancySpectrumDistribution/initialization_test.output
  		*/
    OccupancySpectrumDistribution() = default;

    /**
  	  * \brief Construct the occupancy spectrum distribution resulting from throwing n balls into m urns
  	  * \remark Object can be huge if large k
      * \param n number of balls
      * \param m number of urns
      * \param pred UnaryPredicate
      * \section Example
  		* \snippet coalescence/occupancy_spectrum/test/OccupancySpectrumDistribution/initialization_test.cpp Example
  		* \section Output
      * \include coalescence/occupancy_spectrum/test/OccupancySpectrumDistribution/initialization_test.output
  		*/
    OccupancySpectrumDistribution(unsigned int n, unsigned int m, UnaryPredicate pred = UnaryPredicate()) :
    m_k(n),
    m_N(m),
    m_pred(pred)
    {
      auto callback = make_callback(m_k, m_N);
      Generator::generate(m_k, m_N, callback);
      m_dist = std::discrete_distribution<size_t>( m_probas.cbegin(), m_probas.cend() );
      assert(m_support.size() == m_probas.size());
    }

    /**
  	  * \brief Copy constructor
  	  * \remark Position and Tree must be default constructible
      * \section Example
  		* \snippet coalescence/occupancy_spectrum/test/OccupancySpectrumDistribution/initialization_test.cpp Example
  		* \section Output
      * \include coalescence/occupancy_spectrum/test/OccupancySpectrumDistribution/initialization_test.output
  		*/
    OccupancySpectrumDistribution(const SelfType&) = delete;

    /**
  	  * \brief Move constructor
  	  * \remark Position and Tree must be default constructible
      * \section Example
  		* \snippet coalescence/occupancy_spectrum/test/OccupancySpectrumDistribution/initialization_test.cpp Example
  		* \section Output
      * \include coalescence/occupancy_spectrum/test/OccupancySpectrumDistribution/initialization_test.output
  		*/
    OccupancySpectrumDistribution(SelfType&&) = default;

    /**
  	  * \brief Move assignment operator
  		* \section Example
  		* \snippet coalescence/occupancy_spectrum/test/OccupancySpectrumDistribution/initialization_test.cpp Example
  		* \section Output
      * \include coalescence/occupancy_spectrum/test/OccupancySpectrumDistribution/initialization_test.output
  		*/
    SelfType & operator= (SelfType&& ) = default;

    /**
  	  * \brief Deleted copy assignment operator
      * \remark Deleted for avoiding copying huge amount of data.
  		* \section Example
  		* \snippet coalescence/occupancy_spectrum/test/OccupancySpectrumDistribution/initialization_test.cpp Example
  		* \section Output
      * \include coalescence/occupancy_spectrum/test/OccupancySpectrumDistribution/initialization_test.output
  		*/
    SelfType & operator= (const SelfType& ) = delete;

  /**
    * \brief Generates random occupancy spectrum that are distributed according
    *        to the associated probability function given by von Mises (1939).
    *        The entropy is acquired by calling g.operator().
    * \section Example
    * \snippet coalescence/occupancy_spectrum/test/OccupancySpectrumDistribution/OccupancySpectrumDistribution_test.cpp Example
    * \section Output
    * \include coalescence/occupancy_spectrum/test/OccupancySpectrumDistribution/OccupancySpectrumDistribution_test.output
    */
    template<typename Generator>
    const spectrum_type & operator()(Generator& g) const {
      return m_support[m_dist(g)];
    }

    /**
      * \brief Gives the number of balls that are thrown in the random experience
      * \section Example
      * \snippet coalescence/occupancy_spectrum/test/OccupancySpectrumDistribution/OccupancySpectrumDistribution_test.cpp Example
      * \section Output
      * \include coalescence/occupancy_spectrum/test/OccupancySpectrumDistribution/OccupancySpectrumDistribution_test.output
      */
    unsigned int n() const {return m_k; }

    /**
      * \brief Gives the number of urns used in the random experience
      * \section Example
      * \snippet coalescence/occupancy_spectrum/test/OccupancySpectrumDistribution/OccupancySpectrumDistribution_test.cpp Example
      * \section Output
      * \include coalescence/occupancy_spectrum/test/OccupancySpectrumDistribution/OccupancySpectrumDistribution_test.output
      */
    unsigned int m() const {return m_N; }

    /**
      * \brief Retrieve occupancy spectra constituting the support of the distribution
      * \section Example
      * \snippet coalescence/occupancy_spectrum/test/OccupancySpectrumDistribution/OccupancySpectrumDistribution_test.cpp Example
      * \section Output
      * \include coalescence/occupancy_spectrum/test/OccupancySpectrumDistribution/OccupancySpectrumDistribution_test.output
      */
    const support_type& support() const {return m_support; }

    /**
      * \brief Retrieve the weights associated to the random experience. They do not necessarily sum to 1.
      * \section Example
      * \snippet coalescence/occupancy_spectrum/test/OccupancySpectrumDistribution/OccupancySpectrumDistribution_test.cpp Example
      * \section Output
      * \include coalescence/occupancy_spectrum/test/OccupancySpectrumDistribution/OccupancySpectrumDistribution_test.output
      */
    const probabilities_type & weights() const {return m_probas; }

    friend std::ostream& operator<<(std::ostream& os, SelfType const& dist){
        for(unsigned int i = 0; i < dist.support().size(); ++ i)
        {
          //os << "P( \t" << dist.support()[i] << "\t) = " << dist.weights()[i] << "\n";
          auto v = dist.support()[i];
          os << "P( \t";
          if ( !v.empty() ) {
            os << '[';
            std::copy (v.begin(), v.end(), std::ostream_iterator<unsigned int>(os, ", "));
            os << "\b\b]";
          }
           os << "\t) = " << dist.weights()[i] << "\n";
        }
        return os;
    }

  private:

    auto make_callback(unsigned int k, unsigned int N){
      return [this, k, N](spectrum_type&& M_j){
        auto p = compute_probability(k, N, M_j);
        if(this->m_pred(p)){
          this->m_support.push_back(SpectrumHandler::handle(std::move(M_j)));
          this->m_probas.push_back(p);
        }
      };
    }

    static Int Factorial(int num) {
      Int fact = 1;
      for(int i=num; i>1; --i){
        fact *= i;
      }
      return fact;
    }


    static double compute_probability(unsigned int n, unsigned int m, spectrum_type const& m_js) {
      Float numerator(Factorial(m)*Factorial(n));
      Float denominator(1.0);
      int j = 0;
      for(auto m_j : m_js){
        denominator *= boost::multiprecision::pow(Float(Factorial(j)),Float(m_j)) * Float(Factorial(m_j));
        ++j;
      }
      denominator *= Float(std::pow(m,n));
      Float result = numerator/denominator;
      return static_cast<double>(result) ;
    }

  };

} // namespace occupancy_spectrum
} // namespace coalescence
} // namespace quetzal

#endif
