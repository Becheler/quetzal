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

  std::mt19937 g;

  model_type model;

  std::uniform_real_distribution<double> prior_on_lambda(1.,100.);

  auto abc = quetzal::abc::make_ABC(model, prior_on_lambda);

  auto reference_table = abc.sample_prior_predictive_distribution(30, g);

  std::cout << "Reference Table generated :\n\nlambda\tdata" << std::endl;
  for(auto const& it : reference_table){
    std::cout << it.param().mean() << "\t" << it.data() << std::endl;
  }

  return 0;
}

//! [Example]
