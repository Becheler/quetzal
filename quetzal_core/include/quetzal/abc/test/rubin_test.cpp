// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

// compiles with g++ -o abc_test abc_test.cpp -std=c++14 -Wall

#include "../abc.h"

//! [Example]

#include <random>    // std::poisson_distribution, std::uniform_real_distribution
#include <iostream>  // std::endl

int main () {

  using data_type = unsigned int;
  using model_type = std::poisson_distribution<data_type>;
  using param_type = typename model_type::param_type;
  std::mt19937 g;

  auto true_param = param_type(10);
  model_type model;
  auto pseudo_observed_data = model(g, true_param);

  std::uniform_real_distribution<double> prior(1.,100.);

  auto abc = quetzal::abc::make_ABC(model, prior);

  auto posterior = abc.rubin_rejection_sampler(30, pseudo_observed_data, g);

  std::cout << "Pseudo-observed data value : " << pseudo_observed_data << "\n";
  std::cout << "True parameter value : " << true_param.mean() << "\n";
  std::cout << "Posterior sample :\n\n";

  for(auto const& it : posterior){
    std::cout << it.mean() << std::endl;
  }

  return 0;
}
//! [Example]
