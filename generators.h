#pragma once 
#include "list.h" 
#include <iterator>
#include <optional>
#include <random>
#include <limits>

template <typename T>
class ListStream 
{
	const List<T> res; 

	public:
	using ValueType = T;
	T get (void) const
	{ 
		return res.peek();
	}

	bool end (void) const
	{
		return !res.length();
	}

	ListStream<T>  next (void) const
	{
		return ListStream<T>(res.pop());
	}

	ListStream (List<T> l)
		:res(l)
	{}
};

template <typename IT, typename T = IT>
class IteratorStream 
{
	const IT begin, end_;

	public:
	using ValueType = T; 

	T get (void) const 
	{
		return begin; 
	}

	bool end (void) const 
	{
		return begin == end_; 
	}

	IteratorStream next (void) const 
	{
		return IteratorStream( begin + 1, end_);
	}

	IteratorStream (const IT b, const IT e)
		:begin(b), end_(e)
	{}

};



class Integers 
{
	const int i, last; 
	const bool ended;

	public:
	using ValueType = int; 

	Integers (std::optional<int> start = std::nullopt, std::optional<int> e = std::nullopt, bool re = false) 
		: i(start.value_or(0))
		, last ( e.value_or( std::numeric_limits<int>::max()))
		, ended(re)
	{
	}

	int get (void) const 
	{
		return i;
	}

	bool end (void) const 
	{
		return ended;
	}

	Integers next (void) const 
	{
		const bool repEnded = (i == last);

		if(i > last)
			return Integers( i - 1, last , repEnded);

		return Integers( i + 1, last , repEnded);
	}



	friend bool operator == (const Integers &a, const Integers &b)
	{
		return (a.i == b.i && a.last == b.last);
	}

	//range is entirely above this range 
	friend bool operator > (const Integers &a, const Integers &b)
	{
		return (a.i > b.i && a.last > b.last);
	}

	//range is entirely below another range 
	friend bool operator < (const Integers &a, const Integers &b)
	{
		return (a.i < b.i && a.last < b.last);
	}

	//integer is out the top, or outside the bottom of a range
	friend bool operator > (const int a, const Integers &b)
	{
		return (a > b.last && a > b.i);
	}

	friend bool operator > (const Integers &a, const int b)
	{
		return (a.last > b && a.i > b);
	}

	friend bool operator < (const int a, const Integers &b)
	{
		return (a < b.i && a < b.last);
	}

	friend bool operator < (const Integers &a, const int b)
	{
		return (a.i < b && a.last < b);
	}


};

/*comparison operators, can be used to decide if a range contains another range,
 * is larger than a range, or is entirely below a range of integers */ 



class UniformDist 
{
	const int lowest, highest; 

	public: 
	using ValueType = int; 

	UniformDist (int l, int h)
		: lowest(l), highest(h)
	{}

	int get (void) const
	{
		//cant get around the non const api here, it's either hide the mutation here or somewhere else 
		//cant think of a way rn to wrap this const 
		//cant just return it as an expression because of binding temporaries to references 
		 
		std::random_device r;

		std::mt19937 g ( r());
		return std::uniform_int_distribution<int>(lowest, highest)(g);
	}

	bool end (void) const 
	{
		return false; 
	}

	UniformDist next (void) const 
	{
		return *this;
	}
};

template <typename T> 
auto Range ( const List<T> l) -> ListStream<T>
{
	return ListStream<T>(l);
}

template <typename T, typename IT = typename T::const_iterator>
auto Range (const T container) -> IteratorStream<IT> 
{
	return IteratorStream<IT>(container.begin(), container.end());
}

