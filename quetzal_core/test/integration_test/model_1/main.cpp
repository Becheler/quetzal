// Copyright 2018 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
************************************************************************/

#include "quetzal/quetzal.h"

#include <boost/math/special_functions/binomial.hpp> // binomial coefficient
#include <random>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cassert>
#include <vector>

// Namespace shortnames
namespace geo = quetzal::geography;
namespace demo = quetzal::demography;
namespace sim = quetzal::simulator;
namespace schemes = quetzal::sampling_scheme;
namespace coal = quetzal::coalescence;
namespace expr = quetzal::expressive;

// Parameter 1 : path to geoTiff environmental dataset
int main(int argc, char* argv[])
{

  std::cout << "Reading " << argc << " input parameters" << std::endl;

  /******************************
   * Geospatial dataset
   *****************************/
  std::cout << "1 ... Reading environment file " << argv[1] << std::endl;

  using time_type = int;
  using landscape_type = geo::DiscreteLandscape<std::string,time_type>;
  using coord_type = landscape_type::coord_type;

  landscape_type env( {{"temp", argv[1]}},   {time_type(0)} );


  /******************************
   * Simulator configuration
   *****************************/
   std::cout << "2 ... Initializing spatial coalescence simulator" << std::endl;

  using demographic_policy = demo::strategy::mass_based;
  using coalescence_policy = coal::policies::distance_to_parent<coord_type, time_type>;
  using simulator_type = sim::SpatiallyExplicit<coord_type, time_type, demographic_policy, coalescence_policy>;

  // Introduction deme
  coord_type x_0 = env.reproject_to_centroid(coord_type(40.0, -3.0));
  // Introduction time
  time_type t_0 = 0;
  // Number of introduced gene copies
  demographic_policy::value_type N_0 = 100;
  // Simulator initialization
  simulator_type simulator(x_0, t_0, N_0);
  // Ancestral wright-fisher population size
  simulator.ancestral_Wright_Fisher_N(N_0);
  // Sampling time
  time_type t_sampling = 300;
  // Sampling size
  unsigned int n = 200;

  /******************************
   * Niche and growth functions
   *****************************/
  std::cout << "3 ... Initializing demographic growth niche functions" << std::endl;

  // A literal factory is used to transforms literals into function of space and time
  using expr::literal_factory;
  using expr::use;
  literal_factory<coord_type, time_type> lit;

  // Growth rate is constant across space and time
  auto r = lit(5);

  // Retrieve temperature function of space (homogeneous across time)
  auto temp = env["temp"];
  // Wrap it to ignore time argument
  auto s = [temp](coord_type x, time_type){return temp(x, 0);};
  // Compose it to get a carrying capacity expression
	unsigned int factor = 5;
  auto K = use(s)*lit(factor);

  // Retrieve the population size function
  auto pop_sizes = simulator.pop_size_history();
  // Tranform it into an expression
  auto N = use( [pop_sizes](coord_type x, time_type t){ return pop_sizes(x,t);} );

  // Compose everything in a logistic growth expression
  auto g = N * ( lit(1) + r ) / ( lit(1) + ( (r * N)/K ));
  auto sim_children = [g](auto& gen, coord_type const&x, time_type t){
    std::poisson_distribution<unsigned int> poisson(g(x,t));
    return poisson(gen);
  };

  // Initalize RNG
  std::random_device rd;
  std::mt19937 gen(rd());

  /******************
   * Dispersal
   ******************/
  std::cout << "4 ... Initializing dispersal kernel" << std::endl;

  // Retrieve the demic structure
  auto const& space = env.geographic_definition_space();
  // Define an emigrant rate
  double emigrant_rate = 0.1;
  // Define the friction according to environmental features
  auto is_temp_high = [temp](coord_type x, time_type){return temp(x, 0) >= 15.0 ? true : false ;};
  auto friction = [is_temp_high](coord_type const& x){return is_temp_high(x,0) ? 0.3 : 0.7 ;};
  // Capture an expression to find nearest neighbours
  auto env_ref = std::cref(env);
  auto get_neighbors = [env_ref](coord_type const& x){return env_ref.get().four_nearest_defined_cells(x);};
  // Build a dispersal kernel object
  auto dispersal = demographic_policy::make_neighboring_migration(space, emigrant_rate, friction, get_neighbors);

  /**************************
  * Demographic expansion
  **************************/
  std::cout << "5 ... Simulating demographic expansion" << std::endl;
  simulator.expand_demography(t_sampling, sim_children, dispersal, gen);

  /************************************************************
  *  Sample uniformely at random across the distribution area
  ************************************************************/
  std::cout << "6 ... Simulating sampling scheme" << std::endl;

  // Retrieve the non-empty demes at sampling time
  std::vector<coord_type> distribution_area = pop_sizes.get().definition_space(t_sampling);
  // Retrieve the last expression
  auto last_N = [N, t_sampling](coord_type const& x){return N(x, t_sampling);};

  auto unif_sampler = quetzal::sampling_scheme::make_unif_constrained_sampler(distribution_area, last_N, n);
  auto sample = unif_sampler(gen);

  /**********************
   *  Data visualization
   **********************/
  std::cout << "7 ... Exporting GIS data for visualization" << std::endl;

  std::string N_filename = "N.tif";
  env.export_to_geotiff(N, t_0, t_sampling, [&pop_sizes](time_type const& t){return pop_sizes.get().definition_space(t);}, N_filename);
  env.export_to_shapefile(sample, "sample.shp");

  /**********************
   *  Coalescence
   **********************/

  std::cout << "8 ... Begining coalescence" << std::endl;

  auto results = simulator.coalesce_to_mrca<>(sample, t_sampling, gen);

  std::ofstream file;
  file.open ("trees.txt");
  file << results;
  file.close();

  std::cout << "Done" << std::endl;

  return 0;

}
