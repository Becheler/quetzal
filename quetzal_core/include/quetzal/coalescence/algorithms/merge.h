// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __COALESCENCE_MERGE_ALGORITHM_H_INCLUDED__
#define __COALESCENCE_MERGE_ALGORITHM_H_INCLUDED__

/** @file */

#include <algorithm> // std::shuffle
#include <iterator> // std::advance
#include <cassert>
#include <functional> // std::plus
#include <type_traits> // std::is_invocable
#include <iostream>

namespace quetzal{
namespace coalescence {

  /**
   * \ingroup coal_algorithms
   * \brief merges 2 randomly selected elements in a range.
   * \param first iterator at the begin of the range
   * \param last iterator to the past-the-end element.
   * \param init the value at which parent is initialized
   * \param op binary operation function object that will be applied for branching a
               child to its parent. The binary operator takes the parent value a
               (initialized to init) and the value of the child b. The signature
               of the function should be equivalent to the following:
               `Ret fun(const Type1 &parent, const Type2 &child);`
   * \param g a random generator that must meet the requirements of UniformRandomBitGenerator concept.
   * \return An iterator to the element that follows the last element of the nodes remaining after coalescence.
             The function cannot alter the properties of the object containing the range of elements
             (i.e., it cannot alter the size of an array or a container):
             signaling the new size of the shortened range is done by returning an iterator to the element
             that should be considered its new past-the-end element. The range between first and
             this iterator includes all the remaining nodes in the sequence.
   * \remark BidirectionalIterator must meet the requirements of ValueSwappable and RandomAccessIterator.
   * \section Example
   * \snippet coalescence/algorithms/test/binary_merge_test.cpp Example
   * \section Output
   * \include coalescence/algorithms/test/binary_merge_test.output
   */
  template<class BidirectionalIterator, class T, class BinaryOperation, class Generator>
  BidirectionalIterator binary_merge(BidirectionalIterator first, BidirectionalIterator last, T init, BinaryOperation op, Generator& g)
  {
    assert(std::distance(first, last) > 1 && "Coalescence should operate on a range containing more than one element.");
    std::shuffle(first, last, g);
    if constexpr (std::is_invocable<T>::value){
      *first = op(init(), *first);
    } else {
      *first = op(init, *first);
    }
    *first = op(*first, *(--last));
    return last;
  }

  /**
   * \ingroup coal_algorithms
   * \brief merges 2 randomly selected elements in a range, using sum operand between elements.
   * \param first iterator at the begin of the range
   * \param last iterator to the past-the-end element.
   * \param g a random generator that must meet the requirements of UniformRandomBitGenerator concept.
   *
   * \return An iterator to the element that follows the last element of the nodes remaining after coalescence.
             The function cannot alter the properties of the object containing the range of elements
             (i.e., it cannot alter the size of an array or a container):
             signaling the new size of the shortened range is done by returning an iterator to the element
             that should be considered its new past-the-end element. The range between first and
             this iterator includes all the remaining nodes in the sequence.
   *
   * \remark the type `T` `BidirectionalIterator::value_type` must be defined.
   * \remark the `T` default constructor is used to set the value at which the parent is initialized.
   * \remark With `a` and `b` two objects of type `T`, the expression `a + B` must be defined.
   * \remark BidirectionalIterator must meet the requirements of ValueSwappable and RandomAccessIterator.
   *
   * \section Example
   * \snippet coalescence/algorithms/test/binary_merge_test.cpp Example
   * \section Output
   * \include coalescence/algorithms/test/binary_merge_test.output
   */
  template<class BidirectionalIterator, class Generator>
  BidirectionalIterator binary_merge(BidirectionalIterator first, BidirectionalIterator last, Generator& g) {
    assert(std::distance(first, last) > 1 && "Coalescence should operate on a range containing more than one element.");
    using T = typename BidirectionalIterator::value_type;
    return binary_merge(first, last, T(), std::plus<T>(), g);
  }

  /**
   * \ingroup coal_algorithms
   * \brief merges randomly selected elements in a range according to an occupancy spectrum.
   * \param first iterator at the begin of the range
   * \param last iterator to the past-the-end element.
   * \param init the value at which parent is initialized
   * \param sp occupancy spectrum giving the merging configuration
   * \param op binary operation function object that will be applied for branching a
               child to its parent. The binary operator takes the parent value a
               (initialized to init) and the value of the child b. The signature
               of the function should be equivalent to the following:
               `Ret fun(const Type1 &parent, const Type2 &child);`
   * \param g a random generator that must meet the requirements of UniformRandomBitGenerator concept.
   * \return An iterator to the element that follows the last element of the nodes remaining after coalescence.
             The function cannot alter the properties of the object containing the range of elements
             (i.e., it cannot alter the size of an array or a container):
             signaling the new size of the shortened range is done by returning an iterator to the element
             that should be considered its new past-the-end element. The range between first and
             this iterator includes all the remaining nodes in the sequence.
   * \remark BidirectionalIterator must meet the requirements of ValueSwappable and RandomAccessIterator.
   * \section Example
   * \snippet coalescence/algorithms/test/simultaneous_multiple_merge_test.cpp Example
   * \section Output
   * \include coalescence/algorithms/test/simultaneous_multiple_merge_test.output
   */
  template<class BidirectionalIterator, class T, class BinaryOperation, class OccupancySpectrum, class Generator>
  BidirectionalIterator simultaneous_multiple_merge(BidirectionalIterator first, BidirectionalIterator last,
                                   T init, OccupancySpectrum const& sp, BinaryOperation op, Generator& g) {
      assert(std::distance(first, last) > 1 && "Coalescence should operate on a range containing more than one element.");
      std::shuffle(first, last, g);

       // directly go to binary merge.
      auto m_it = sp.cbegin();
      std::advance(m_it, 2);
      int j = 2;

      while(m_it != sp.cend()){
        for(unsigned int i = 1; i <= *m_it; ++i){ // loop on the m_j parents
          if constexpr (std::is_invocable<T>::value){
            *first = op(init(), *first);
          }else {
            *first = op(init, *first);
          }
          for(int k = 1; k < j; ++k){ // loop on the others children
            *first = op(*first, *(--last));
          }
          ++first;
        }
        ++j;
        ++m_it;
      }
      return last;
  }

  /**
   * \ingroup coal_algorithms
   * \brief merges randomly selected elements in a range according to an occupancy spectrum.
   * \param first iterator at the begin of the range
   * \param last iterator to the past-the-end element.
   * \param init the value at which parent is initialized
   * \param sp occupancy spectrum giving the merging configuration
   * \param op binary operation function object that will be applied for branching a
               child to its parent. The binary operator takes the parent value a
               (initialized to init) and the value of the child b. The signature
               of the function should be equivalent to the following:
               `Ret fun(const Type1 &parent, const Type2 &child);`
   * \param g a random generator that must meet the requirements of UniformRandomBitGenerator concept.
   * \return An iterator to the element that follows the last element of the nodes remaining after coalescence.
             The function cannot alter the properties of the object containing the range of elements
             (i.e., it cannot alter the size of an array or a container):
             signaling the new size of the shortened range is done by returning an iterator to the element
             that should be considered its new past-the-end element. The range between first and
             this iterator includes all the remaining nodes in the sequence.
   * \remark The type `T` `BidirectionalIterator::value_type` must be defined.
   * \remark The `T` default constructor is used to set the value at which the parent is initialized.
   * \remark With `a` and `b` two objects of type `T`, the expression `a + B` must be defined.
   * \remark BidirectionalIterator must meet the requirements of ValueSwappable and RandomAccessIterator.
   * \section Example
   * \snippet coalescence/algorithms/test/simultaneous_multiple_merge_test.cpp Example
   * \section Output
   * \include coalescence/algorithms/test/simultaneous_multiple_merge_test.output
   */
  template<class BidirectionalIterator, class OccupancySpectrum, class Generator>
  BidirectionalIterator
  simultaneous_multiple_merge(BidirectionalIterator first, BidirectionalIterator last, OccupancySpectrum const& sp, Generator& g) {
      assert(std::distance(first, last) > 1 && "Coalescence should operate on a range containing more than one element.");
      using T = typename BidirectionalIterator::value_type;
      return simultaneous_multiple_merge(first, last, T(), sp, std::plus<T>(), g);
  }

} // namespace coalescence
} // namespace quetzal

#endif
