#ifndef LIST_H
#define LIST_H 

#include <memory>
#include <initializer_list>
#include <functional> 
//singly linked list 
//nodes are shared between list tails thanks to being persistent 
//ref counted with shared_ptr 
//ref counted child node 
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
	std::shared_ptr<ListNode<T>> head; 

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
	List push_back (const List<T> b) const //combines two lists together
	{
		if(!length())
			return b; 
		if(!b.length())
		   return *this;

		if(length() < 2)
			return b.push(peek());

		return pop_back().push_back(b.push( peek_back()));
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
	List push (const List<T> b) const//combines two lists together, but B is infront of A
	{
		return b.push_back(*this);
	}

	List reverse (void) const 
	{
		if(length() < 2)
			return *this; 

		return List(peek()).push( pop().reverse());
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
			return pop_back().split(start, end);

		return *this;

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

		return fun( peek_back(), pop_back().foldl( fun, init));
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
		
		return os << l.pop();
	}



#endif
