#pragma once 
#include <tuple> 
#include <type_traits>
#include "generators.h"
#include "collectors.h" 

struct Take 
{
	const size_t quant; 

	Take (size_t q)
		: quant(q)
	{}

};

template<typename T, typename S>
class TakeInstance
{
	const size_t quant;
	const S stream; 

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

	TakeInstance (const S s, const Take t)
		: quant(t.quant), stream(s)
	{}
};

template<typename S>
TakeInstance<typename S::ValueType, S> operator | (S left, const Take& right)
{
	return TakeInstance<typename S::ValueType , S>(left, right);
}

struct Skip 
{
	const size_t quant; 

	Skip (size_t q)
		: quant(q)
	{}
};

template<typename T, typename S>
class SkipInstance 
{

	const S stream; 

	S skip (const S s, const size_t q) const
	{
		if(q == 0 || s.end())
			return s; 

		return skip (s.next(), q - 1);
	}

	public:
	using ValueType = T;
	T get (void) const
	{
	   	return stream.get();
	}

	bool end (void) const
	{
	   	return stream.end();
	}

	SkipInstance  next (void) const
	{
		return SkipInstance( stream.next(), 0);
	}

	SkipInstance  (const S s, const size_t q)
		: stream( skip(s, q))
	{}
};

template<typename S>
SkipInstance<typename S::ValueType, S> operator | (S left, const Skip& right)
{
	return SkipInstance<typename S::ValueType, S>(left, right.quant);
};

template<typename F>
struct Map 
{
	using FunctionType = F;
	const F fun;

	Map (const F f)
		: fun(f)
	{
	}


};

template<typename M>
struct RecursiveMap 
{
	using MapType = M; 

	const M map; 

	RecursiveMap ( const M m)
		: map(m)
	{}
};

template<typename F>
auto map (const RecursiveMap<F> m) -> RecursiveMap< RecursiveMap<F>>
{
	std::cout << "double recursive map being formed" << std::endl;
	return RecursiveMap<RecursiveMap<F>>(m);
}

template<typename F>
auto map (const Map<F> m) -> RecursiveMap<Map<F>>
{
	return RecursiveMap<Map<F>>(m);
}

template<typename F>
auto map (const F m) -> Map<F>
{
	return Map<F>(m);
}



template<typename Value, typename Stream, typename F>
class MapInstance
{
	const Stream stream; 
	const F fun;

	public:
	using ValueType = Value; 
	using FunctionType = F; 
	using StreamType = Stream;

	MapInstance( const Stream s, const Map<F> f)
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


/*
template<typename Stream, typename F>
auto operator | (Stream left, const Map<F>& right) -> MapInstance<decltype( right.fun(left.get())), Stream, F>
{
	return MapInstance<decltype( right.fun(left.get())), Stream, F>(left, right);
	//return MapInstance<typename Stream::ValueType, Stream, F>(left, right);
}
*/



template<typename Container, typename Stream, typename MInstance>
class RecursiveMapInstance
{
	const Stream stream; 
	const MInstance _map; 

	public: 
	using ValueType = Container;
	using FunctionType = MInstance;
	using StreamType = Stream; 

	RecursiveMapInstance ( const Stream s, const MInstance m)
		: stream(s), _map(m)
	{}

	Container get (void) const 
	{
		return Range(stream.get()) | _map | Collect<Container>();
	}

	bool end (void) const 
	{
		return stream.end();
	}

	RecursiveMapInstance next (void) const 
	{
		return RecursiveMapInstance ( stream.next(), _map);
	}
};

template< typename Stream, typename M>
auto operator | (Stream left, const RecursiveMap<M>& right) 
-> RecursiveMapInstance< typename Stream::ValueType, Stream, M>
{
	return RecursiveMapInstance< typename Stream::ValueType, Stream, M>(left, right.map);
};

template<typename Stream, typename F>
auto operator | (Stream left, const Map<F>& right) -> MapInstance<decltype( right.fun(left.get())), Stream, F>
{
	return MapInstance<decltype( right.fun(left.get())), Stream, F>(left, right);
	//return MapInstance<typename Stream::ValueType, Stream, F>(left, right);
}

/*
template<typename Stream, typename F >
auto operator >> (Stream left, const Map<F>& right) ->  
		typename std::enable_if< 
								std::is_base_of<MapTag ,F>::value , 
								RecursiveMapInstance< typename Stream::ValueType, Stream, F> 
								void>::type
{
	//return RecursiveMapInstance<typename Stream::ValueType, Stream, F>(left, right.fun);
	std::cout << "recursive map instance" << std::endl;
}

template<typename Stream, typename F>
auto operator >> (Stream left, const Map<F>& right) ->
			typename std::enable_if< 
						( std::is_invocable< F, typename Stream::ValueType>::value && 
						(!std::is_base_of<MapTag, F>::value)) , 
						 MapInstance< decltype(right.fun( left.get())), Stream, F> / 
						void>::type
{
	std::cout << "non recursive map instance " << std::endl;

//	return MapInstance<decltype( right.fun(left.get())), Stream, F>(left, right);
}
*/

template <typename Stream>
struct Zip 
{
	using StreamType = Stream;
	const Stream stream; 

	Zip (const Stream s)
		: stream(s)
	{}
};

template <typename InputStream, typename ParamStream, 
		 typename Value = std::tuple< typename InputStream::ValueType, typename ParamStream::ValueType>>
class ZipInstance
{
	const InputStream istream; 
	const ParamStream pstream;

	public:
	using ValueType = Value; 
	using StreamType = InputStream;

	ZipInstance (const InputStream is, const ParamStream ps)
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

template <typename Stream>
struct ZipApply
{
	using StreamType = Stream;
	const Stream stream; 

	ZipApply (const Stream s)
		: stream(s)
	{}
};

template <typename InputStream, typename ParamStream 
	, typename Value>
class ZipApplyInstance
{
	const InputStream istream; 
	const ParamStream pstream;

	public:
	using ValueType = Value; 
	using StreamType = InputStream;

	ZipApplyInstance (const InputStream is, const ParamStream ps)
		: istream(is), pstream(ps)
	{}

	Value get (void) const 
	{
		return pstream.get()(istream.get());
	}

	bool end (void) const
	{
		return pstream.end() || istream.end();
	}

	ZipApplyInstance next (void) const
	{
		return ZipApplyInstance(istream.next(), pstream.next());
	}
};

template <typename InputStream, typename ParamStream>
auto operator | (const InputStream left, const ZipApply<ParamStream>& right)
	-> ZipApplyInstance< InputStream, typename ZipApply<ParamStream>::StreamType, 
		decltype( right.stream.get()(left.get()))>
{
	return ZipApplyInstance< InputStream, typename ZipApply<ParamStream>::StreamType, 
		decltype( right.stream.get()(left.get()))>(left, right.stream);
}


template <typename Stream>
struct Product 
{
	using StreamType = Stream;
	const Stream stream; 

	Product (const Stream s)
		: stream(s)
	{}
};

template <typename InputStream, typename ParamStream 
	, typename Value = std::tuple< typename InputStream::ValueType, typename ParamStream::ValueType>>
class ProductInstance 
{
	const InputStream istream; 
	const ParamStream pstream;
	const InputStream start; 

	public:
	using ValueType = Value; 
	using StreamType = InputStream;

	ProductInstance (const InputStream is, const ParamStream ps)
		: istream(is), pstream(ps), start(is)
	{}

	ProductInstance (const InputStream is, const ParamStream ps, const InputStream s)
		: istream(is), pstream(ps), start(s)
	{}

	Value get (void) const 
	{
		return std::make_tuple( istream.get(), pstream.get());
	}

	bool end (void) const 
	{
		return pstream.end();
	}

	ProductInstance next (void) const 
	{
		if(istream.end())
			return  ProductInstance( start, pstream.next(), start);

		return ProductInstance( istream.next(), pstream, start);
	}
};

template <typename InputStream, typename ParamStream>
auto operator | (const InputStream left, const Product<ParamStream>& right)
	-> ProductInstance< InputStream, typename Product<ParamStream>::StreamType>
{
	return ProductInstance(left, right.stream);
}

template <typename F>
struct Filter 
{
	using FunctionType = F;
	const F fun; 

	Filter (const F f)
		: fun(f)
	{}
};

template <typename Value, typename Stream, typename F>
class FilterInstance 
{
	const F fun;
	const Stream stream; 

	Stream nextStream (const Stream s) const 
	{
		if(s.end()) return s;
		if(fun(s.get())) return s;

		return nextStream(s.next());
	}

	public:
	using ValueType = Value; 
	using FunctionType = F;
	using StreamType = Stream; 

	FilterInstance (const Stream s, const F f)
		: fun(f), stream( nextStream(s))
	{}


	Value get (void) const 
	{
		return stream.get();
	}

	bool end (void) const 
	{
		return stream.end();
	}

	FilterInstance next (void) const 
	{
		return FilterInstance( stream.next(), fun);
	}
};

template< typename Stream, typename F>
auto operator | (Stream left, const Filter<F>& right) -> FilterInstance<typename Stream::ValueType, Stream, F>
{
	return FilterInstance<typename Stream::ValueType, Stream, F>(left, right.fun);
}

template< typename F, typename V>
struct Fold 
{
	using FunctionType = F;
	using ValueType = V;
	const F fun; 
	const V val;

	Fold (const F f, const V v)
		: fun(f), val(v)
	{}
};

template <typename Value, typename Stream, typename F>
class FoldInstance 
{
	const Stream stream; 
	const F fun; 
	const bool over; 
	const Value init;

	Value streamEater ( const Stream s, const Value v) const
	{
		if(s.end()) return v;
		return streamEater(s.next(), fun(s.get(), v));
	}


	public:
	using ValueType = Value; 
	using FunctionType = F; 
	using StreamType = Stream; 

	FoldInstance ( const Stream s, const Fold<F, Value> f)
		: stream(s), fun(f.fun), init(f.val), over(false)
	{}

	FoldInstance ( const Stream s, F f, Value v, bool o)
		: stream(s), fun(f), init(v), over(o)
	{}

	Value get (void) const
	{
		return streamEater (stream, init);
	}

	Value eval (void) const 
	{
		return streamEater (stream, init);
	}

	bool end (void) const 
	{
		return over || stream.end();
	}

	FoldInstance next (void) const 
	{
		return FoldInstance( stream, fun, init,  true);
	}
};

template< typename Stream, typename F, typename V>
auto operator | (Stream left, const Fold<F,V >& right) -> FoldInstance<V, Stream, F>
{
	return FoldInstance< V , Stream, F> ( left, right);
}


struct Length 
{};

template <typename Stream>
class LengthInstance 
{
	const Stream stream; 
	//some kind of static memoization would be nice desu 
	
	int streamEater (const Stream s, const int i = 0) const 
	{
		if(s.end()) return i; 
		return streamEater(s.next(), i + 1);
	}

	public: 
	using StreamType = Stream;

	LengthInstance (const Stream s)
		: stream(s)
	{};

	
	int get (void) const 
	{
		return streamEater(stream); 
	}

	int eval (void) const 
	{
		return get(); 
	}

	bool end (void) const 
	{
		return false; 
	}

	LengthInstance next (void) const 
	{
		return *this; 
	}
};

template <typename Stream> 
auto operator | (Stream left, const Length& right) -> LengthInstance<Stream> 
{
	return LengthInstance<Stream>(left);
}

struct Unique 
{};

template <typename Value, typename Stream>
class UniqueInstance 
{

	const Tree<Value> set; 
	const Stream stream; 

	Stream nextStream (const Stream s) const 
	{
		if(s.end()) return s;
		if(!set.contains(s.get())) return s; 

		return nextStream(s.next()); 
	}

	public:
	using ValueType = Value; 
	using StreamType = Stream; 

	UniqueInstance (const Stream s, const Tree<Value> t = Tree<Value>())
		: set(t), stream( nextStream(s)) 
	{}

	Value get (void) const 
	{
		return stream.get();
	}

	bool end (void) const 
	{
		return stream.end(); 
	}

	UniqueInstance next (void) const 
	{
		return UniqueInstance( stream.next(), set.push( stream.get()));
	}
};

template <typename Stream>
auto operator | (Stream left, const Unique& right) -> UniqueInstance<typename Stream::ValueType, Stream>
{
	return UniqueInstance<typename Stream::ValueType, Stream>(left);
}

template <typename Stream>
class FlattenInstance 
{
	const Stream stream; 
	const int skipCount;

	Stream skipBlank (const Stream s)
	{
		if(s.end()) return s;

		if (s.get().end())
			return s.next();

		return s;
	}

	public:
	using ValueType = typename Stream::ValueType::ValueType; 
	using StreamType = Stream;


	FlattenInstance( const Stream s, const int skip = 0)
		: stream( skipBlank(s)), skipCount(skip)
	{}

	ValueType get (void) const
	{
		return (stream.get() | Skip(skipCount)).get();
	}

	bool end (void) const
	{
		return stream.end();
	}

	FlattenInstance next (void) const
	{

		if(skipCount + 1>= (stream.get() | Length()).eval()) return FlattenInstance(stream.next(),0);

		return FlattenInstance(stream, skipCount + 1);
	}	
};

struct Flatten 
{
	Flatten (void)
	{}
};

template<typename S>
FlattenInstance<S> operator | (S left, const Flatten& right)
{
	return FlattenInstance<S>(left);
}
