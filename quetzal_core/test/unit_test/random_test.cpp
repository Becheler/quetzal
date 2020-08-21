// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE random_test

#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;

#include <quetzal/random.h>
#include <iostream>

BOOST_AUTO_TEST_SUITE( random_suite )

BOOST_AUTO_TEST_CASE( discrete_distribution )
{
  using quetzal::random::DiscreteDistribution;
  std::mt19937 gen;
	DiscreteDistribution<std::string> d({"Bordeaux", "Paris"}, {0.2,0.8});
  std::map<std::string, int> m;
  for(int n=0; n<10000; ++n) {
      ++m[d(gen)];
  }
  for(auto p : m) {
      std::cout << p.first << " generated " << p.second << " times\n";
  }
}

BOOST_AUTO_TEST_CASE( discrete_distribution_init )
{
  std::mt19937 gen;
  using quetzal::random::DiscreteDistribution;

	// initialization by default constructor
	DiscreteDistribution<int> first;

	// initialization by copy constructor
	DiscreteDistribution<int> second(first);

  // initialization by move constructor
	DiscreteDistribution<int> third(std::move(first)); // first should not be used again

	// Also initialization by copy constructor
	DiscreteDistribution<int> fourth = second;

	// assignment by copy assignment operator
	fourth = third;

	// assignment by move assignment operator
	fourth = std::move(second); // second should not be used again

  std::discrete_distribution<int> e;

  // Default construction is a Dirac on the default constructible value.
  std::cout << fourth(gen) << std::endl;

}

BOOST_AUTO_TEST_CASE( time_transition_kernel_init )
{

  	using State = int;
  	using Time = int;
  	using Distribution = std::discrete_distribution<State>;
  	using Kernel = quetzal::random::TransitionKernel<Time, Distribution>;

  	// initialization by default constructor
  	Kernel first;

  	// initialization by copy constructor
  	Kernel second(first);

  	// Also initialization by copy constructor
  	Kernel third = first;

  	// assignment by copy assignment operator
  	second = third;

  	// assignment by move assignment operator
  	second = std::move(third);

  	// initialization copying distribution
  	Distribution d({1, 10, 20, 30});
  	State x = 1;
  	Time t = 0;
  	Kernel A(x, t, d);

  	// initialization moving distribution
  	Kernel B(x, t, std::move(d));
  	std::mt19937 gen;
  	std::cout << B(gen, x, t) << std::endl;
}

BOOST_AUTO_TEST_CASE (time_transition_kernel)
{
  using deme_ID_type = int;
  using time_type = int;
  using law_type = std::discrete_distribution<deme_ID_type>;
  using kernel_type = quetzal::random::TransitionKernel<time_type, law_type>;

  std::mt19937 gen;
  law_type d({0.5,0.5});
  kernel_type kernel;

  // Random walk
  deme_ID_type x0 = {0};
  std::cout << x0;
  for(time_type t = 0; t < 10; ++t){
    std::cout << " -> " << x0 ;
    if( ! kernel.has_distribution(x0, t)){
      kernel.set(x0, t, d);
    }
    x0 = kernel(gen, x0, t); // sample new state
  }
}

BOOST_AUTO_TEST_CASE (transition_kernel_init)
{
  using state_type = int;
  using distribution_type = std::discrete_distribution<state_type>;
  using kernel_type = quetzal::random::TransitionKernel<distribution_type>;

  // initialization by default constructor
  kernel_type first;

  // initialization by copy constructor
  kernel_type second(first);

  // Also initialization by copy constructor
  kernel_type third = first;

  // assignment by copy assignment operator
  second = third;

  // assignment by move assignment operator
  second = std::move(third);

  // initialization copying distribution
  distribution_type d({1, 10, 20, 30});
  kernel_type A(1, d);

  // initialization moving cell
  kernel_type B(1, std::move(d));
}

BOOST_AUTO_TEST_CASE (transition_kernel)
{
  using deme_ID_type = int;
  using law_type = std::discrete_distribution<deme_ID_type>;
  using kernel_type = quetzal::random::TransitionKernel<law_type>;

  law_type distribution({0.5,0.5}); // return deme 0 or deme 1 with same probability
  kernel_type kernel;
  std::mt19937 gen;

  // Random walk
  deme_ID_type x0 = 0;
  std::cout << x0;
  for(int i = 0; i < 10; ++i){
    std::cout << " -> " << x0 ;
    if( ! kernel.has_distribution(x0)){
      kernel.set(x0, distribution);
    }
    x0 = kernel(gen, x0); // sample new state
  }
}

BOOST_AUTO_TEST_SUITE_END()
