#ifndef TRANSFORM_H 
#define TRANSFORM_H 

#include "list.h" 
#include "niave_tree.h"
#include <iostream>
#include <functional>

//replace with some kind of traverse generic map -> List push?
template<typename T>
List<T> TreeToList (const NiaveTree<T> tree, const List<T> list = List<T>(nullptr))
{
	if (tree.head == nullptr)
		return list;

	//lmao autism
	//make + overloads 
	const auto left  = TreeToList<T>(tree.head->left, list);
	const auto right = TreeToList<T>(tree.head->right, list);

	return left + tree.head->res + right;
}

//AHHHHHHHHHH
template <typename T>
NiaveTree<T> ListToTree (const List<T> list, const NiaveTree<T> tree = NiaveTree<T>(nullptr))
{
	if (length(list) == 0)
		return tree;

	return ListToTree(pop(list), peek(list) + tree);
}


template <typename T, typename F = T(T)>
List<T> map (const List<T> list, F fun)
{
	if (length(list) == 0)
		return List<T>(nullptr);

	if(length(list) == 1)
		return List<T>(fun(peek(list)));

	return fun(peek(list)) + map(pop(list), fun);
}

template <typename T, typename G, typename F = std::function<G(T,G)>>
G foldr (const List<T> list, F fun, G init)
{
	if (length(list) == 0)
		return init; 

	return fun( peek(list), foldr(pop(list), fun, init));
}

template <typename T, typename G, typename F = std::function<G(T,G)>>
G foldl (const List<T> list, F fun, G init)
{
	if (length(list) == 0)
		return init; 

	return fun( peek_back(list), foldl(pop_back(list), fun, init));
}

//should these be implicit conversions?
//also these are all unbalancing if the list gets sorted
template <typename T, typename F = T(T)>
NiaveTree<T> map (const NiaveTree<T> tree, F fun)
{
	return ListToTree(map(TreeToList(tree), fun));
}

template <typename T, typename F = T(T,T)>
T fold (const NiaveTree<T> tree, F fun, T init)
{
	return ListToTree( fold( TreeToList(tree), fun, init));
}

#endif
