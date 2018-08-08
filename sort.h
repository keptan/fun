#ifndef SORT_H
#define SORT_H 

#include "list.h" 
#include "utility.h"
#include <future>

int count = 0;

template <typename T, typename F = Ord<T>>
bool sorted (const List<T> l, const F compare = ordOverload)
{
	if(l.length() < 2)
		return true;

	if( !compare( l.pop().peek(), l.peek()))
		return false; 

	return sorted(l.pop());
}

template <typename T> 
List<List<T>> shuffleSplit (const List<T> l, const List<T> larger = List<T>(nullptr), const List<T> smaller = List<T>(nullptr))
{
	if(l.length() == 0)
		return larger + List<List<T>>(smaller);

	if(l.length() == 1)
		return larger + List<List<T>>(smaller + l.peek());

	return shuffleSplit (l.pop(), l.peek() + smaller, larger);
}

template <typename T, typename F = Ord<T>>
List<T> mergeOrdered (const List<T> a, const List<T> b, const F compare = ordOverload)
{
	if(a.length() == 0 || b.length() == 0)
		return a + b;

	const auto c = [&](const auto first, const auto second){ return compare(first.peek(), second.peek());};

	const List<T> larger =  max(a,b,c);
	const List<T> smaller = min(a,b,c);

	return smaller.peek() + mergeOrdered( smaller.pop(), larger);
}

template <typename T>
List<T> mergeSort (const List<T> l, const Ord<T> compare = ordOverload)
{
	if(sorted(l, compare))
		return l; 

	const auto split  = shuffleSplit(l); 
	const auto first  = mergeSort (split.peek(), compare);
	const auto second = mergeSort (split.peek_back(), compare);

	/*
	auto first = std::async(std::launch::async, 
			[&](){return mergeSort(peek(split), compare);});

	auto second = std::async(std::launch::async, 
			[&](){return mergeSort(peek_back(split), compare);});
			*/

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
	
