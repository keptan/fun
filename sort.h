#ifndef SORT_H
#define SORT_H 

#include "list.h" 
#include "utility.h"

int count = 0;

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
List<List<T>> shuffleSplit (const List<T> l, const List<T> larger = List<T>(nullptr), const List<T> smaller = List<T>(nullptr))
{
	if(length(l) == 0)
		return larger + List<List<T>>(smaller);

	if(length(l) == 1)
		return larger + List<List<T>>(smaller + peek(l));

	return shuffleSplit (pop(l), peek(l) + smaller, larger);
}

template <typename T, typename F = Ord<T>>
List<T> mergeOrdered (const List<T> a, const List<T> b, const F compare = ordOverload)
{
	if(length(a) == 0 ||  length(b) == 0)
		return a + b;

	const auto c = [&](const auto first, const auto second){ return compare(peek(first), peek(second));};

	const List<T> larger =  max(a,b,c);
	const List<T> smaller = min(a,b,c);

	return peek(smaller) + mergeOrdered(pop(smaller), larger);
}

template <typename T>
List<T> mergeSort (const List<T> l, const Ord<T> compare = ordOverload)
{
	if(sorted(l, compare))
		return l; 


	count += 1;
	const auto split  = shuffleSplit(l); 
	const auto first  = mergeSort( peek(split), compare); 
	const auto second = mergeSort( peek_back(split), compare); 

	return mergeOrdered(first,second, compare);
}

//we would have checks during debug, but then trust during release?
//a lazy sorted list would return the 1st biggest and then second biggest ect 
//would a lazy sorted list even be more efficient? 
//assert that sorted, or actually construct mergesort? 
/*
template <typename T>
struct SortedList<T> : public List<T>
{
	public:
	*/
	
	


#endif
	
