// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE coalescence_test

#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;

#include <quetzal/coalescence.h>

BOOST_AUTO_TEST_SUITE( coalescence )

BOOST_AUTO_TEST_CASE( binary_merge )
{
  using node_type = int;
  std::vector<node_type> nodes = {1,1,1,1};
  std::mt19937 rng;
  using quetzal::coalescence::binary_merge;
  // First coalescence using default parent initialization and operator
  auto last = binary_merge(nodes.begin(), nodes.end(), rng);
  std::copy(nodes.begin(), last, std::ostream_iterator<node_type>(std::cout, " "));
  std::cout << std::endl;
  // Second coalescence using custom initialization and operator
  last = binary_merge(nodes.begin(), last, 100, std::minus<node_type>(), rng);
  std::copy(nodes.begin(), last, std::ostream_iterator<node_type>(std::cout, " "));
}

BOOST_AUTO_TEST_CASE( simultaneous_multiple_merge )
{
  using node_type = int;
  std::vector<node_type> nodes = {1,1,1,1,1};
  // 99 parents with no child, 0 parent with 1 child, 1 parent with 2 children, 1 parent with 3 children
  std::vector<unsigned int> spectrum = {98,0,1,1};
  std::mt19937 rng;
  using quetzal::coalescence::simultaneous_multiple_merge;
  auto last = simultaneous_multiple_merge(nodes.begin(), nodes.end(), spectrum, rng);
  std::copy(nodes.begin(), last, std::ostream_iterator<node_type>(std::cout, " "));
}

BOOST_AUTO_TEST_CASE( forest )
{
  using position_type = int;
	using tree_type = std::string;
	using forest_type = quetzal::coalescence::Forest<position_type, tree_type>;
	forest_type forest;
	auto summarize = [](auto const& forest){
	std::cout << "Forest has "
	  << forest.nb_trees() << " trees at "
		<< forest.positions().size() << " geographic positions." << std::endl;
	};
	auto print = [](auto const& forest){
		for(auto const& it : forest){
			std::cout << "Position: " << it.first << "\t" << "Species: " << it.second << std::endl;
		}
	};
	// Insert some trees at some positions
	std::vector<tree_type> trees = {"populus_tremula","salix_nigra"};
	forest.insert(1,  std::move(trees));
	forest.insert(2, std::string("populus_tremula"));
	forest.insert(3, std::string("quercus_robur"));
	summarize(forest);
	print(forest);
	std::cout << "\nErase coordinate 1:" << std::endl;
	forest.erase(1);
	summarize(forest);
	print(forest);
}

BOOST_AUTO_TEST_CASE( forest_init )
{
  using position_type = int;
	using tree_type =int;
	using forest_type = quetzal::coalescence::Forest<position_type, tree_type>;
	// initialization by default constructor
	forest_type a;
	// initialization by copy constructor
	forest_type b(a);
	// initialization by move constructor
	forest_type c(std::move(b)); // b should not be used again
	// Also initialization by copy constructor
	forest_type d = a;
	// assignment by copy assignment operator
	d = c;
	// assignment by move assignment operator
	d = std::move(c);
}

BOOST_AUTO_TEST_CASE (forest_insert)
{
  // \remark These are very basic types sufficient for testing, but have little to see with coalescence.
  using position_type = int;
  using std::string;
  using tree_type = string;
  using forest_type = quetzal::coalescence::Forest<position_type, tree_type>;
  forest_type forest;
  // Insert some trees at some positions
  forest.insert(1, string("salix_nigra"));
  std::vector<string> u {"populus_tremula", "salix_nigra"};
  forest.insert(2, u);
  std::vector<string> v {"quercus_robur", "salix_nigra"};
  forest.insert(3, std::move(v));
  auto it = forest.insert(4, string("salix_nigra"));
  BOOST_CHECK_EQUAL(it->first , 4);
  BOOST_CHECK_EQUAL(forest.nb_trees() , 6);
  BOOST_CHECK_EQUAL(forest.nb_trees(3) , 2);
  for(auto const& it : forest){
    std::cout << "Position "<< it.first << " : " << it.second << std::endl;
  }
}

BOOST_AUTO_TEST_CASE (tree_init)
{
  using quetzal::coalescence::Tree;
  using std::string;
  // initialization by default constructor
  Tree<int> a;
  Tree<int> b;
  Tree<int> c;
  // assignment by copy assignment operator
  a = b;
  // assignment by move assignment operator
  c = std::move(b); // b should not be used anymore !
  int cell = 12;
  // initialization copying cell
  Tree<int> d(cell);
  // initialization moving cell
  Tree<int> e(std::move(cell)); // cell should not be used anymore !
}

BOOST_AUTO_TEST_CASE ( tree_DFS)
{
  using quetzal::coalescence::Tree;
  using std::string;

  /* Topology :
   *             a
   *           /\ \
   *          b  d e
   *         /      \\
   *        c       f g
   */

  Tree<string> root("a");
  root.add_child("b").add_child("c");
  root.add_child("d");
  auto& e = root.add_child("e");
  e.add_child("f");
  e.add_child("g");
  std::vector<string> expected = {"c","d","f","g"};
  std::vector<string> v;
  auto f = [&v](string s){ v.push_back(s); };
  root.visit_leaves_cells_by_DFS(f);
  BOOST_TEST(v == expected);
}

BOOST_AUTO_TEST_CASE (tree_preorder_DFS)
{
  using quetzal::coalescence::Tree;
  using std::string;
  /* Topology :
   *             a
   *           /\ \
   *          b  d e
   *         /      \\
   *        c       f g
   */

  Tree<string> root("a");
  root.add_child("b").add_child("c");
  root.add_child("d");
  auto& e = root.add_child("e");
  e.add_child("f");
  e.add_child("g");
  std::vector<string> expected = {"a","b","c","d","e","f","g"};
  std::vector<string> v;
  auto f = [&v](string s){ v.push_back(s); };
  root.visit_cells_by_pre_order_DFS(f);
  BOOST_TEST(v == expected);
}

BOOST_AUTO_TEST_CASE (occupancy_spectrum_generator)
{
  using quetzal::coalescence::occupancy_spectrum::Generator;
  using spectrum_type = Generator::occupancy_spectrum_type;
  auto spectrum_handler = [](spectrum_type && M_j){
    std::copy(M_j.begin(), M_j.end(), std::ostream_iterator<spectrum_type::value_type>(std::cout, " "));
    std::cout << "\n";
  };
  Generator::generate(5, 10, spectrum_handler);
}

BOOST_AUTO_TEST_CASE (occupancy_spectrum__distribution_init)
{
  using quetzal::coalescence::occupancy_spectrum::OccupancySpectrumDistribution;
  // initialization by default constructor
  OccupancySpectrumDistribution<> a;
  // constructor
  OccupancySpectrumDistribution<> b(5,10);
  // initialization by move constructor
  OccupancySpectrumDistribution<> c(std::move(b)); // a can not be used again
  // deleted initialization by copy constructor
  // OccupancySpectrumDistribution d(c); // uncomment will cause a compilation error
  // deleted assignment by copy assignment operator
  // a = b; // uncomment will cause a compilation error
  // assignment by move assignment operator
  OccupancySpectrumDistribution<> e = std::move(c); // c should not be used anymore !
}

BOOST_AUTO_TEST_CASE (occupancy_spectrum_distribution)
{
  using quetzal::coalescence::occupancy_spectrum::OccupancySpectrumDistribution;
  unsigned int n = 5;
  unsigned int m = 10;

  OccupancySpectrumDistribution<> dist1(n, m);
  std::cout << "Throwing " << dist1.n() << " balls in " << dist1.m() << " urns.\n";
  std::cout << "Full distribution: "
            << dist1.support().size() << " configurations and weights sum to "
            << std::accumulate(dist1.weights().begin(), dist1.weights().end(), 0.0)
            << std::endl;

   std::cout << dist1 << "\n" << std::endl;

  // Sample from it :
  std::mt19937 g;
  auto spectrum = dist1(g);

  // Truncate the empty spectrum for memory optimization
  using quetzal::coalescence::occupancy_spectrum::truncate_tail;
  using quetzal::coalescence::occupancy_spectrum::return_always_true;
  OccupancySpectrumDistribution<return_always_true, truncate_tail> dist2(n, m);
  std::cout << "Shorten spectra:\n" << dist2 << "\n" << std::endl;

  // Filter the occupancy spectrum with small probability.
  auto pred = [](double p){
    bool keep = false;
    if(p > 0.01){ keep = true; }
    return keep;
  };
  OccupancySpectrumDistribution<decltype(pred)> dist3(n, m, pred);
  std::cout << "Approximated distribution:\n" << dist3 << "\n" << std::endl;

  // Combine strategies
  OccupancySpectrumDistribution<decltype(pred), truncate_tail> dist4(n, m, pred);
  std::cout << "Combined strategies:\n" << dist4 << "\n" << std::endl;
}

BOOST_AUTO_TEST_CASE(binary_merger)
{
  using node_type = std::string;
  using quetzal::coalescence::BinaryMerger;
  std::vector<node_type> nodes = {"a", "b", "c", "d"};
  unsigned int N = 3;
  std::mt19937 gen;
  std::cout << "\nNodes at sampling time :\n";
  std::copy(nodes.begin(), nodes.end(), std::ostream_iterator<node_type>(std::cout, "\n"));
  std::cout << "\n";
  auto last = BinaryMerger::merge(nodes.begin(), nodes.end(), N, gen);
  std::cout << "\nAfter one binary merge generation:\n";
  for(auto it = nodes.begin(); it != last; ++it){
    std::cout << *it << std::endl;
  }
}

BOOST_AUTO_TEST_CASE (simultaneous_multiple_merger)
{
  using node_type = std::string;
  using quetzal::coalescence::occupancy_spectrum::on_the_fly;
  using SMM = quetzal::coalescence::SimultaneousMultipleMerger<on_the_fly> ;
  std::vector<node_type> nodes = {"a", "b", "c", "d"};
  unsigned int N = 3;
  std::mt19937 gen;
  std::cout << "\nNodes at sampling time :\n";
  std::copy(nodes.begin(), nodes.end(), std::ostream_iterator<node_type>(std::cout, "\n"));
  std::cout << "\n";
  auto last = SMM::merge(nodes.begin(), nodes.end(), N, gen);
  std::cout << "\nAfter one simultaneous multiple merge generation:\n";
  for(auto it = nodes.begin(); it != last; ++it){
    std::cout << *it << std::endl;
  }
}

BOOST_AUTO_TEST_CASE (memoize )
{
  std::mt19937 g;
  // build and copy sample
  auto spectrum_a = quetzal::coalescence::occupancy_spectrum::utils::memoize_OSD<>(10,10)(g);
  // no need to build, sample directly in the memoized distribution, no copy
  auto const& spectrum_b = quetzal::coalescence::occupancy_spectrum::utils::memoize_OSD<>(10,10)(g);
  (void)spectrum_b;
}

BOOST_AUTO_TEST_CASE (spectrum_creation_policy)
{
  std::mt19937 g;

  auto a = quetzal::coalescence::occupancy_spectrum::on_the_fly::sample(10,10, g);
  for(auto const& it : a){
    std::cout << it << " ";
  }

  std::cout << std::endl;

  auto b = quetzal::coalescence::occupancy_spectrum::in_memoized_distribution<>::sample(10,10,g);
  for(auto const& it : b){
    std::cout << it << " ";
  }
}
BOOST_AUTO_TEST_SUITE_END()
