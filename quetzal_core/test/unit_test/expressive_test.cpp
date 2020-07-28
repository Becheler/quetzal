// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE expressive_test

#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;

#include <quetzal/expressive.h>

BOOST_AUTO_TEST_SUITE( expressive )

BOOST_AUTO_TEST_CASE( using_lambdas_and_literals )
{
  using quetzal::expressive::use;

  quetzal::expressive::literal_factory<int,int> lit;
  auto g  = lit(5);

  auto f = [](int a, int b){ return a + b; };

  auto h  = use(f) + g ;
  assert( h(2,3) == 10);

  BOOST_CHECK_EQUAL( h(2,3), 10);

}

BOOST_AUTO_TEST_CASE( composing_functors )
{
  using quetzal::expressive::use;

  // NxN -> N -> string
  auto inner = use( [](int a, int b){ return a + b;} );
  auto outer = use( [](int){ return "c";} );

  auto composed = quetzal::expressive::compose(outer, inner);

  BOOST_CHECK_EQUAL( composed(1,2), "c");

}

BOOST_AUTO_TEST_SUITE_END()
