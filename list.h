#ifndef LIST_H
#define LIST_H 

#include <memory>
#include <initializer_list>
#include <functional> 
#include <iostream> 
#include <tuple> 
#include "utility.h" 
//singly linked list 
//nodes are shared between list tails thanks to being persistent 
//ref counted with shared_ptr 
//ref counted child node 
//
//
template<typename T> 
struct ListNode 
{
	const T res; 
	const std::shared_ptr<ListNode<T>> next; 
	const int length;

	ListNode (const T r, const std::shared_ptr<ListNode<T>> n = nullptr)
		:res(r), next(n), length(n == nullptr? 1 : 1 + n->length)
	{}
};

//just a node and a few constructors 
template <typename T>
struct List 
{
	using DataType = T;
	std::shared_ptr<ListNode<T>> head; 

	List reverseBuilder (const List l = List(nullptr)) const 
	{
		if(length() == 0)
				return l; 

		if(length() == 1)
			return l.push(peek());

		const List out = l.push(peek()); 

		return pop().reverseBuilder(out);
	}

	struct Iterator 
	{
		const List data;
		const List pos;

		Iterator (const List l)
			: data(l)	
		{}

		T operator* (void) const
		{
			return data.peek();
		}

		Iterator operator ++ (int) const
		{
			return Iterator(data.pop());
		}

		bool operator == (Iterator a) const
		{
			return data == a.data;
		}

		explicit operator bool (void) const
		{
			return data.length();
		}
	};


public:


	List(T r, std::shared_ptr<ListNode<T>> h = nullptr)
		: head(std::make_shared<ListNode<T>>(r,h))
	{}

	List(std::shared_ptr<ListNode<T>> h)
		: head(h)
	{}

	explicit List (void)
		: head(nullptr)
	{}

	List (const List& a)
		: head(a.head)
	{}


	List& operator = (const List& a)
	{
		head = a.head;
		return *this;
	}

	Iterator begin (void)
	{
		return Iterator(*this);
	}

	Iterator rbegin (void)
	{
		return Iterator(reverse());
	}

	bool operator == (const List& a) const
	{
		if(length() == 0 && a.length() == 0)
			return true; 

		if(length() != a.length())
			return false; 

		if (peek() != a.peek())
			return false;


		return pop() == a.pop();
	}

	bool operator != (const List& a) const
	{
		return !( *this == a);
	}

	//0 length for empty lists, 1 is just a node, 2+ has tail 
	int length (void) const
	{
		if(!head)
			return 0;

		return head->length;

		/*
		if(!head->next)
			return 1;

		//inline pop 
		return 1 + List(head->next).length();
		*/
	}

	//a->b .. c->a->b 
	List push ( const T res) const//pushes an element to the head of a list
	{
		if(length() == 0)
			return List(res);

		return List(res, head);
	}

	//a->b .. a
	T peek (void) const//returns first element of List 
	{
		if(length())
			return head->res;

		throw std::out_of_range("peeking a List of size zero");
	}

	//a->b->c .. b->c
	List pop (void) const //returns a list where the head is gone 
	{
		if(length() > 1)
			return List(head->next);

		return List(); 
	}

	//a->b->c .. c
	T peek_back (void) const //returns last element of a List
	{
		if(length() > 1)
			return pop().peek_back();

		return peek();
	}

	//a->b->c .. //na->nb 
	List pop_back (void) const //returns the List minus the tail element a-b-c .. na-b 
	{
		if(length() > 1)
		{
			if(length() > 2)
				return List( peek(), pop().pop_back().head);

			return List(peek());
		}

		return List();
	};


	//a->b->c , d->e->f ... na->nb->nc->d->e->f 
	List push (const List b) const //combines two lists together
	{
		if(!length())
			return b; 
		if(!b.length())
		   return *this;

		if(length() < 2)
			return b.push(peek());

		return b.foldr( [](const auto a, const auto list){ return a + list;}, *this);

		//return pop_back().push_back(b.push( peek_back()));
	}

	List push_back (const T res) const //puts an element at the back of a list
	{
		if(!length())
			return List(res);

		if(length() > 1)
			return pop().push_back(res).push(peek());

		return List<T>( peek(), std::make_shared<ListNode<T>>(res));
	}

	//a->b->c, d->e->f ... n(d->e->f->a->b->c)
	List push_back (const List<T> b) const//combines two lists together, but B is infront of A
	{
		return b.push(*this);
	}

	//variadic zip method 
	//
	
	List reverse (void) const 
	{
		if(length() < 2)
			return *this; 

		return reverseBuilder();
	}

	List splitBuilder (const List in , const int end, const List out = List(nullptr)) const 
	{
		if(end == 0)
			return out.push(in.peek()); 


		return splitBuilder( in.pop(), end - 1, out.push(in.peek()));
	}
		
		

	List split (int start , int end = length() + 1) const
	{
		if(start < 0 || end +1 > length())
			throw std::out_of_range("trying to split out of range");

		if(start > end)
			throw std::out_of_range("start of split is after end");
			
		if(start > 0)
			return pop().split( start - 1, end -1);

		if(end - start < length()- 1)
			return splitBuilder(*this, end).reverse();

		return *this;
	}

	List find (T res) const
	{
		if (length() == 0)
			return *this;

		if (peek() == res)
		{
			return *this;
		}

		return  pop().find(res);
	}

	List find (const List<T> l) const
	{
		if (l.length() == 0)
			return List(nullptr); 

		if (length() == 0)
			return *this;

		if (peek() == l.peek())
		{
			if (split(0, l.length() -1) == l)
				return *this;
		}

		return pop().find(l);
	}



	template <typename F = T(T)>
	List map (const F fun) const
	{
		if (length() == 0)
			return List(nullptr);

		if (length() == 1)
			return List( fun( peek()));

		return fun(peek()) + pop().map(fun);
	}

	template < typename G, typename F = std::function<G(T,G)>>
	G foldr ( F fun, G init) const
	{
		if (length() == 0)
			return init;

		return fun( peek(), pop().foldr(fun, init));

	}

	template <typename G, typename F = std::function<G(T,G)>>
	G foldl (F fun, G init) const
	{
		if (length() == 0)
			return init;

		return reverse().foldr( fun, init);

		//return fun( peek_back(), pop_back().foldl( fun, init));
	}

	template <typename F = std::function<bool(T)>>
	List filter (F fun) const
	{
		if (length() == 0)
			return *this;

		if (fun(peek()))
			return peek() + pop().filter(fun);

		return pop().filter(fun);
	}

	T operator [] (int i) const
	{
		if (i + 1> length())
			throw std::out_of_range("[] trying to access an out of range element");

		if(i == 0)
			return peek(); 

		return pop()[i - 1];
	}


	//need to unify how we're traversing, because we're pushing_back innificiently to append which is retarded 
	//use map for <<
	//maybe pop_back is needed there being slow again
	/*
	template<typename T>
	List<T> initBuild (const std::initializer_list<T> init, const int pos = 0) //crappy {} constructor, make it a member with a helper function
	{
		if(pos + 1 > init.size())
			return List<T>(nullptr);

		return push_back( List<T>(*(init.begin() + pos)), initBuild(init, pos + 1 ));
	}
	*/


};

template <typename T>
List<T> mergeSort (const List<T> l, const Ord<T> compare = ordOverload);


	template<typename T>
	List<T> operator+ (const List<T> a,  const T b)
	{
		return a.push_back(b);
	}

	template<typename T>
	List<T> operator+ (const T a, const List<T> b)
	{
		return b.push(a);
	}

	template<typename T>
	List<T> operator + (const List<T>a, const List<T> b)
	{
		return a.push_back(b);
	}


	//replace with an IO monad or something
	template<typename T>
	std::ostream& operator<< (std::ostream& os, const List<T>& l) //ostreams the elements, maybe move this OUT of the lib because OS isn't const 
	{
		if(!l.length())
			return os;

		os << l.peek();
		
		return os << ',' << l.pop();
	}

	template<typename T>
	std::ostream& operator<< (std::ostream& os, const List<List<T>>& l) //ostreams the elements, maybe move this OUT of the lib because OS isn't const 
	{

		if(!l.length())
			return os;

		os << '[' << l.peek() << ']';
		
		return os << l.pop();
	}




	
	template<typename T>
	List< List<T>> zip2 (const List<T> a, const List<T> b)
	{

		const auto doubleZip = [](const T input, const std::tuple< List<List<T>>, List<List<T>>> tuple)
		{
			const auto first = std::get<0>(tuple).pop(); 
			const auto second = std::get<1>(tuple).push( std::get<0>(tuple).peek().push(input));


			return std::make_tuple(first, second);
		};

		//const List< List<T>> out = a.foldr( [](const T in, const List< List<T>> out){ return out.push( List<T>(in));}, List< List<T>>(nullptr));
		const auto larger = a.length() > b.length() ? a : b;
		const auto smaller = a.length() > b.length() ? b : a;

		const auto initTupleList = larger.foldr( [](const T in, const auto out){ return out.push( List<T>(in));}, List<List<T>>(nullptr));
		const auto finalTuple = smaller.foldl( doubleZip, std::make_tuple(initTupleList, List<List<T>>(nullptr)));

		return std::get<1>(finalTuple) + std::get<0>(finalTuple);
	}

	template<typename T>
	List<List<T>> vSortInput (const List<List<T>> out, const List<T> first)
	{
		return mergeSort<List<T>>(out.push_back(first), [](List<T>  a, List<T>  b){ return a.length() < b.length();});
	}


	template <typename T, typename... A>
	List<List<T>> vSortInput (const List<List<T>> out, const List<T> first, A... rest)
	{
		return vSortInput(out.push_back(first), rest...);
	}

	template<typename T>
	List<List<T>> zipAdd (const List<T> input, const List<List<T>> output)
	{

		const auto tupleZip = [](const T input, const std::tuple< List<List<T>>, List<List<T>>> tuple)
		{
			const auto first = std::get<0>(tuple).pop(); 
			const auto second = std::get<1>(tuple).push( std::get<0>(tuple).peek().push(input));

			return std::make_tuple(first, second);
		};

		const auto finalTuple = input.foldl( tupleZip, std::make_tuple(output, List<List<T>>(nullptr)));
		return (std::get<0>(finalTuple).reverse() + std::get<1>(finalTuple)).reverse();
		//return (std::get<1>(finalTuple)) + std::get<0>(finalTuple)).reverse();
	}

		
	template<typename T, typename... A>
	List<List<T>> superZip (const List<T> first, A... rest)
	{
	
		const auto sortedInput = vSortInput( List<List<T>>(nullptr), first, rest...);

		const auto initTupleList = sortedInput.peek().foldr( [](const T in, const auto out){ return out.push( List<T>(in));}, List<List<T>>(nullptr));

		return  sortedInput.pop().foldr(zipAdd<T>, initTupleList);
	}


#endif

