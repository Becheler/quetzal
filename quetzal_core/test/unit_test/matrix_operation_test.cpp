// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE utils_test

#include <quetzal/utils.h>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;

#include <cmath>

BOOST_AUTO_TEST_SUITE( matrix_operation_test )

BOOST_AUTO_TEST_CASE( boost_features )
{
  using matrix_type = typename boost::numeric::ublas::matrix<double>;
  matrix_type m(3,4);
  for(unsigned int i = 0; i < m.size1(); ++i ){
    for(unsigned int j = 0; j < m.size2(); ++j){
      m(i,j) = static_cast<double>(i+j);
    }
  }
  std::cout << m << std::endl;
  matrix_type m2 = quetzal::utils::divide_terms_by_row_sum(m);
  std::cout << m2 << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()
