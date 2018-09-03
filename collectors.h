#pragma once
#include "list.h"

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


