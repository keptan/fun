#pragma once 
#include "list.h" 
#include "string.h"
#include "tree.h"
#include <iterator>
#include <optional>
#include <random>
#include <limits>
#include <stdio.h>
#include <iostream>
#include <concepts>


template <typename T>
concept IsFunStream = requires(T s)
{
	{std::as_const(s).get()}  -> std::same_as<typename T::ValueType>;
	{std::as_const(s).end()}  -> std::same_as<bool>;
	{std::as_const(s).next()} -> std::same_as<T>;
};

template <typename T>
class FunStream
{
	public: 

	auto get (void) const -> auto
	{
		return static_cast<const T*>(this)->get_();
	}

	auto end (void) const -> bool 
	{
		return static_cast<const T*>(this)->end_();
	}

	auto next (void) const -> T
	{
		return static_cast<const T*>(this)->next_();
	}
};

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

//VERY non const 
ListStream<String> FileLineStream (const String name)
{
	List<String> acc; 

	FILE *input = fopen(name.string().c_str(), "r");

	if(!input)
	{
		return acc; 
	}

	char*  line = nullptr;
	size_t len = 0;

	while((getline(&line, &len, input)) != -1)
		acc = acc.push(line);

	fclose(input);
	if(line) free(line);

	return acc;
}
	

template <typename IT, typename T = IT>
class IteratorStream : public FunStream<IteratorStream<IT, T>>
{
	const IT first, last;

	public: 

	T get_ (void) const 
	{
		return first; 
	}

	bool end_ (void) const 
	{
		return first >= last; 
	}

	IteratorStream next_ (void) const 
	{
		return IteratorStream( first + 1, last);
	}

	using ValueType = T; 

	IteratorStream (const IT b, const IT e)
		:first(b), last(e)
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

struct LazyFileStream
{
	using ValueType = char; 
	struct CharList
	{
		const char res;
		mutable std::shared_ptr<CharList> tail;
		mutable std::shared_ptr<CharList> head;

		CharList (const char c, const std::shared_ptr<CharList> t)
			: res(c), tail(t)
		{}

		void push (const char c) const
		{
			if(!tail)
			{
				tail = std::make_shared<CharList>(c, nullptr);
				return; 
			}

			auto i = tail;
			while(i->tail) i = i->tail;

			i->tail = std::make_shared<CharList>(c, nullptr);
			return;
		}
	};

	const std::shared_ptr<CharList> res; 
	FILE* const file;

	LazyFileStream (const std::shared_ptr<CharList> c, FILE* f)
		: res(c), file(f)
	{}

	char get (void) const
	{
		return res->res;
	}

	bool end (void) const
	{
		return res->res == EOF;
	}

	LazyFileStream next (void) const
	{
		if (end()) return LazyFileStream(res, file);
		if (res->tail) return LazyFileStream(res->tail, file);


		const char c = fgetc(file);
		if(c == EOF) fclose(file);

		res->push(c);
		return LazyFileStream(res->tail, file);
	}

};

LazyFileStream fileStream (const char* name)
{
		FILE* const file = fopen(name, "r");
		if(!file) return LazyFileStream( std::make_shared<LazyFileStream::CharList>(EOF, nullptr), file);
		return LazyFileStream( std::make_shared<LazyFileStream::CharList>(fgetc(file), nullptr), file);
}


LazyFileStream fileStream (FILE* f)
{
		if(!f) return LazyFileStream( std::make_shared<LazyFileStream::CharList>(EOF, nullptr), f);
		return LazyFileStream( std::make_shared<LazyFileStream::CharList>(fgetc(f), nullptr), f);
}


