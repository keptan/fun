#ifndef SORT_H
#define SORT_H 

#include "list.h" 
#include "niave_tree.h"
#include "string.h" 
#include "utility.h"

template <typename T, typename F = Ord<T>>
bool sorted (const List<T> l, const F compare = ordOverload)
{
	if(length(l) < 2)
		return true; 

	if(! compare( peek( pop(l)), peek(l)))
		return false; 

	return sorted(pop(l));
}

template <typename T> 
List<List<T>> shuffleSplit (const List<T> l, const List<T> a = List<T>(), const List<T> b = List<T>())
{
	const auto compare = [](const auto a, const auto b){ return length(a) > length(b);};

	const List<T> larger =  max(a,b,compare);
	const List<T> smaller = min(a,b,compare);


	if(length(l) == 0)
		return push(List<List<T>>(smaller),larger);

	if(length(l) == 1)
		return push (List<List<T>>( push(smaller, peek(l))), larger);

	return shuffleSplit (pop(l), push(smaller, peek(l)), larger);
}

template <typename T, typename F = Ord<T>>
List<T> mergeOrdered (const List<T> a, const List<T> b, const F compare = ordOverload)
{
	if(length(a) == 0 ||  length(b) == 0)
		return push(a, b);

	const auto c = [=](const auto first, const auto second){ return compare(peek(first), peek(second));};

	const List<T> larger =  max(a,b,c);
	const List<T> smaller = min(a,b,c);

	return push(List<T>( peek(smaller)), mergeOrdered( pop(smaller), larger));
}

template <typename T, typename F = Ord<T>>
List<T> mergeSort (const List<T> l, const F compare = ordOverload)
{
	if(sorted(l, compare))
		return l; 

	const auto split = shuffleSplit(l); 
	const auto first  = mergeSort( peek(split), compare); 
	const auto second = mergeSort( peek_back(split), compare); 

	return mergeOrdered(first,second, compare);
}

#endif
	
