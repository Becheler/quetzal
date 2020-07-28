// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __SIMULATORS_H_INCLUDED__
#define __SIMULATORS_H_INCLUDED__

#include "../demography/History.h"
#include "../coalescence/containers/Forest.h"
#include "../coalescence/policies/merger.h"

#include <map>

namespace quetzal {
namespace simulator {

/**
* @brief Discrete-time coalescence simulator in a discrete spatially explicit landscape.
*
* @details This simulator consists in a forward demographic simulation of population
*          growth and migration, after what a backward coalescent simulation is performed
*          that traces back the ancestry of a sample of gene copies sampled in one
*          or more populations. Most Recent Common ancestor is not always fond in the
*          time span of the spatial history, so if the chosen CoalescencePolicy implements
*          the required behavior, ancestry can be simulated in the more distant past using another model
*          like the DiscreteTimeWrightFisher model, to find the MRCA.
*          Details concerning the mergers (binary mergers, simultaneous multiple mergers)
*          can be controlled using the relevant Merger policy class when calling a
*          coalescence function of the class interface.
* @tparam Space deme identifiers (like the populations geographic coordinates)
* @tparam Time time identifier (like an integer representing the year)
* @tparam Strategy a template class argument (e.g. mass_based, individual_based)
*                  that indicates how dispersal should be defined in terms of
*                  demographic expansion algorithms. Strategy class
*                  implements the details (computing migrations probabilites as a function of the
*                  distance for example) whereas template specialization of the
*                  internal history_type class defines different migration algorithms.
* @tparam CoalescencePolicy a policy class for the demographic expansion.
*
* @ingroup simulator
*
*/
template<typename Space, typename Time, typename Strategy, typename CoalescencePolicy>
class SpatiallyExplicit :
public CoalescencePolicy
{

public:
  //! \typedef space type
  using coord_type = Space;
  //! \typedef time type
  using time_type = Time;
  //! \typedef strategy type
  using strategy_type = Strategy;
  //! \typedef value type to represent populations size
  using N_value_type = typename strategy_type::value_type;

  //! \typedef time type
  template<typename tree_type>
  using forest_type = quetzal::coalescence::Forest<coord_type, tree_type>;

private:

  using history_type = demography::History<coord_type, time_type, strategy_type>;
  history_type m_history;

public:


  /**
  * @brief Constructor
  *
  * @param x_0 Initialization coordinate.
  * @param t_0 Initialization time.
  * @param N_0 Population size at intialization
  */
  SpatiallyExplicit(coord_type x_0, time_type t_0, N_value_type N_0) : m_history(x_0, t_0, N_0){}

  /**
  * @brief Read-only access to the population size history.
  *
  * @return a functor with signature 'N_value_type fun(coord_type const& x,
  * time_type const& t)' giving the population size in deme \f$x\f$ at time \f$t\f$.
  *
  */
  auto pop_size_history() const noexcept
  {
    return std::cref(m_history.pop_sizes());
  }

  /**
  * @brief Simulate the forward-in-time demographic expansion.
  *
  */
  template<typename Generator, typename Growth, typename Dispersal>
  void expand_demography(time_type t_sampling, Growth growth, Dispersal kernel, Generator& gen )
  {
    time_type nb_generations = t_sampling - m_history.first_time() ;
    m_history.expand(nb_generations, growth, kernel, gen);
  }

  template<
  typename Merger,// = quetzal::coalescence::SimultaneousMultipleMerger<quetzal::coalescence::occupancy_spectrum::on_the_fly>,
  typename Generator>
  auto make_forest_and_coalesce_along_spatial_history(std::map<coord_type, unsigned int> sample, Generator &gen)
  {
    test_sample_consistency(sample);
    auto forest = this->make_forest(sample);
    return coalesce_along_spatial_history<Merger>(forest, this->branch(), gen, this->init() );
  }

  template<
  typename Merger,// = quetzal::coalescence::SimultaneousMultipleMerger<quetzal::coalescence::occupancy_spectrum::on_the_fly>,
  typename Generator>
  auto make_forest_using_sampling_time_and_coalesce_along_spatial_history(std::map<coord_type, unsigned int> sample, time_type const& sampling_time, Generator & gen)
  {
    test_sample_consistency(sample);
    auto forest = this->make_forest(sample, sampling_time);
    return coalesce_along_spatial_history<Merger>(forest, this->branch(), gen, this->init() );
  }

  /**
  * @brief Coalesce a forest of nodes conditionally to the simulated demography.
  *
  */
  template<
  typename Merger, // = quetzal::coalescence::SimultaneousMultipleMerger<quetzal::coalescence::occupancy_spectrum::on_the_fly>,
  typename Generator,
  typename F,
  typename Tree,
  typename U
  >
  auto coalesce_along_spatial_history(forest_type<Tree> forest, F binary_op, Generator& gen, U make_tree) const
  {

    this->check_consistency(m_history, forest);

    auto t = m_history.last_time();

    while( (forest.nb_trees() > 1) && (t > m_history.first_time()) )
    {
      may_coalesce_colocated<Merger>(forest, t, gen, binary_op, make_tree);
      migrate_backward(forest, t, gen);
      --t;
    }
    may_coalesce_colocated<Merger>(forest, t, gen, binary_op, make_tree);
    return forest;
  }

  /**
  * @brief Create a forest from a sample and coalesce it conditionally to the simulated demography.
  *
  * @tparam Merger a coalescence merger to use
  * @tparam Generator a random number Generator
  *
  * @param sample the number of gene copies at each location
  * @param sampling_time the time of sampling_time to build the make_forest coalescence policy function
  * @gen a random number generator
  *
  */
  template
  <
    typename Merger=quetzal::coalescence::SimultaneousMultipleMerger<quetzal::coalescence::occupancy_spectrum::on_the_fly>,
    typename Generator
  >
  auto coalesce_to_mrca(std::map<coord_type, unsigned int> sample, time_type const& sampling_time, Generator & gen)
  {
    test_sample_consistency(sample);
    // sampling_time used to initialize the forest
    auto forest = this->make_forest(sample, sampling_time);
    auto new_forest = coalesce_along_spatial_history<Merger>(forest, this->branch(), gen, this->init() );
    auto tree = this->find_mrca(new_forest, m_history.first_time(), gen);
    return this->treat(tree);
  }

  /**
  * @brief Create a forest from a sample and coalesce it conditionally to the simulated demography.
  *
  * @tparam Merger a coalescence merger to use
  * @tparam Generator a random number Generator
  *
  * @param sample the number of gene copies at each location
  * @gen a random number generator
  *
  */
  template
  <
    typename Merger=quetzal::coalescence::SimultaneousMultipleMerger<quetzal::coalescence::occupancy_spectrum::on_the_fly>,
    typename Generator
  >
  auto coalesce_to_mrca(std::map<coord_type, unsigned int> sample, Generator & gen)
  {
    test_sample_consistency(sample);
    // no need of sampling time to initalize the forest
    auto forest = this->make_forest(sample);
    auto new_forest = coalesce_along_spatial_history<Merger>(forest, this->branch(), gen, this->init() );
    auto tree = this->find_mrca(new_forest, m_history.first_time(), gen);
    return this->treat(tree);
  }

  /**
  * @brief Create a forest from a sample giving name to tips, then coalesce them conditionally to the simulated demography.
  *
  * @tparam Generator a random number Generator
  * @tparam F a unary operation with signature equivalent to 'std::string fun(coord_type const& x,
  * time_type const& t)'
  *
  * @param sample the number of gene copies at each location
  * @param sampling_time the time of sampling_time
  * @param leaf_name a functor assigning a name to a tip as a function of its location and of sampling time.
  * @gen a random number generator
  *
  */
  template
  <
    typename Merger=quetzal::coalescence::SimultaneousMultipleMerger<quetzal::coalescence::occupancy_spectrum::on_the_fly>,
    typename Generator,
    typename F
  >
  auto coalesce_to_mrca(std::map<coord_type, unsigned int> sample, time_type const& sampling_time, F leaf_name, Generator & gen)
  {
    test_sample_consistency(sample);
    auto forest = this->make_forest(sample, sampling_time, leaf_name);
    auto new_forest = coalesce_along_spatial_history<Merger>(forest, this->branch(), gen, this->init() );
    auto tree = this->find_mrca(new_forest, m_history.first_time(), gen);
    return this->treat(tree);
  }

  /**
  * @brief Create a forest from a sample then coalesce them conditionally to the simulated demography.
  *
  * @tparam T type of an individual
  * @tparam F1 a unary operation with signature equivalent to 'coord_type fun(const T& i)'
  * @tparam F2 a unary operation with signature equivalent to 'std::string fun(const T& i)'
  * @tparam Generator a random number Generator
  *
  * @param sample a collection of individuals (gene copies)
  * @param sampling_time the time of sampling_time
  * @param get_location a unary operation taking a sampled individual of type T as an argument and returning its geographic location
  * @param get_name a functor taking a sampled individual of type T as an argument and returning an identifier
  * @gen a random number generator
  *
  */
  template
  <
  typename Merger=quetzal::coalescence::SimultaneousMultipleMerger<quetzal::coalescence::occupancy_spectrum::on_the_fly>,
  typename T,
  typename F1,
  typename F2,
  typename Generator
  >
  auto coalesce_to_mrca(std::vector<T> sample, time_type const& sampling_time, F1 get_location, F2 get_name, Generator & gen)
  {
    auto forest = this->make_forest(sample, sampling_time, get_location, get_name);
    auto new_forest = coalesce_along_spatial_history<Merger>(forest, this->branch(), gen, this->init() );
    auto tree = this->find_mrca(new_forest, m_history.first_time(), gen);
    return this->treat(tree);
  }

private:

  template<typename T, typename U>
  void check_consistency(T const& history, U const& forest) const
  {
    auto t = history.last_time();
    for(auto const& it : forest.positions())
    {
      if( !history.pop_sizes().is_defined(it, t) || history.pop_sizes()(it, t) < forest.nb_trees(it))
      {
        throw std::domain_error("Simulated population size inferior to sampling size");
      }
    }
  }

  void test_sample_consistency(std::map<coord_type, unsigned int> const& sample) const{
    if(sample.size() == 0){
      throw std::logic_error("Sample to coalesce is empty");
    }else if(sample.size() == 1){
      if(sample.begin()->second < 2){
        throw std::logic_error("Sample to coalesce only contains one gene copy");
      }
    }
  }

  template<typename Generator, typename Forest>
  void migrate_backward(Forest & forest, time_type t, Generator& gen) const noexcept
  {
    Forest new_forest;
    for(auto const it : forest)
    {
      coord_type x = it.first;
      new_forest.insert(m_history.backward_kernel(x, t, gen), it.second);
    }
    assert(forest.nb_trees() == new_forest.nb_trees());
    forest = new_forest;
  }


  template<typename Merger, typename Generator, typename Tree, typename F, typename U>
  void may_coalesce_colocated(forest_type<Tree>& forest, time_type const& t, Generator& gen, F binop, U make_tree) const noexcept
  {
    auto N = pop_size_history();

    for(auto const & x : forest.positions())
    {
      auto range = forest.trees_at_same_position(x);
      std::vector<Tree> v;

      for(auto it = range.first; it != range.second; ++it)
      {
        v.push_back(it->second);
      }

      if(v.size() >= 2){
        auto last = Merger::merge(v.begin(), v.end(), N(x, t), make_tree(x,t), binop, gen );
        forest.erase(x);
        for(auto it = v.begin(); it != last; ++it){
          forest.insert(x, *it);
        }
      }
    }
  }

};

/**
* @brief Discrete time simulation in a Wright-Fisher Population.
* @details This simulator consists in a backward coalescent simulation
*          that traces back the ancestry of a sample of gene copies sampled in a
*          populations. Either the user wants to perform generation-by-generation
*          coalescence during a given number of generations (leading to a Forest of
*          coalescent trees rather a single genealogy), or the user would be interested
*          in coalescing lineages until MRCA has been found.
* @ingroup simulator
*/
class DiscreteTimeWrightFisher
{

public:

  //! \typedef forest type
  template<typename Space, typename Tree>
  using forest_type = quetzal::coalescence::Forest<Space, Tree>;

  /* @brief Coalesce a spatial forest of trees in a Wrigh-Fisher population until MRCA has been found.
   *
   * @tparam Space the coordinate type
   * @tparam Tree the tree type
   * @tparam Generator a random number Generator
   * @tparam Binop a BranchingOperation functor for coalescence
   * @tparam TimeFun an intialization functor for coalescence
   *
   * @param forest the spatial distribution of lineages to coalesce
   * @param N number of gene copies in the Wright-Fisher population
   * @param gen a random number Generator
   * @param branch a branching binary operation (functor usually given by the selected coalescence policy).
   * @param make_tree a initialization functor (functor usually given by the selected coalescence policy).
   *
   * @detail Coalesce lineages by sampling the waiting times directly in their distribution.
   *
   * @return One single tree.
  */
  template<typename Space, typename Tree, typename Generator, typename Binop, typename TimeFun>
  static Tree coalesce(forest_type<Space, Tree> const& forest, unsigned int N, Generator& gen, Binop branch, TimeFun make_tree)
  {

    if(forest.nb_trees() == 0){

      throw std::logic_error("The forest to coalesce is empty.");

    }else if(forest.nb_trees()==1){

      auto range = forest.trees_at_same_position(*(forest.positions().begin()));
      assert(std::distance(range.first, range.second) == 1);
      return range.first->second;

    }
    assert(forest.nb_trees() >= 2 && "Trying to coalesce less than 2 nodes.");
    auto trees = forest.get_all_trees();
    return coalesce(trees, N, gen, branch, make_tree);
  }

  /* @brief Simulates the number of generations to wait before the next coalescence event
  *
  * @tparam Generator a random number Generator
  *
  * @param k number of lineages
  * @param N number of gene copies in the Wright-Fisher population
  * @param gen a random number Generator
  *
  * @detail Samples the number of generations to wait before the next coalescence
  * event in a geometric distribution of parameter {k \choose 2} / N
  * (see Molecular evolution, a statistical approach, Z. Yang, 9.2 p 313).
  *
  * @return the number of generations to wait before the next coalescence event.
  */
  template<typename Generator>
  static unsigned int sample_waiting_time(unsigned int k, unsigned int N, Generator& gen)
  {
    assert(N > k);
    double mean = static_cast<double>(N) / boost::math::binomial_coefficient<double>(k, 2);
    double p = 1 / (1 + mean);
    assert( 0 < p && p <= 1);
    std::geometric_distribution dist(p);
    return dist(gen);
  }

  /*
   * @brief Coalesce a non-spatial forest of trees in a Wrigh-Fisher population until MRCA has been found.
   *
   * @tparam Tree the tree type
   * @tparam Generator a random number Generator
   * @tparam Binop a BranchingOperation functor for coalescence
   * @tparam TimeFun an intialization functor for coalescence
   *
   * @param trees the set of lineages to coalesce
   * @param N number of gene copies in the Wright-Fisher population
   * @param gen a random number Generator
   * @param branch a branching binary operation (functor usually given by the selected coalescence policy).
   * @param make_tree a initialization functor (functor usually given by the selected coalescence policy).
   *
   * @detail Coalesce lineages by sampling the waiting times directly in their
   * distribution. The algorithm will perform one generation of simultaneous
   * multiple merges if the number of lineages to coalesce k is superior to the
   * population size N.
   *
   * @return One single tree.
   */
  template<typename Tree, typename Generator, typename Binop, typename TimeFun>
  static Tree coalesce(std::vector<Tree> & trees, unsigned int N, Generator& gen, Binop branch, TimeFun make_tree)
  {

    assert(trees.size() >= 2 && "Trying to coalesce less than 2 nodes.");

    if (trees.size() > N) // Wright Fisher assumptions are clearly violated
    {
      // Let's do one generation of simultaneous multiple merges to fix it.
      unsigned int g = 1;
      using smm_type = coalescence::SimultaneousMultipleMerger<coalescence::occupancy_spectrum::on_the_fly>;
      auto last = smm_type::merge(trees.begin(), trees.end(), N, make_tree(g), branch, gen);
      trees.erase(last, trees.end());
    }

    unsigned int k = trees.size();
    assert( k <= N && "number of lineages greater than ancestral Wright-Fisher population size.");
    auto last = trees.end();

    while( k > 1 ){
      unsigned int g = sample_waiting_time(k, N, gen);
      last = quetzal::coalescence::binary_merge(trees.begin(), last, make_tree(g), branch, gen);
      --k;
    }

    return *(trees.begin());
  }


  /*
   * @brief Coalesce a non-spatial forest of trees in a Wrigh-Fisher population
   * during a finite number of generations.
   *
   * @tparam Tree the tree type
   * @tparam Generator a random number Generator
   * @tparam Binop a BranchingOperation functor for coalescence
   * @tparam TimeFun an intialization functor for coalescence
   *
   * @param trees the set of lineages to coalesce
   * @param N number of gene copies in the Wright-Fisher population
   * @param gen a random number Generator
   * g the number of generations that lasts the coalescence process
   * @param branch a branching binary operation (functor usually given by the selected coalescence policy).
   * @param make_tree a initialization functor (functor usually given by the selected coalescence policy).
   *
   * @detail Coalesce lineages by sampling the waiting times directly in their
   * distribution. The algorithm will perform one generation of simultaneous
   * multiple merges if the number of lineages to coalesce k is superior to the
   * population size N.
   *
   * @return A set of trees, possibly of length 1 if MRCA was found during the g generations.
   */
  template<
  typename MergerType = coalescence::SimultaneousMultipleMerger<coalescence::occupancy_spectrum::on_the_fly>,
  typename Tree, typename Generator, typename Binop, typename TimeFun>
  static std::vector<Tree> coalesce(std::vector<Tree> & trees, unsigned int N, unsigned int g, Generator& gen, Binop branch, TimeFun make_tree)
  {
    assert(trees.size() >= 2 && "Trying to coalesce less than 2 nodes.");
    assert(g > 0 && "Number of generations for the coalescence process should be at least 1");
    using merger_type = MergerType;
    unsigned int t = 0;
    auto last = trees.end();
    while( t < g && std::distance(trees.begin(), last) > 1)
    {
      last = merger_type::merge(trees.begin(), last, N, make_tree(t), branch, gen);
      ++t;
    }
    trees.erase(last, trees.end());
    return trees;
  }

};

    } // namespace simulator
  } // namespace quetzal

  #endif
