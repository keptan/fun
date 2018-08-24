#pragma once 
#include "generators.h" 
#include "collectors.h" 
#include "transformers.h"



/*
template <typename F>
struct Filter
{
	using FunctionType = F;
	const F fun; 

	Filter (F f)
		: fun(f)
	{}
};

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
/*

template <typename Stream>
struct SuperZip 
{
	using StreamType = Stream;
	const Stream stream; 

	Zip (Stream s)
		: stream(s)
	{}
};

template <typename InputStream, typename ParamStream, 
		 typename Value = std::tuple< typename InputStream::ValueType, typename ParamStream::ValueType>>
class SuperZipInstance
{
	InputStream istream; 
	ParamStream pstream;

	InputStream firstistream; 

	public:
	using ValueType = Value; 
	using StreamType = InputStream;

	SuperZipInstance (InputStream is, ParamStream ps, InputStream isf = is)
		: istream(is), pstream(ps), firstistream(isf)
	{}

	Value get (void) const 
	{
		return std::make_tuple( istream.get(), pstream.get());
	}

	bool end (void) const
	{
		return istream.end() && pstream.end();
	}

	SuperZipInstance next (void) const
	{
		return SuperZipInstance( istream.next(), pstream.next());

		if(istream.end())
			return SuperZipInstance( firstistream, pstream.next(), firstistream);

		


	}
};

template <typename InputStream, typename ParamStream>
auto operator | ( InputStream left, const Zip<ParamStream>& right) 
	-> ZipInstance< InputStream, typename Zip<ParamStream>::StreamType>
{
	return ZipInstance<InputStream, typename Zip<ParamStream>::StreamType>( left, right.stream);
}
*/



