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
struct CollectList{}; 


template<typename T, typename S>
struct TakeInstance
{
	size_t quant;
	S stream; 
	using ValueType = T;


	T get (void) const
	{
	   	return stream.get();
	}

	bool end (void) const
	{
	   	return stream.end() || !quant;
	}

	TakeInstance next (void) 
	{
		quant--;
	   	stream = stream.next();
		return *this;
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
struct CollectListInstance
{
	List<T> res; 
	S stream; 

	operator List<T> (void) 
	{
		return res.reverse();
	}

	CollectListInstance (S s)
		: res(nullptr), stream(s)
	{
		while(!stream.end())
		{
			res = res.push(stream.get());
			stream = stream.next();
		}
	
	}
};


template<typename S>
CollectListInstance<typename S::ValueType, S> operator | (S left, const CollectList& right)
{
	return CollectListInstance<typename S::ValueType, S>(left);
}




	
