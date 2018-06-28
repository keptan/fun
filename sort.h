#include "list.h" 
#include "string.h" 

template <typename T>
using Ord  = bool(T,T);

template <typename T>
bool ordOverload (const T a, const T b)
{
	return a > b;
}

template <typename T, typename F = Ord<T>>
T max (const T a, const T b, F compare = ordOverload)
{
	if(compare(a,b) == compare(b,a))
		return a; 

	const T max =  compare(a,b) ? a : b;
	return max; 
}

template <typename T, typename F = Ord<T>>
T min (const T a, const T b, F compare = ordOverload)
{
	if(compare(a,b) == compare(b,a))
		return b; 

	const T min =  compare(a,b) ? b : a;
	return min;
}

template <typename T, typename F = Ord<T>>
bool sorted (const List<T> l, F compare = ordOverload)
{
	if(length(l) < 2)
		return true; 

	if(! compare( peek(pop(l)), peek(l)))
		return false; 

	return sorted(pop(l));
}

template <typename T> 
List<List<T>> shuffleSplit (const List<T> l, const List<T> a= List<T>(), const List<T> b= List<T>())
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
List<T> mergeOrdered (const List<T> a, const List<T> b, F compare = ordOverload, const List<T> out = List<T>())
{
	if(length(a) == 0 ||  length(b) == 0)
		return push_back(out, push(a, b));

	const auto c = [&](const auto first, const auto second){ return compare(peek_back(first), peek_back(second));};

	const List<T> larger =  max(a,b,c);
	const List<T> smaller = min(a,b,c);

	return mergeOrdered(pop_back(larger), smaller,  compare, push(out, peek_back(larger)));
}

template <typename T, typename F = Ord<T>>
List<T> mergeSort (const List<T> l, const F compare = ordOverload)
{
	if(sorted(l, compare))
		return l; 

	const auto splits = shuffleSplit(l); 
	const auto first  = mergeSort(peek(splits), compare); 
	const auto second = mergeSort(peek_back(splits), compare); 

	/*
	const auto firstL = [=](const List<T> ad){return mergeSort(ad,compare);};

	auto first = std::async (firstL, peek(splits));
	auto  second = std::async (firstL, peek_back(splits));
	*/

	return mergeOrdered(first,second, compare);
}
	
