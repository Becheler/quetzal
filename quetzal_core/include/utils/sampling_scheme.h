// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __SAMPLING_SCHEME_H_INCLUDED__
#define __SAMPLING_SCHEME_H_INCLUDED__

#include <random>
#include <map>

namespace quetzal {

//! Generic sampling schemes simulation
namespace sampling_scheme {

  //! Base class for sampling schemes parameter classes
  class param_base_class {
  private:
    unsigned int _n;
  public:
    /* Constructor
     * @param n sampling size
     */
    param_base_class(unsigned int n): _n(n) {}

    param_base_class() = default;

    /* Read the sampling size parameter
    */
    unsigned int sampling_size() const
    {
      return _n;
    }

    /* Set the sampling size parameter
    */
    void sampling_size(unsigned int n)
    {
      this->_n = n;
    }
  };


//! Policy to sample coordinates at random in a space
template<typename X, typename N>
class constrained_sampling {
public:

  /* @brief parameter of the sampling scheme
  */
  class param_type : public param_base_class {
    using param_base_class::param_base_class;
  };

  /* @brief type used to refer geographic coordinates
  */
  using coord_type = X;

  /* @brief type of the function. Signature must be equivalent to `double (coord_type const& x)`
   *         and returns the weight associated to x.
  */

private:

  std::vector<double> _weights;
  const std::vector<coord_type> & _space;
  N _population_size;
  param_type _param;

  template<typename F>
  auto compute_weights(std::vector<coord_type> const& space, F f) const {
    std::vector<double> w(space.size(), 0);
    std::transform(space.cbegin(), space.cend(), w.begin(), f);
    return w;
  }

public:


  template<typename F1>
  constrained_sampling(std::vector<coord_type> const& space, F1 f, N pop_size, unsigned int n) :
  _weights(compute_weights(space, f)),
  _space(space),
  _population_size(pop_size),
  _param(n)
  {
    assert(n > 0);
  }

  param_type param() const { return _param; }

  void param(unsigned int n){ _param = param_type(n); }

    /* @brief Sample points in a discrete space, uniformely at random
   * @remark a same coordinates can be sampled several times
   *
   * @tparam Cont a container of coordinates
   * @tparam Generator a RandomNumberGenerator
   *
   * @param space coordinates to be sampled
   * @param n the number of points to sample
   * @param gen a random number generator
   *
   * @return a 'std::map<Cont::value_type, unsigned int>' giving the sampling intensities
   * at the sampled coordinates.
   */
	template<typename Generator>
	auto operator()(Generator& gen) const
	{
		std::discrete_distribution<size_t> dist(_weights.begin(), _weights.end());
		std::map<coord_type, unsigned int> sample;
    unsigned int i = 0;
    unsigned int nb_try = 0;
    while(i < _param.sampling_size() && nb_try < 10 * (_param.sampling_size()) ){
			auto id = dist(gen);
      if(sample[_space[id]] < _population_size(_space[id]) ){
        sample[_space[id]] += 1;
        ++i;
      }
      ++nb_try;
		}
    if(sample.empty()){
      throw std::logic_error("Sampling scheme unable to populate a sample.");
    }
		return sample;
	}

}; // constrained_sampling

template<typename X, typename F, typename N>
auto make_constrained_sampler(std::vector<X> const& space, F f, N pop_size, unsigned int n){
  if(space.empty()){
    throw std::logic_error("Space for sampling scheme is empty.");
  }
  return constrained_sampling<X, N>(space, f, pop_size, n);
}

template<typename X, typename N>
auto make_unif_constrained_sampler(std::vector<X> const& space, N pop_size, unsigned int n){
  if(space.empty()){
    throw std::logic_error("Space for sampling scheme is empty.");
  }
  auto uniform_weighting = [](X) -> double { return 1.0; };
  return constrained_sampling<X, N>(space, uniform_weighting, pop_size, n);
}

} // namespace sampling_scheme
} // namespace quetzal

#endif
