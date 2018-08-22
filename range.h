#include "list.h" 
#include <optional>
#include <limits>
#include <time.h> 
#include <random>

template <typename T>
class ListStream 
{
	List<T> res; 

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

class Integers 
{
	const int i, last; 

	public:
	using ValueType = int; 

	Integers (std::optional<int> start = std::nullopt, std::optional<int> e = std::nullopt) 
		: i(start.value_or(0))
		, last ( e.value_or( std::numeric_limits<int>::max()))
	{
	}

	int get (void) const 
	{
		return i;
	}

	bool end (void) const 
	{
		return i == last;
	}

	Integers next (void) const 
	{
		if(i > last)
			return Integers( i - 1, last);

		return Integers( i + 1, last);
	}
};

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


struct Take 
{
	const size_t quant; 

	Take (size_t q)
		: quant(q)
	{}

};

template<typename F>
struct Map 
{
	using FunctionType = F;
	const F fun;

	Map (F f)
		: fun(f)
	{}
};
	
struct CollectList{}; 

template<typename Value, typename Stream, typename F>
class MapInstance
{
	Stream stream; 
	F fun;

	public:
	using ValueType = Value; 
	using FunctionType = F; 
	using StreamType = Stream;

	MapInstance( Stream s, Map<F> f)
		: stream(s), fun(f.fun)
	{}

	Value get (void) const 
	{
		return fun( stream.get());
	}

	bool end (void) const 
	{
		return stream.end(); 
	}

	MapInstance next (void) const 
	{
		return MapInstance( stream.next(), fun);
	}

};

template<typename Stream, typename F>
auto operator | (Stream left, const Map<F>& right) -> MapInstance<decltype( right.fun(left.get())), Stream, F>
{
	return MapInstance<decltype( right.fun(left.get())), Stream, F>(left, right);
	//return MapInstance<typename Stream::ValueType, Stream, F>(left, right);
}


template<typename T, typename S>
class TakeInstance
{
	size_t quant;
	S stream; 

	public:
	using ValueType = T;
	T get (void) const
	{
	   	return stream.get();
	}

	bool end (void) const
	{
	   	return stream.end() || !quant;
	}

	TakeInstance next (void) const
	{
		return TakeInstance ( stream.next(), Take(quant - 1));
	}

	TakeInstance (S s, Take t)
		: quant(t.quant), stream(s)
	{}
};

template<typename S>
TakeInstance<typename S::ValueType, S> operator | (S left, const Take& right)
{
	return TakeInstance<typename S::ValueType , S>(left, right);
}

template<typename T, typename S>
class CollectListInstance
{
	List<T> res; 

	List<T> streamBuild (List<T> l , S s)
	{
		if(s.end())
			return l;

		return  streamBuild(l.push(s.get()), s.next());
	}

	public:

	List<T> get (void) const 
	{
		return res.reverse();
	}

	operator List<T> (void) const
	{
		return res.reverse();
	}

	CollectListInstance (S s)
		: res( streamBuild(List<T>(typename List<T>::SharedNode(nullptr)), s))
	{
	}
};

template<typename S>
CollectListInstance<typename S::ValueType, S> operator | (S left, const CollectList& right)
{
	return CollectListInstance<typename S::ValueType, S>(left);
}

template <typename Stream>
struct Zip 
{
	using StreamType = Stream;
	const Stream stream; 

	Zip (Stream s)
		: stream(s)
	{}
};

template <typename InputStream, typename ParamStream, 
		 typename Value = std::tuple< typename InputStream::ValueType, typename ParamStream::ValueType>>
class ZipInstance
{
	InputStream istream; 
	ParamStream pstream;

	public:
	using ValueType = Value; 
	using StreamType = InputStream;

	ZipInstance (InputStream is, ParamStream ps)
		: istream(is), pstream(ps)
	{}

	Value get (void) const 
	{
		return std::make_tuple( istream.get(), pstream.get());
	}

	bool end (void) const
	{
		return istream.end() || pstream.end();
	}

	ZipInstance next (void) const
	{
		return ZipInstance( istream.next(), pstream.next());
	}
};

template <typename InputStream, typename ParamStream>
auto operator | ( InputStream left, const Zip<ParamStream>& right) 
	-> ZipInstance< InputStream, typename Zip<ParamStream>::StreamType>
{
	return ZipInstance<InputStream, typename Zip<ParamStream>::StreamType>( left, right.stream);
}

template <typename F>
struct Filter
{
	using FunctionType = F;
	const F fun; 

	Filter (F f)
		: fun(f)
	{}
};

/*
template <typename Value, typename Stream, typename F>
class FilterInstance 
{
	Stream stream; 
	F fun; 

	public: 
	using ValueType = Value; 
	using FunctionType = F; 
	using StreamType = Stream; 

	FilterInstance ( Stream s, Filter<F> f)
		: stream(s), fun(f.fun)
	{}

	bool inputOver (void)
	{


	bool end (void) const 
	{
		return stream.end(); 

	Value get (void) const 
	{
	*/

		




