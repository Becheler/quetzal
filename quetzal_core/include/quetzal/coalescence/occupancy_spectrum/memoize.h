
// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __OCCUPANCY_SPECTRUM_UTILS_H_INCLUDED__
#define __OCCUPANCY_SPECTRUM_UTILS_H_INCLUDED__

#include "OccupancySpectrumDistribution.h"

#include <unordered_map>
#include <functional> // std::hash

namespace quetzal{
namespace coalescence {
namespace occupancy_spectrum {
namespace utils{

  struct MyHash
  {
  	std::size_t operator()(const std::pair<unsigned int, unsigned int>& param) const
  	{
  		static const auto hacher = std::hash<unsigned int>();
  		const std::size_t h1 = hacher(param.first);
  		const std::size_t h2 = hacher(param.second);
  		return h1 ^ (h2 << 1);
  	}
  };

  template
  <
  class UnaryPredicate = return_always_true,
  class SpectrumHandler = identity,
  class Int = cpp_int,
  class Float = cpp_dec_float_50
  >
  const OccupancySpectrumDistribution<UnaryPredicate, SpectrumHandler, Int, Float>&
  memoize_OSD(unsigned int k, unsigned int N)
  {
  	static std::unordered_map<std::pair<unsigned int, unsigned int>, OccupancySpectrumDistribution<UnaryPredicate, SpectrumHandler, Int, Float>, MyHash> cache;
  	const auto pair_k_N = std::make_pair(k, N);
  	const auto it = cache.find(pair_k_N);
  	if(it != cache.end())
  		return it->second;
  	const auto pair_itNewResult_true = cache.emplace(
      std::piecewise_construct,
      std::forward_as_tuple(pair_k_N),
      std::forward_as_tuple(k,N)
    );
  	return pair_itNewResult_true.first->second;
  }

} // namespace utils
} // namespace occupancy_spectrum
} // namespace coalescence
} // namespace quetzal


#endif
