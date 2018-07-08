#ifndef TRANSFORM_H 
#define TRANSFORM_H 

#include "list.h" 
#include "niave_tree.h"
#include <iostream>

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

	return push(left, (push(right, tree.head->res)));
}

//AHHHHHHHHHH
template <typename T>
NiaveTree<T> ListToTree (const List<T> list, const NiaveTree<T> tree = NiaveTree<T>(nullptr))
{
	if (length(list) == 0)
		return tree;

	return ListToTree(pop(list), push(tree, peek(list)));
}


template <typename T, typename F = T(T)>
List<T> map (const List<T> list, F fun)
{
	if (length(list) == 0)
		return List<T>(nullptr);

	if(length(list) == 1)
		return List<T>(peek(list));

	return push( map(pop(list), fun), fun(peek(list)));
}

template <typename T, typename F = T(T,T)>
T fold (const List<T> list, T init, F fun)
{
	if (length(list) == 0)
		return init; 

	return fun( peek(list), fold(pop(list), init, fun));
}

//should these be implicit conversions?
//also these are all unbalancing if the list gets sorted
template <typename T, typename F = T(T)>
NiaveTree<T> map (const NiaveTree<T> tree, F fun)
{
	return ListToTree(map(TreeToList(tree), fun));
}

template <typename T, typename F = T(T,T)>
T fold (const NiaveTree<T> tree, T init, F fun)
{
	return ListToTree( fold( TreeToList(tree), init, fun));
}

#endif
