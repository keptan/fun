#include "list.h" 
#include <optional>

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
MapInstance<typename Stream::ValueType, Stream, F> operator | (Stream left, const Map<F>& right)
{
	return MapInstance<typename Stream::ValueType, Stream, F>(left, right);
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

	operator List<T> (void) const
	{
		return res.reverse();
	}

	CollectListInstance (S s)
		: res( streamBuild(List<T>(nullptr), s))
	{
	}
};

template<typename S>
CollectListInstance<typename S::ValueType, S> operator | (S left, const CollectList& right)
{
	return CollectListInstance<typename S::ValueType, S>(left);
}




	