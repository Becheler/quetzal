// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __HISTORY_STRATEGIES_H_INCLUDED__
#define __HISTORY_STRATEGIES_H_INCLUDED__

#include "PointWithId.h"
#include "dispersal.h"
#include "../utils.h"
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/symmetric.hpp>

#include <vector>

namespace quetzal {

namespace demography {

/*!
 * Various implementations of dispersal process implementing an interface
 * compatible with the History demographic extension algorithms requirements.
 */
namespace strategy {

  /*!
   * @brief Class for demographic simulation of small populations.
   *
   * @ingroup demography
   *
   * \details Simulate the demographic history by sampling the location of each
   *          individuals in a proability distribution.
   */
  class individual_based {

  private:

    /*!
     * Implementation for demographic expansion algorithm
     * @tparam Space type of geographic coordinates
     * @tparam Matrix the type of matrix used for storing migration rates
     */
    template<typename Space, typename Matrix>
    class Implementation {

      using coord_type = Space;
      // distance matrix type, can be sparse or symmetric
      using matrix_type = Matrix;
      // for easier access to matrix row/columns ID
      using point_ID_type = PointWithId<coord_type>;
      // owe the matrix, heavy object
      matrix_type _matrix;
      // don't owe the vector
      std::vector<point_ID_type> const& _points;
      // don't owe the space
      std::vector<coord_type> const& _coords;

      // specific to individual-based:
      // store discrete distributions centered on departure
      using distribution_type = std::discrete_distribution<size_t>;
      mutable std::vector<distribution_type> _distributions;

      /*!
       * make a weight matrix
       * @tparam F functor with signature matrix_type::value_type (coord_type const& x, coord_type const&y)
       * @param  coords vector of coordinates to compute distance between
       * @param  w      functor F, usually a transform of geographic distance between two coordinates
       * @return        a weight matrix
       */
      template<typename F>
      matrix_type make_weight_matrix(std::vector<coord_type> const& coords, F w){
        matrix_type A (coords.size(), coords.size());
        for (unsigned i = 0; i < A.size1 (); ++ i)
        {
          auto x = coords.at(i);
          for (unsigned j = 0; j < A.size2(); ++ j)
          {
            auto y = coords.at(j);
            A (i, j) = w(x, y);
          }
        }
        return A; // will be used in discrete_distribution: no need to divide_terms_by_row_sum
      }

      /*!
       * Builds the discrete distributions for sampling coords IDs
       * @return a vector of discrete distributions
       */
      std::vector<distribution_type> make_distributions() const
      {
        std::vector<distribution_type> dists;
        dists.reserve(_matrix.size1());
        for (unsigned i = 0; i < _matrix.size1 (); ++ i)
        {
          std::vector<double> weights;
          weights.reserve(_matrix.size2());
          for (unsigned j = 0; j < _matrix.size2(); ++ j)
          {
            weights.push_back(_matrix(i, j));
          }
          dists.emplace_back(weights.cbegin(), weights.cend());
        }
        return dists;
      }

    public:

      // Forbid costly copy
      Implementation(const Implementation&) = delete;

      template<typename F>
      Implementation(std::vector<point_ID_type> const& points, std::vector<coord_type> const& coords, F f) :
      _matrix( make_weight_matrix(coords, f) ),
      _points(points),
      _coords(coords),
      _distributions(make_distributions())
      {}

      // interface with indidividual-based demographic history expand method
      template<typename Generator>
      coord_type sample_arrival(Generator& gen, coord_type const& x) const
      {
        return _coords[_distributions[getIndexOfPointInVector(x, _coords)].operator()(gen)];
      }

    }; // inner class Implementation

    // Cheap copy interfacing the implementation with the demographic simulation
    template<typename Space, typename Matrix>
    class Interface {

      using impl_type = Implementation<Space, Matrix>;
      using coord_type = Space;
      using point_ID_type = PointWithId<coord_type>;
      //copying the interface is kind ok
      std::shared_ptr<impl_type> m_pimpl;

    public:

      // Constructor invoked by helper functions
      template<typename F>
      Interface(std::vector<point_ID_type> const& points, std::vector<coord_type> const& coords, F f):
      m_pimpl(std::make_shared<impl_type>(points, coords, f) ) {}

      // migration rate interface in the demographic algorithm
      template<typename Generator>
      coord_type operator()( Generator& gen, coord_type const& x){
        return m_pimpl->sample_arrival(gen, x);
      }
    }; // inner class Interface

  public:

    //! \typedef type used to represent effective population size and flows
    using value_type = unsigned int;

    /*!
     * Contruct a dispersal kernel compatible with the mass-based strategy
     *
     * @param  coords a vector of geographic points
     * @param  f      a functor giving the migration probability of an individual between two points
     *                The signature should be equivalent to double f(T const& x, T const& Y)
     * @return        A dispersal kernel
     */
    template<typename T, typename F>
    static auto make_distance_based_dispersal(std::vector<T> const& coords, F f)
    {
      using coord_type = T;
      std::vector<PointWithId<coord_type>> points;
      points.reserve(coords.size());
      const auto& ref = coords;
      std::transform(coords.begin(), coords.end(), std::back_inserter(points),
                     [ref](coord_type const& x){ return PointWithId<coord_type>(ref, x); });
      using matrix_type = boost::numeric::ublas::symmetric_matrix<double>;
      return Interface<coord_type, matrix_type>(points, coords, f);
    }

  };




  /*!
  * @brief Class for demographic simulation of large populations.
  *
  * @ingroup demography
  *
  * \details Simulate the demographic history by considering populations of
  *          individuals as divisible masses leading to faster simulations.
  */
  class mass_based {

    /*!
     * Implementation for demographic expansion algorithm
     * @tparam Space type of geographic coordinates
     * @tparam Matrix the type of matrix used for storing migration rates
     */
    template<typename Space, typename Matrix>
    class Implementation {

      using coord_type = Space;
      // distance matrix type, can be sparse or symmetric
      using matrix_type = Matrix;
      // for easier access to matrix row/columns ID
      using point_ID_type = PointWithId<coord_type>;
      // owe the matrix, heavy object
      matrix_type _matrix;
      // don't owe the vector
      std::vector<point_ID_type> const& _points;
      // don't owe the space
      std::vector<coord_type> const& _coords;
      // oops what was that again ?
      mutable std::map<coord_type, std::vector<coord_type>> m_arrival_space_cash;

      /*!
       * make a weight matrix
       * @tparam F functor with signature matrix_type::value_type (coord_type const& x, coord_type const&y)
       * @param  coords vector of coordinates to compute distance between
       * @param  w      functor F, usually a transform of geographic distance between two coordinates
       * @return        a weight matrix
       */
      template<typename F>
      matrix_type make_weight_matrix(std::vector<coord_type> const& coords, F w){
        matrix_type A (coords.size(), coords.size());
        for (unsigned i = 0; i < A.size1 (); ++ i)
        {
          for (unsigned j = 0; j < A.size2(); ++ j)
          {
            auto x = coords.at(i);
            auto y = coords.at(j);
            A (i, j) = w(x, y);
          }
        }
        return quetzal::utils::divide_terms_by_row_sum(A);
      }

    public:

      // Forbid costly copy
      Implementation(const Implementation&) = delete;

      template<typename F>
      Implementation(std::vector<point_ID_type> const& points, std::vector<coord_type> const& coords, F f) :
      _matrix( make_matrix(coords, f) ),
      _points(points),
      _coords(coords)
      {}

      /*!
       * interface with mass-based demographic history expand method that iterates
       * on the locations that are reachable from x.
       * @param  x departure coordinate
       * @return   a std::vector of reachable coordinates from x.
       */
      auto arrival_space(coord_type const& x) const {
        auto it = m_arrival_space_cash.find(x);
        if(it != m_arrival_space_cash.end())
        {
          return it->second;
        }else{
          auto p = m_arrival_space_cash.emplace(x, retrieve_non_zero_arrival_space(x));
          return p.first->second;
        }
      }

      auto retrieve_non_zero_arrival_space(coord_type const& x) const {
        using it1_t = typename matrix_type::const_iterator1;
        using it2_t = typename matrix_type::const_iterator2;
        auto i = getIndexOfPointInVector(x, _coords);
        it1_t it1 = _matrix.begin1();
        std::advance(it1, i);
        std::vector<coord_type> v;
        v.reserve(_matrix.size2()); // worst case
        for (it2_t it2 = it1.begin(); it2 != it1.end(); it2++) {
            if(*it2 > 0.0) v.push_back(_coords.at(it2.index2()));
        }
        return v;
      }

      // interface with mass-based demographic history expand method
      double migration_rate(coord_type const& x, coord_type const& y){
        return _matrix(getIndexOfPointInVector(x, _coords), getIndexOfPointInVector(y, _coords) );
      }

    }; // inner class Implementation

    // Cheap copy interfacing the implementation with the demographic simulation
    template<typename Space, typename Matrix>
    class Interface {

      using impl_type = Implementation<Space, Matrix>;
      using coord_type = Space;
      using point_ID_type = PointWithId<coord_type>;
      //copying the interface is kind ok
      std::shared_ptr<impl_type> m_pimpl;

    public:

      // Constructor invoked by helper functions
      template<typename F>
      Interface(std::vector<point_ID_type> const& points, std::vector<coord_type> const& coords, F f):
      m_pimpl(std::make_shared<impl_type>(points, coords, f) ) {}

      // migration rate interface in the demographic algorithm
      double operator()(coord_type const& x, coord_type const& y){
        return m_pimpl->migration_rate(x,y);
      }

      // part of the interface in the demographic algorithm
      auto arrival_space(coord_type const& x) const {
        return m_pimpl->arrival_space(x);
      }
    }; // inner class Interface

  public:

    //! \typedef type used to represent effective population size and flows
    using value_type = unsigned int;

    /*!
     * Contruct a dispersal kernel compatible with the mass-based strategy
     *
     * @param  coords a vector of geographic points
     * @param  f      a functor giving the migration coefficient between two points
     *                The signature should be equivalent to double f(T const& x, T const& Y)
     * @return        A dispersal kernel
     */
    template<typename T, typename F>
    static auto make_distance_based_dispersal(std::vector<T> const& coords, F f)
    {
      using coord_type = T;
      std::vector<PointWithId<coord_type>> points;
      points.reserve(coords.size());
      const auto& ref = coords;
      std::transform(coords.begin(), coords.end(), std::back_inserter(points),
                     [ref](coord_type const& x){ return PointWithId<coord_type>(ref, x); });
      using matrix_type = boost::numeric::ublas::symmetric_matrix<double>;
      return Interface<coord_type, matrix_type>(points, coords, f);
    }

    /*!
     * Contruct a dispersal kernel compatible with the mass-based strategy
     *
     * @details This implementation should be chosen when the migration coefficients
     *          are expect to be null over the major part of the landscape.
     *
     * @param  coords a container of geopgrahic points
     * @param  f      a functor giving the migration coefficient between two points
     *                The signature should be equivalent to double f(T const& x, T const& Y)
     * @return        A dispersal kernel
     */
    template<typename T, typename F>
    static auto make_sparse_distance_based_dispersal(std::vector<T> const& coords, F f){
      using coord_type = T;
      std::vector<PointWithId<coord_type>> points;
      points.reserve(coords.size());
      const auto& ref = coords;
      std::transform(coords.begin(), coords.end(), std::back_inserter(points),
                     [ref](coord_type const& x){ return PointWithId<coord_type>(ref, x); });
      using matrix_type = boost::numeric::ublas::compressed_matrix<double>;
      return Interface<coord_type, matrix_type>(points, coords, f);
    }


    template<typename Space, typename F1, typename F2>
    class neighboring_migration{

      using dico_type = std::unordered_map<Space, std::vector<Space>>;
      std::shared_ptr<dico_type> m_neighborhood;
      double m_emigrant_rate;
      F1 m_friction;
      F2 m_get_neighbors;

    public:

      using coord_type = Space;

      // Constructor
      neighboring_migration(std::vector<coord_type> const& coords, double emigrant_rate, F1 friction, F2 get_neighbors):
      m_neighborhood(std::make_shared<dico_type>(build_neighborhood(coords, get_neighbors))),
      m_emigrant_rate(emigrant_rate),
      m_friction(friction),
      m_get_neighbors(get_neighbors)
      {}

      // arrival space contains focal deme
      std::vector<coord_type> arrival_space(coord_type const& x) const
      {
        auto v = m_neighborhood->at(x);
        v.push_back(x);
        return v;
      }

      auto operator()(coord_type const& from, coord_type const& to){
       if(from == to){
         return 1.0 - m_emigrant_rate;
       }else{ // we have "ensurance" that arrival_space(x) is used, so here only neighbours.
         return m_emigrant_rate * friction_weights(from, to);
       }
     }

    private:

      // returns weight associated to a migration from x to y
      // need a m_friction member
      // perhaps memoize the sum related to x,t
      // problem for t argument
      double friction_weights(coord_type const& x, coord_type const& y) const {
        const auto & neighbors = m_neighborhood->at(x);
        std::vector<double> v(neighbors.size(), 0.0);
        std::transform(neighbors.begin(), neighbors.end(), v.begin(), [this](coord_type const& z) { return 1.0/(this->m_friction(z));} );
        double sum = std::accumulate(v.begin(), v.end(), 0.0);
        return 1.0/(m_friction(y)*sum);
      }

      auto build_neighborhood(std::vector<coord_type> const& coords, F2 get_neighbors){
        std::unordered_map<coord_type, std::vector<coord_type>> dico;
        for(auto const& x : coords){
          dico[x] = get_neighbors(x);
        }
        return dico;
      }

    }; // neighboring_migration

    template<typename T, typename U, typename V>
    static auto make_neighboring_migration(std::vector<T> const& coords, double emigrant_rate, U friction, V get_neighbors)
    {
      return neighboring_migration<T, U, V>(coords, emigrant_rate, friction, get_neighbors);
    }

  }; // mass_based

} // namespace strategy

} //namespace demography

} // namespace quetzal

#endif
