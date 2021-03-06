#pragma once
#include "list.h"

template <typename S>
S evalHelper (const S stream)
{
	if(stream.end()) return stream;

	stream.get();
	return evalHelper( stream.next());
}


template<typename S>
void eval (S stream)
{
	static_assert( 
			std::is_same<
					void, typename S::ValueType>::value, 
			"requires stream with void value type");

	evalHelper( stream);
}

	


struct CollectList{}; 

template<typename T, typename S>
class CollectListInstance
{
	const List<T> res; 

	List<T> streamBuild (const List<T> l , const S s)
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

	CollectListInstance (const S s)
		: res( streamBuild(List<T>(typename List<T>::SharedNode(nullptr)), s))
	{
	}
};


template<typename S>
List<typename S::ValueType> operator | (S left, const CollectList& right)
{
	return CollectListInstance<typename S::ValueType, S>(left);
}

//SFINAE to detect if we're collecting a list or what 
template <typename T, 
		  typename = std::enable_if< 
		std::is_same<
					typename List<T>::DataType, T
					>::value
									>
		>
CollectList Collect (void) 
{
	return CollectList();
}


