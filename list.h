#ifndef LIST_H
#define LIST_H 

#include <memory>
#include <initializer_list>

//singly linked list 
//nodes are shared between list tails thanks to being persistent 
//ref counted with shared_ptr 
//ref counted child node 
template<typename T> 
struct ListNode 
{
	const T res; 
	const std::shared_ptr<ListNode<T>> next; 

	ListNode (const T r, const std::shared_ptr<ListNode<T>> n = nullptr)
		:res(r), next(n)
	{}
};

//just a node and a few constructors 
template <typename T>
struct List 
{
	const std::shared_ptr<ListNode<T>> head; 

	List(T r, std::shared_ptr<ListNode<T>> h = nullptr)
		: head(std::make_shared<ListNode<T>>(r,h))
	{}

	List(std::shared_ptr<ListNode<T>> h)
		: head(h)
	{}

	explicit List (void)
		: head(nullptr)
	{}
};

//0 length for empty lists, 1 is just a node, 2+ has tail 
template<typename T>
int length (const List<T> l)
{
	if(!l.head)
		return 0;

	if(!l.head->next)
		return 1;

	//inline pop 
	return 1 + length<T>(l.head->next);
}

//a->b .. c->a->b 
template <typename T>
List<T> push (const List<T> l, const T res) //pushes an element to the head of a list
{
	if(length(l) == 0)
		return List<T>(res);

	return List<T>(res, l.head);
}

//a->b .. a
template <typename T>
T peek (const List<T> l) //returns first element of List 
{
	if(length(l))
		return l.head->res;

	throw std::out_of_range("peeking a List of size zero");
}

//a->b->c .. b->c
template <typename T>
List<T> pop (const List<T> l) //returns a list where the head is gone 
{
	if(length(l) > 1)
		return List<T>(l.head->next);

	return List<T>(); 
}

//a->b->c .. c
template<typename T>
T peek_back (const List<T> l) //returns last element of a List
{
	if(length(l) > 1)
		return peek_back( pop(l));

	return peek(l); 
}

//a->b->c .. //na->nb 
template<typename T>                //n stands for new node, so in this example we need a new node containing a 
List<T> pop_back (const List<T> l) //returns the List minus the tail element a-b-c .. na-b 
{
	if(length(l) > 1)
	{
		if(length(l) > 2)
			return List<T>( peek(l), pop_back( pop(l)).head);

		return List<T>(peek(l));
	}

	return List<T>(); 
};

//a->b->c , d->e->f ... na->nb->nc->d->e->f 
template<typename T>
List<T> push_back (const List<T> a, const List<T> b) //combines two lists together
{
	if(!length(b))
		return a; 
	if(!length(a))
	   return b;

	if(length(a) < 2)
		return push(b,peek(a));

	return push_back( pop_back(a), push(b, peek_back(a)));
}

//a->b .. na->nb->c
template<typename T>
List<T> push_back (const List<T> l, const T res) //puts an element at the back of a list
{
	if(!length(l))
		return List<T>(res);

	if(length(l) > 1)
		return push(push_back( pop(l), res), peek(l));

	return List<T>(peek(l), std::make_shared<ListNode<T>>(res));
}

//a->b->c, d->e->f ... n(d->e->f->a->b->c)
template<typename T>
List<T> push (const List<T> a, const List<T> b) //combines two lists together, but B is infront of A
{
	return push_back(b,a);
}

//need to unify how we're traversing, because we're pushing_back innificiently to append which is retarded 
//use map for <<
//maybe pop_back is needed there being slow again
template<typename T>
List<T> operator+ (const List<T> a, const T b)
{
	return push_back(a, b);
}

template<typename T>
List<T> operator+ (const T a, const List<T> b)
{
	return push(b, a);
}

template<typename T>
List<T> operator + (const List<T> a, const List<T> b)
{
	return push_back(a, b);
}



//replace with an IO monad or something
template<typename T>
std::ostream& operator<< (std::ostream& os, const List<T>& l) //ostreams the elements, maybe move this OUT of the lib because OS isn't const 
{
	os << peek(l);
	
	if(length(l) > 1)
		return operator<< (os, List<T>(pop(l)));

	return os;
}

template<typename T>
List<T> initBuild (const std::initializer_list<T> init, const int pos = 0) //crappy {} constructor, make it a member with a helper function
{
	if(pos + 1 > init.size())
		return List<T>(nullptr);

	return push_back( List<T>(*(init.begin() + pos)), initBuild(init, pos + 1 ));
}


#endif
