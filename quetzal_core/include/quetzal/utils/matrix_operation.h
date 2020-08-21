// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __MATRIX_OPERATIONS_H_INCLUDED__
#define __MATRIX_OPERATIONS_H_INCLUDED__

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/operation.hpp>
#include <algorithm>

namespace quetzal {

namespace utils {

template<typename matrix_type>
auto divide_terms_by_row_sum(matrix_type const& A) {
  using namespace boost::numeric::ublas;
  auto v = prod(scalar_vector<double>( A.size2(), 1.0), trans(A));
  vector<double> w ( v.size() );
  for(unsigned int i = 0; i < v.size(); ++i){
    double a = v (i);
    assert( a > 0);
    w(i) = 1.0 / a ;
  }
  compressed_matrix<double> S( w.size(), w.size(), 0.0 );
  assert(S.size2() == A.size1());
  for(unsigned int i = 0; i < v.size(); ++i ){
    S(i,i) = w(i);
  }
  matrix_type M(A.size1(),A.size2());
  axpy_prod(S, A, M);
  return M;
}

}
}

#endif
