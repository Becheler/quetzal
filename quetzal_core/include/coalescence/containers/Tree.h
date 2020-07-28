// Copyright 2016 Arnaud Becheler <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
************************************************************************/

#ifndef TREE_H_INCLUDED
#define TREE_H_INCLUDED

#include <vector>

namespace quetzal{
namespace coalescence{

/*!
 * \ingroup coal_containers
 * \brief Tree topology where each node contains a data field.
 * \tparam CellT Cell type, ie the value of the node. The requirements that are imposed on CellT
 *               depend on the actual operations performed on Tree<CellT>.
 */

// Inspired from code publicated by Pyramidev here : http://www.developpez.net/forums/d1623113/c-cpp/cpp/debuter/loi-demeter/#post8894717

template<class CellT>
class Tree
{
private:

	Tree<CellT>*             m_parent;
	CellT                 m_cell;
	std::vector<Tree<CellT>> m_children;

public:

  /**
	  * \brief Default constructor
	  * \remark CellT must be default constructible
		* \section Example
		* \snippet coalescence/containers/test/Tree/initialization_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Tree/initialization_test.output
		*/
	Tree();

	/**
	  * \brief Constructor with data copy
		* \param cell the value of the tree cell
	  * \remark CellT must be copy constructible.
		* \section Example
		* \snippet coalescence/containers/test/Tree/initialization_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Tree/initialization_test.output
		*/
	Tree(const CellT& cell);

	/**
	  * \brief Constructor with data move
		* \param cell the value of the tree cell
	  * \remark CellT must be move constructible.
		* \section Example
		* \snippet coalescence/containers/test/Tree/initialization_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Tree/initialization_test.output
		*/
	Tree(CellT&& cell) noexcept;

	/**
	  * \brief Copy constructor
		* \param other the tree to be constructed from
	  * \remark CellT must be copy constructible.
		* \section Example
		* \snippet coalescence/containers/test/Tree/initialization_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Tree/initialization_test.output
	  */
	Tree(const Tree<CellT>& other);

	/**
	  * \brief Move constructor
		* \param other the tree to be constructed from
	  * \remark CellT must be move constructible.
		* \section Example
		* \snippet coalescence/containers/test/Tree/initialization_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Tree/initialization_test.output
	  */
	Tree(Tree<CellT>&& other) noexcept;

	/**
	  * \brief Constructor
		* \param cell the value of the tree cell
		* \param children a vector of subtrees
	  * \remark CellT must be move constructible.
		* \section Example
		* \snippet coalescence/containers/test/Tree/initialization_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Tree/initialization_test.output
	  */
	Tree(CellT&& cell, std::vector<Tree<CellT>>&& children) noexcept;

	/**
	  * \brief Copy assignment operator
	  * \remark CellT must be copy constructible and move assignable.
		* \section Example
		* \snippet coalescence/containers/test/Tree/initialization_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Tree/initialization_test.output
	  */
	Tree<CellT>& operator=(const Tree<CellT>& other);

	/**
	  * \brief Move assignment operator
	  * \remark CellT must be move assignable.
		* \section Example
		* \snippet coalescence/containers/test/Tree/initialization_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Tree/initialization_test.output
	  */
	Tree<CellT>& operator=(Tree<CellT>&& other) noexcept;

	/**
	  * \brief Cell accessor
		* \return a const-reference to the tree cell
	  * \remark Read-only data access
		* \section Example
		* \snippet coalescence/containers/test/Tree/basics_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Tree/basics_test.output
	  */
	const CellT& cell() const;

	/**
	  * \brief  Cell accessor
		* \return a reference to the tree cell
	  * \remark Read-and-write data access
		* \section Example
		* \snippet coalescence/containers/test/Tree/basics_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Tree/basics_test.output
	  */
 	CellT& cell();

	/**
	  * \brief Add a subtree to the children list of the tree
		* \param subtree the subtree to be added
		* \return a reference on the new subtree
		* \section Example
		* \snippet coalescence/containers/test/Tree/basics_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Tree/basics_test.output
	  */
 	Tree<CellT>& add_child(const Tree<CellT>& subtree);

	/**
		* \brief Add a subtree to the children list of the tree
		* \param subtree the subtree to be added
		* \return a reference on the new subtree
		* \section Example
		* \snippet coalescence/containers/test/Tree/basics_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Tree/basics_test.output
		*/
 	Tree<CellT>& add_child(Tree<CellT>&& subtree) noexcept;

	/**
		* \brief Add a subtree to the children list of the tree, initialize the tree
		* \param cell the cell value of the subtree to be added
		* \return a reference on the new subtree
		* \section Example
		* \snippet coalescence/containers/test/Tree/basics_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Tree/basics_test.output
		*/
 	Tree<CellT>& add_child(CellT&& cell) noexcept;

	/**
	  * \brief Checks whether the tree has a parent
		* \return Returns true if the tree has a parent, else returns false.
		* \section Example
		* \snippet coalescence/containers/test/Tree/basics_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Tree/basics_test.output
	  */
 	bool has_parent() const{
		return m_parent != nullptr;
	}
	
	/**
	  * \brief Checks whether the tree has one or more child
		* \return Returns false if the tree has no child, else returns true.
		* \section Example
		* \snippet coalescence/containers/test/Tree/basics_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Tree/basics_test.output
	  */
 	bool has_children() const;

	/**
	  * \brief Applies a function object to each node encountered in a depth first search algorithm
		* \param op unary operation function object that will be applied to the data member
		            of the tree. The unary operator takes the cell as argument. The signature
								of the function should be equivalent to the following:
								`void op(const CellT & cell);`.
		* \section Example
	  * \snippet coalescence/containers/test/Tree/pre_order_DFS_test.cpp Example
		* \section Output
		* \include coalescence/algorithms/test/binary_merge_test.output
	  */
 	template<typename UnaryOperation>
	void visit_cells_by_pre_order_DFS(UnaryOperation op) const
	{
		op(this->cell());
		for(auto const& child : this->m_children){
			child.visit_cells_by_pre_order_DFS(op);
		}
	}

	template<class UnaryOperation>
	void visit_subtrees_by_pre_order_DFS(UnaryOperation op) {
		op(*this);
		for(auto & child : this->m_children){
			child.visit_subtrees_by_pre_order_DFS(op);
		}
	}

	template<typename Op1, typename Op2, typename Op3>
	void visit_subtrees_by_generic_DFS(Op1 preorder, Op2 inorder, Op3 postorder){
		preorder(*this);
		for(auto& child : this->m_children)
		{
			child.visit_subtrees_by_generic_DFS(preorder, inorder, postorder);
			inorder(*this);
		}
		postorder(*this);
	}

	auto& parent(){
		assert(has_parent());
		return *m_parent;
	}

	auto const& parent() const {
		assert(has_parent());
		return *m_parent;
	}

	unsigned int nb_children() const { return m_children.size(); }
	/**
	  * \brief Applies a function object to each leave encountered in a depth first search algorithm
		* \param op unary operation function object that will be applied to the data member
		            of the tree. The unary operator takes the cell as argument. The signature
								of the function should be equivalent to the following:
								`void op(const CellT & cell);`.
		* \section Example
	  * \snippet coalescence/containers/test/Tree/access_leaves_by_DFS_test.cpp Example
		* \section Output
		* \include coalescence/containers/test/Tree/access_leaves_by_DFS_test.output
	  */
	template<typename UnaryOperation>
	void visit_leaves_cells_by_DFS(UnaryOperation op) const
	{
		if(has_children()){
			for(auto const& child : this->m_children){
				child.visit_leaves_cells_by_DFS(op);
			}
		}else{
			op(this->cell());
		}
	}

};









template<typename CellT>
Tree<CellT>::Tree() :
	m_parent(nullptr), m_cell(), m_children()
{}

template<typename CellT>
Tree<CellT>::Tree(const CellT& cell) :
	m_parent(nullptr), m_cell(cell), m_children()
{}

template<typename CellT>
Tree<CellT>::Tree(CellT&& cell) noexcept :
	m_parent(nullptr), m_cell(std::move(cell)), m_children()
{}


template<class CellT>
Tree<CellT>::Tree(const Tree<CellT>& other) :
	m_parent(nullptr), m_cell(other.m_cell), m_children(other.m_children)
{
	for(Tree<CellT>& child : m_children)
		child.m_parent = this;
}


template<class CellT>
Tree<CellT>::Tree(Tree<CellT>&& other) noexcept :
	m_parent(nullptr), m_cell(std::move(other.m_cell)), m_children(std::move(other.m_children))
{
	for(Tree<CellT>& child : m_children)
		child.m_parent = this;
}


template<class CellT>
Tree<CellT>::Tree(CellT&& cell, std::vector<Tree<CellT>>&& children) noexcept :
	m_parent(nullptr), m_cell(std::move(cell)), m_children(std::move(children))
{
	for(Tree<CellT>& child : m_children)
		child.m_parent = this;
}


template<class CellT>
Tree<CellT>& Tree<CellT>::operator=(const Tree<CellT>& other)
{
	Tree<CellT> otherCopy(other);
	*this = std::move(otherCopy);
	return *this;
}


template<class CellT>
Tree<CellT>& Tree<CellT>::operator=(Tree<CellT>&& other) noexcept
{
	m_cell     = std::move(other.m_cell);
	m_children = std::move(other.m_children);
	for(Tree<CellT>& child : m_children)
		child.m_parent = this;
	return *this;
}

template<class CellT>
const CellT& Tree<CellT>::cell() const
{
	return m_cell;
}

template<class CellT>
CellT& Tree<CellT>::cell()
{
 	return m_cell;
}

template<class CellT>
Tree<CellT>& Tree<CellT>::add_child(const Tree<CellT>& subtree)
{
	Tree<CellT> otherCopy(subtree);
	otherCopy.m_parent = this;
	m_children.push_back(std::move(otherCopy));
	return *this;
}

template<class CellT>
Tree<CellT>& Tree<CellT>::add_child(Tree<CellT>&& subtree) noexcept
{
	subtree.m_parent = this;
	m_children.push_back(std::move(subtree));
	return *this;
}

template<class CellT>
Tree<CellT>& Tree<CellT>::add_child(CellT&& cell) noexcept{
	Tree<CellT> subtree(std::move(cell));
	subtree.m_parent = this;
	m_children.push_back(std::move(subtree));
	return m_children.back();
}

template<class CellT>
bool Tree<CellT>::has_children() const {
	return !m_children.empty();
}





} // namespace coalescence
} // namespace quetzal

#endif
