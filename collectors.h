#pragma once
#include "list.h"

struct CollectList{}; 

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


