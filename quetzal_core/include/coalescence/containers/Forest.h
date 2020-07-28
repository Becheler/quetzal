// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
************************************************************************/

#ifndef __FOREST_H_INCLUDED__
#define __FOREST_H_INCLUDED__

#include <unordered_map>
#include <set>
#include <vector>
#include "assert.h"

namespace quetzal{
namespace coalescence{

/*!
 * \ingroup coal_containers
 * \brief Collection of geo-localized coalescents.
 * \tparam Position Type of the geographic coordinate of the associated tree.
 * \tparam Tree Type of the coalescence tree.
 * \section Example
 * \snippet coalescence/containers/test/Forest/Forest_test.cpp Example
 * \section Output
 * \include coalescence/containers/test/Forest/Forest_test.output
 */
template<typename Position, typename Tree>
class Forest{

private:

using SelfType = Forest<Position, Tree>;
using data_type = std::unordered_multimap<Position, Tree>;
using size_type = typename data_type::size_type;
data_type m_data;

public:

	using position_type = Position;
	using iterator = typename data_type::iterator;
	using const_iterator = typename data_type::const_iterator;

	/**
	  * \brief Default constructor
	  * \remark Position and Tree must be default constructible
		* \section Example
		* \snippet coalescence/containers/test/Forest/initialization_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Forest/initialization_test.output
		*/
	Forest();

	/**
	  * \brief Copy constructor
		* \param other the Forest to be constructed from
	  * \remark Position and Tree must be copy constructible.
		* \section Example
		* \snippet coalescence/containers/test/Forest/initialization_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Forest/initialization_test.output
	  */
	Forest(const Forest<Position, Tree>& other);

	/**
	  * \brief Move constructor
		* \param other the Forest to be constructed from
	  * \remark Position and Tree must be copy constructible.
		* \section Example
		* \snippet coalescence/containers/test/Tree/initialization_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Tree/initialization_test.output
	  */
	Forest(Forest<Position, Tree>&& other) noexcept;

	/**
	  * \brief Copy assignment operator
	  * \remark Position and Tree must be copy constructible and move assignable.
		* \section Example
		* \snippet coalescence/containers/test/Forest/initialization_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Forest/initialization_test.output
	  */
	Forest<Position, Tree>& operator=(const Forest<Position, Tree>& other);

	/**
	  * \brief Move assignment operator
	  * \remark CellT must be move assignable.
		* \section Example
		* \snippet coalescence/containers/test/Forest/initialization_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Forest/initialization_test.output
	  */
	Forest<Position, Tree>& operator=(Forest<Position, Tree>&& other) noexcept;

	/**
	  * \brief returns iterator to the begin
		* \section Example
		* \snippet coalescence/containers/test/Forest/basics_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Forest/basics_test.output
	  */
	iterator begin();

	/**
	  * \brief returns iterator to the end
		* \section Example
		* \snippet coalescence/containers/test/Forest/basics_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Forest/basics_test.output
	  */
	iterator end();

	/**
	  * \brief returns iterator to the begin
		* \section Example
		* \snippet coalescence/containers/test/Forest/basics_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Forest/basics_test.output
	  */
	const_iterator begin() const;

	/**
	  * \brief returns iterator to the begin
		* \section Example
		* \snippet coalescence/containers/test/Forest/basics_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Forest/basics_test.output
	  */
	const_iterator end() const;

	/**
	  * \brief returns iterator to the begin
		* \section Example
		* \snippet coalescence/containers/test/Forest/basics_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Forest/basics_test.output
	  */
	const_iterator cbegin() const;

	/**
	  * \brief returns iterator to the begin
		* \section Example
		* \snippet coalescence/containers/test/Forest/basics_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Forest/basics_test.output
	  */
	const_iterator cend() const;

	/**
	  * \brief number of trees in the forest
		* \section Example
		* \snippet coalescence/containers/test/Forest/basics_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Forest/basics_test.output
	  */
	unsigned int nb_trees() const;

	/**
	  * \brief number of trees in the forest at a given position
		* \section Example
		* \snippet coalescence/containers/test/Forest/basics_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Forest/basics_test.output
	  */
	unsigned int nb_trees(Position const& position) const;

	/**
	  * \brief non-modifying access to trees in the forest at a given position
		* \return a range [first, last) given by a pair of const iterators. The second member
		          of the pair is the past-the-end iterator
		* \section Example
		* \snippet coalescence/containers/test/Forest/basics_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Forest/basics_test.output
	  */
	std::pair<const_iterator,const_iterator> trees_at_same_position (const Position& position) const;

	/**
	  * \brief access to trees in the forest at a given position
		* \return a range [first, last) given by a pair of iterators. The second member
		          of the pair is the past-the-end iterator
		* \section Example
		* \snippet coalescence/containers/test/Forest/basics_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Forest/basics_test.output
	  */
	std::pair<iterator,iterator> trees_at_same_position (const Position& position);

	/**
	  * \brief insert a new tree at a given position
		* \return an iterator to the inserted tree
		* \remark Position and Tree should be copy constructible.
		* \section Example
		* \snippet coalescence/containers/test/Forest/basics_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Forest/basics_test.output
	  */
	iterator insert(Position const& position, Tree const& tree);

	/**
	  * \brief insert a new tree at a given position
		* \return an iterator to the inserted tree
		* \remark Position and Tree should be move constructible.
		* \section Example
		* \snippet coalescence/containers/test/Forest/basics_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Forest/basics_test.output
	  */
	iterator insert(Position const& position, Tree&& tree) noexcept;

	/**
	  * \brief insert a vector of trees at a given position
		* \return a reference to the Forest
		* \remark Position and Tree should be copy constructible.
		* \section Example
		* \snippet coalescence/containers/test/Forest/basics_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Forest/basics_test.output
	  */
	Forest<Position, Tree>& insert(Position const& position, std::vector<Tree> const& trees);

	/**
	  * \brief insert a vector of trees at a given position
		* \return a reference to the Forest
		* \remark Position should be copy constructible.
		* \section Example
		* \snippet coalescence/containers/test/Forest/basics_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Forest/basics_test.output
	  */
	Forest<Position, Tree>& insert(Position const& position, std::vector<Tree>&& trees) noexcept;

	/**
	  * \brief erase a position from the Forest.
		* \return the number of deleted trees.
		* \section Example
		* \snippet coalescence/containers/test/Forest/basics_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Forest/basics_test.output
	  */
	size_type erase(Position const& x);

	/**
	  * \brief remove elements from the Forest.
		* \return the number of deleted trees.
		* \section Example
		* \snippet coalescence/containers/test/Forest/basics_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Forest/basics_test.output
	  */
	void erase(iterator first, iterator last){
		m_data.erase(first,last);
	}

	/**
	  * \brief positions in the forest
		* \return a set of coordinates giving the spatial occupancy of the forest.
		* \section Example
		* \snippet coalescence/containers/test/Forest/basics_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Forest/basics_test.output
	  */
	std::set<Position> positions() const;

	/**
	  * \brief copies all trees in the forest
		* \return a vector of trees.
	  */
  std::vector<Tree> get_all_trees() const {
    std::vector<Tree> trees;
    trees.reserve(this->nb_trees());
    for(auto & it : *this){
      trees.push_back(it.second);
    }
    return trees;
  }
};


template<typename Position, typename Tree>
Forest<Position, Tree>::Forest() :
	m_data()
{}

template<typename Position, typename Tree>
Forest<Position, Tree>::Forest(const SelfType& other) :
	m_data(other.m_data)
{}

template<typename Position, typename Tree>
Forest<Position, Tree>::Forest(SelfType&& other) noexcept :
	m_data(std::move(other.m_data))
{}

template<typename Position, typename Tree>
Forest<Position, Tree>& Forest<Position, Tree>::operator=(const SelfType& other)
{
	Forest<Position, Tree> otherCopy(other);
	*this = std::move(otherCopy);
	return *this;
}

template<typename Position, typename Tree>
Forest<Position, Tree>& Forest<Position, Tree>::operator=(SelfType&& other) noexcept
{
	m_data = std::move(other.m_data);
	return *this;
}

template<typename Position, typename Tree>
typename Forest<Position, Tree>::iterator Forest<Position, Tree>::begin(){
	return m_data.begin();
}

template<typename Position, typename Tree>
typename Forest<Position, Tree>::iterator Forest<Position, Tree>::end(){
	return m_data.end();
}

template<typename Position, typename Tree>
typename Forest<Position, Tree>::const_iterator Forest<Position, Tree>::begin() const {
	return m_data.begin();
}

template<typename Position, typename Tree>
typename Forest<Position, Tree>::const_iterator Forest<Position, Tree>::end() const {
	return m_data.end();
}

template<typename Position, typename Tree>
typename Forest<Position, Tree>::const_iterator Forest<Position, Tree>::cbegin() const {
	return m_data.cbegin();
}

template<typename Position, typename Tree>
typename Forest<Position, Tree>::const_iterator Forest<Position, Tree>::cend() const {
	return m_data.cend();
}

template<typename Position, typename Tree>
unsigned int Forest<Position, Tree>::nb_trees() const {
	return m_data.size();
}

template<typename Position, typename Tree>
unsigned int Forest<Position, Tree>::nb_trees(Position const& position) const {
	return m_data.count(position);
}

template<typename Position, typename Tree>
std::pair<typename Forest<Position, Tree>::const_iterator, typename Forest<Position, Tree>::const_iterator> Forest<Position, Tree>::trees_at_same_position (const Position& position) const{
	assert(m_data.count(position) > 0 && "no trees at position" );
	return m_data.equal_range(position);
}

template<typename Position, typename Tree>
std::pair<typename Forest<Position, Tree>::iterator, typename Forest<Position, Tree>::iterator> Forest<Position, Tree>::trees_at_same_position (const Position& position){
	assert(m_data.count(position) > 0 && "no trees at position" );
	return m_data.equal_range(position);
}

template<typename Position, typename Tree>
typename Forest<Position, Tree>::iterator Forest<Position, Tree>::insert(Position const& position, Tree const& tree){
	return m_data.insert( std::pair<Position, Tree>(position, tree));
}

template<typename Position, typename Tree>
typename Forest<Position, Tree>::iterator Forest<Position, Tree>::insert(Position const& position, Tree&& tree) noexcept {
	return m_data.insert( std::pair<Position, Tree>(position, std::move(tree)));
}

template<typename Position, typename Tree>
typename Forest<Position, Tree>::size_type Forest<Position, Tree>::erase(Position const& x){
	return m_data.erase(x);
}

template<typename Position, typename Tree>
Forest<Position, Tree>& Forest<Position, Tree>::insert(Position const& position, std::vector<Tree> const& trees){
	for(auto const& it : trees){
		insert(position, it);
	}
	return *this;
}

template<typename Position, typename Tree>
Forest<Position, Tree>& Forest<Position, Tree>::insert(Position const& position, std::vector<Tree>&& trees) noexcept{
	for(auto&& it : trees){
		insert(position, std::move(it));
	}
	return *this;
}

template<typename Position, typename Tree>
std::set<Position> Forest<Position, Tree>::positions() const {
	std::set<Position> s;
	for(auto const& it : m_data){
		s.insert(it.first);
	}
	return s;
}

} // namespace coalescence
} // namespace quetzal

#endif
