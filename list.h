#ifndef LIST_H
#define LIST_H

#include <memory>

//singly linked list 
//nodes are shared between list tails thanks to being persistent 
//ref counted with shared_ptr 
//ref counted child node 
template<typename T> 
struct ListNode 
{
	const T res; 
	const std::shared_ptr<ListNode<T>> next; 

	ListNode (T r, const std::shared_ptr<ListNode<T>> n = nullptr)
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
	return 1 + length(List<T>(l.head->next));
}

//a->b .. c->a->b 
template <typename T>
List<T> push (const List<T> l, const T res)
{
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

//a->b->c .. b->->c
template <typename T>
List<T> pop (const List<T> l) //a->b .. b-> 
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
template<typename T>
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
List<T> push (const List<T> a, const List<T> b)
{
	if(!length(b))
		return a; 
	if(!length(a))
	   return b;

	if(length(a) < 2)
		return push(b,peek(a));

	return push( pop_back(a), push(b, peek_back(a)));
}

//a->b .. na->nb->c
template<typename T>
List<T> push_back (const List<T> l, const T res)
{
	if(!length(l))
		return List<T>(res);

	if(length(l) > 1)
		return push( peek(l), push_back( pop(l), res));

	return List<T>(peek(l), List<T>(res).head);
}

//a->b->c, d->e->f ... n(d->e->f->a->b->c)
template<typename T>
List<T> push_back (const List<T> a, const List<T> b)
{
	return push(b,a);
}

template<typename T>
std::ostream& operator<< (std::ostream& os, const List<T>& l)
{
	os << peek(l);
	
	if(length(l) > 1)
		return operator<< (os, List<T>(pop(l)));

	return os;
}

#endif
