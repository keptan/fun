#ifndef UTILITY_H 
#define UTILITY_H


template <typename A, typename B = A>
using Ord  = bool(A,B);

template <typename A, typename B>
bool ordOverload (const A a, const B b)
{
	return a > b;
}

template <typename T, typename F = Ord<T>>
T max (const T a, const T b, const F compare = ordOverload)
{
	if(compare(a,b) == compare(b,a))
		return a; 

	const T max =  compare(a,b) ? a : b;
	return max; 
}

template <typename T, typename F = Ord<T>>
T min (const T a, const T b, const F compare = ordOverload)
{
	if(compare(a,b) == compare(b,a))
		return b; 

	const T min =  compare(a,b) ? b : a;
	return min;
}

#endif
