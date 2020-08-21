// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/
#include "../expressive.h"

#include <vector>
#include <algorithm>

/*!
 * \brief Reference table produced by ABC sampling procedures in the prior predictive distribution.
 *
 * In some ABC procedures, a parameter \f$\theta\f$ is generated from the prior \f$\pi(\theta)\f$,
 * a data \f$z\f$ is simulated from the likelyhood and a dimension reduction function \f$\eta\f$
 * is computed on \f$z\f$. The set of simulated statistics is called the
 * reference table.
 *
 * \tparam ParamType the parameter type.
 * \tparam DataType the type of the data generated.
 * \tparam Cont the type wrapping the couple parameter and the data value.
 *
 * The template parameter Cont must meet the following requirements:
 *
 * * `Cont` satisfies [CopyConstructible](http://en.cppreference.com/w/cpp/concept/CopyConstructible)
 * * `Cont` satisfies [CopyAssignable](http://en.cppreference.com/w/cpp/concept/CopyAssignable)
 *
 * Given
 *
 * * `T`, the type named by `Cont::data_type`
 * * `P`, the type named by `Cont::param_type`, which:
 * * `s`, a value of type `Cont`
 * * `x`, a value of type `T`
 * * `p`, a value of type `P`
 *
 * The following expressions must be valid and have their specified effects :
 *
 * Expression          | Type | Notes
 * --------------------|------|-------------------------------------|
 * `Cont::data_type`   | `T`    | the type of the simulated data    |
 * `Cont::param_type`  | `P`    | the type of the parameter         |
 * `s.data()`          | `T`    | Returns the simulated data        |
 * `s.param()`         | `P`    | Returns the parameter used for simulating the data |
 *
 * \ingroup abc
 * \section Example
 * \snippet abc/test/test.cpp Example
 * \section Output
 * \include abc/test/test.output
 */
template<template <class,class> class Cont, class ParamType, class DataType>
class ReferenceTable{

private:
  std::vector<Cont<ParamType,DataType>> m_table;
public:

  auto begin() {return m_table.begin(); }

  auto end() {return m_table.end(); }

  auto cbegin() const {return m_table.cbegin(); }

  auto cend() const {return m_table.cend(); }

  auto size() const {return m_table.size(); }

  void reserve(size_t n){ m_table.reserve(n); }

  void push_back(Cont<ParamType,DataType> const& sample ){
    m_table.push_back(sample);
  }

  void push_back( Cont<ParamType,DataType>&& sample ){
    m_table.push_back(std::move(sample));
  }

  template<class... Args>
  void emplace_back(Args&&... args) {
    m_table.emplace_back(std::forward<Args>(args)...);
  }

  template<typename UnaryOperation>
  auto compute_summary_statistics(UnaryOperation const& eta) const {
    using sumstat_type = typename std::result_of_t<UnaryOperation(const DataType &)>;
    ReferenceTable<Cont, ParamType, sumstat_type> s;
    for(auto const& it : m_table){
      s.emplace_back(it.param(), eta(it.data()));
    }
    return s;
  }

  template<typename BinaryOperation>
  auto compute_distance_to(DataType const& obs, BinaryOperation const& rho) const {
    using Ret = typename quetzal::expressive::return_type<BinaryOperation>;
    ReferenceTable<Cont, ParamType, Ret> s;
    for(auto const& it : m_table){
      s.emplace_back(it.param(), rho(obs, it.data()));
    }
    return s;
  }

}; // end class ReferenceTable
