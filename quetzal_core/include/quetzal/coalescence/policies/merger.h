// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __COALESCENCE_MERGERS_H_INCLUDED__
#define __COALESCENCE_MERGERS_H_INCLUDED__

#include "../algorithms/merge.h"
#include "../occupancy_spectrum/spectrum_creation_policy.h" // default policies

#include <iterator> // std::distance
#include <cassert>
#include <functional> // std::plus

namespace quetzal{
namespace coalescence {

  /*!
   * \brief Discrete generation binary merger.
   *
   *  Defines a policy class to coalesce two nodes selected uniformely at random
   *  in a range, with a probability \f$1/N\f$. Its interface makes it exchangeable
   *  with other merger policies.
   *
   * \ingroup coal_policies
   * \section Example
   * \snippet coalescence/policies/test/BinaryMerger/test.cpp Example
   * \section Output
   * \include coalescence/policies/test/BinaryMerger/test.output
   */
  struct BinaryMerger{


    /**
     * \brief merges 2 randomly selected elements in a range.
     *
     * With a probability \f$1/N\f$, merges 2 elements selected uniformely at random in a range.
     *
     * \param first iterator at the begin of the range
     * \param last iterator to the past-the-end element.
     * \param N the number of individuals in the population
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
     *
     * \remark BidirectionalIterator must meet the requirements of ValueSwappable and RandomAccessIterator.
     * \section Example
     * \snippet coalescence/policies/test/BinaryMerger/test.cpp Example
     * \section Output
     * \include coalescence/policies/test/BinaryMerger/test.output
     */
    template<class BidirectionalIterator, class T, class BinaryOperation, class Generator>
    static auto
    merge(BidirectionalIterator first, BidirectionalIterator last, unsigned int N, T init, BinaryOperation const& binop, Generator& g)
    {
      assert(N >= 1 && "Population size should be more than 1 for evaluating coalescence probability" );
      assert(std::distance(first, last) > 1 && "Coalescence should operate on a range containing more than one element.");
      double k = static_cast<double>(std::distance(first, last));
      double coal_proba = (k*(k-1.0))/static_cast<double>(2*N);
      std::bernoulli_distribution d(coal_proba);
      if( d(g) ){
        last = binary_merge(first, last, init, binop, g);
      }
      return last;
    }

    /**
     * \brief merges 2 randomly selected elements in a range.
     *
     * With a probability \f$1/N\f$, merges 2 elements selected uniformely at random in a range.
     *
     * \param first iterator at the begin of the range
     * \param last iterator to the past-the-end element.
     * \param N the number of individuals in the population
     * \param g a random generator that must meet the requirements of UniformRandomBitGenerator concept.
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
     * \section Example
     * \snippet coalescence/policies/test/BinaryMerger/test.cpp Example
     * \section Output
     * \include coalescence/policies/test/BinaryMerger/test.output
     */
    template<class BidirectionalIterator, class Generator>
    static auto
    merge(BidirectionalIterator first, BidirectionalIterator last, unsigned int N, Generator& g)
    {
      assert(N >= 1 && "Population size should be positive for evaluating coalescence probability" );
      assert(std::distance(first, last) > 1 && "Coalescence should operate on a range containing more than one element.");
      using T = typename BidirectionalIterator::value_type;
      return merge(first, last, N, T(), std::plus<T>(), g);
    }

  };

  /*!
   * \brief Discrete generation simultaneous multiple merger.
   *
   *  Defines a policy class to coalesce multiple nodes selected uniformely at random
   *  in a range, according to an OccupancySpectrum. Its interface makes it exchangeable
   *  with other merger policies.
   *
   * \ingroup coal_policies
   * \tparam SpectrumCreationPolicy The policy used to generate an OccupancySpectrum.
   * \section Example
   * \snippet coalescence/policies/test/SimultaneousMultipleMerger/test.cpp Example
   * \section Output
   * \include coalescence/policies/test/SimultaneousMultipleMerger/test.output
   */
  template<class SpectrumCreationPolicy>
  struct SimultaneousMultipleMerger {


    /**
     * \brief merges multiple randomly selected elements in a range.
     *
     * Coalesce multiple nodes selected uniformely at random in a range, according to an OccupancySpectrum.
     *
     * \param first iterator at the begin of the range
     * \param last iterator to the past-the-end element.
     * \param N the number of individuals in the population
     * \param init the value at which parent is initialized
     * \param op binary operation function object that will be applied for branching a
                 child to its parent. The binary operator takes the parent value a
                 (initialized to init) and the value of the child b. The signature
                 of the function should be equivalent to the following:
                 `Ret fun(const Type1 &parent, const Type2 &child);`
     * \return An iterator to the element that follows the last element of the nodes remaining after coalescence.
               The function cannot alter the properties of the object containing the range of elements
               (i.e., it cannot alter the size of an array or a container):
               signaling the new size of the shortened range is done by returning an iterator to the element
               that should be considered its new past-the-end element. The range between first and
               this iterator includes all the remaining nodes in the sequence.
     * \remark BidirectionalIterator must meet the requirements of ValueSwappable and RandomAccessIterator.
     * \section Example
     * \snippet coalescence/policies/test/SimultaneousMultipleMerger/test.cpp Example
     * \section Output
     * \include coalescence/policies/test/SimultaneousMultipleMerger/test.output
     */
    template<class BidirectionalIterator, class T, class BinaryOperation, class Generator>
    static auto
    merge(BidirectionalIterator first, BidirectionalIterator last, unsigned int N, T const& init, BinaryOperation const& binop, Generator& g)
    {
      assert(N >= 1 && "Population size should be positive for evaluating coalescence probability" );
      assert(std::distance(first, last) > 1 && "Coalescence should operate on a range containing more than one element.");
      unsigned int k = std::distance(first, last);
      return simultaneous_multiple_merge(first, last, init, SpectrumCreationPolicy::sample(k, N, g), binop, g);
    }


    /**
     * \brief merges multiple randomly selected elements in a range.
     *
     * Coalesce multiple nodes selected uniformely at random in a range, according to an OccupancySpectrum.
     *
     * \param first iterator at the begin of the range
     * \param last iterator to the past-the-end element.
     * \param N the number of individuals in the population
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
     * \section Example
     * \snippet coalescence/policies/test/SimultaneousMultipleMerger/test.cpp Example
     * \section Output
     * \include coalescence/policies/test/SimultaneousMultipleMerger/test.output
     */
    template<class BidirectionalIterator, class Generator>
    static auto
    merge(BidirectionalIterator first, BidirectionalIterator last, unsigned int N, Generator& g)
    {
      assert(N >= 1 && "Population size should be positive for evaluating coalescence probability" );
      assert(std::distance(first, last) > 1 && "Coalescence should operate on a range containing more than one element.");
      unsigned int k = std::distance(first, last);
      return simultaneous_multiple_merge(first, last, SpectrumCreationPolicy::sample(k, N, g), g);
    }

  };

} // namespace coalescence
} // namespace quetzal

#endif
