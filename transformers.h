#pragma once 
#include <tuple> 
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

template<typename F>
struct Map 
{
	using FunctionType = F;
	const F fun;

	Map (const F f)
		: fun(f)
	{}
};

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

template<typename Stream, typename F>
auto operator | (Stream left, const Map<F>& right) -> MapInstance<decltype( right.fun(left.get())), Stream, F>
{
	return MapInstance<decltype( right.fun(left.get())), Stream, F>(left, right);
	//return MapInstance<typename Stream::ValueType, Stream, F>(left, right);
}

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






