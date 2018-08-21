#include "list.h" 
#include <optional>
#include <limits>

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

	List<T> list (void) const 
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
struct Fold 
{
	using StreamType = Stream;
	const Stream stream; 

	Fold (Stream s)
		: stream(s)
	{}
};

template <typename InputStream, typename ParamStream, 
		 typename Value = std::tuple< typename InputStream::ValueType, typename ParamStream::ValueType>>
class FoldInstance
{
	InputStream istream; 
	ParamStream pstream;

	public:
	using ValueType = Value; 
	using StreamType = InputStream;

	FoldInstance (InputStream is, ParamStream ps)
		: istream(is), pstream(ps)
	{}

	Value get (void) const 
	{
		return std::make_tuple( istream.get(), pstream.get());
	}

	bool end (void)
	{
		return istream.end() || pstream.end();
	}

	FoldInstance next (void) const
	{
		return FoldInstance( istream.next(), pstream.next());
	}
};

template <typename InputStream, typename ParamStream>
auto operator | ( InputStream left, const Fold<ParamStream>& right) 
	-> FoldInstance< InputStream, typename Fold<ParamStream>::StreamType>
{
	return FoldInstance<InputStream, typename Fold<ParamStream>::StreamType>( left, right.stream);
}









	
