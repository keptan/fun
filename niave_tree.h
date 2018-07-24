#ifndef NIAVE_TREE_H
#define NIAVE_TREE_H 
#include "utility.h" 
#include "list.h"

enum Color {Red, Black};
//node that holds resource and points to other members
//shared by different trees (persistent tree)
template<typename T>
struct NiaveTreeNode
{
	using SharedNode = std::shared_ptr<NiaveTreeNode<T>>;

	Color c;

	const T res; 
	const SharedNode left; 
	const SharedNode right;

	NiaveTreeNode (Color c, T res , SharedNode l = nullptr 
						 ,SharedNode  r = nullptr)
		:c(c), res(res), left(l), right(r)
	{}
};

//holds a head and some constructors
template <typename T>
struct NiaveTree 
{
	using Node = NiaveTreeNode<T>;
	using SharedNode = std::shared_ptr<NiaveTreeNode<T>>;
	const SharedNode head; 

	NiaveTree (const SharedNode h)
		:head(h)
	{}

	explicit NiaveTree (T res, Color c = Black)
		: head(std::make_shared<Node>(c, res))
	{}

	explicit NiaveTree (T res, SharedNode left, SharedNode right, Color c = Black)
		: head(std::make_shared<Node>(c, res, left, right))
	{}

	explicit NiaveTree (void)
		: head(nullptr)
	{}
};

//returns the number of elements in a tree
template <typename T>
int size (const NiaveTree<T> tree)
{
	if(tree.head == nullptr)
		return 0;

	return 1 + size<T>(tree.head->left) + size<T>(tree.head->right);
}

//returns the height of a tree
template <typename T>
int height (const NiaveTree<T> tree)
{
	if(tree.head == nullptr)
		return 0;

	return  max(1 +  height<T>(tree.head->left),  1 + height<T>(tree.head->right));
}

template <typename T>
int blackHeight (const NiaveTree<T> tree)
{
	if (tree.head == nullptr)
		return 1; 

	if (tree.head->c == Red)
		return max(  blackHeight<T>(tree.head->left), blackHeight<T>(tree.head->right));

	return max( 1 +  blackHeight<T>(tree.head->left), 1 + blackHeight<T>(tree.head->right));
}

template <typename T>
bool doubleLeft (const NiaveTree<T> tree)
{
	return tree.head && tree.head->c == Red && height<T>(tree.head->left) && tree.head->left->c == Red; 
}

template <typename T>
bool doubleRight (const NiaveTree<T> tree)
{
	return height(tree) && tree.head->c == Red && height<T>(tree.head->right) && tree.head->right->c == Red;
}

template <typename T>
NiaveTree<T> paint (const NiaveTree<T> tree, Color c)
{
	if(!height(tree))
		return tree; 

	return NiaveTree<T>(tree.head->res, tree.head->left, tree.head->right, c);
}
	

//returns if the tree is balanced
template <typename T>
bool balanced (const NiaveTree<T> tree)
{
	if(tree.head == nullptr)
		return true;

	const int left = height<T>(tree.head->left);
	const int right = height<T>(tree.head->right);

	if ( max(left, right) - min(left, right) < 2)
		return true;

	return false;
}

/*
//returns if the tree is black balanced 
template <typename T> 
bool blackBalanced (const NiaveTree<T> tree)
{
	if(tree.head == nullptr)
		return true; 

	const int left = blackHeight<T>(tree.head->left);
	const int right = blackHeight<T>(tree.head->right);

	if (left == right)
		return blackBalanced(

	return false;
*/


template <typename T>
NiaveTree<T> rotateLeft (const NiaveTree<T> tree)
{
	if(height(NiaveTree<T>(tree.head->right)) == 0)
		throw std::out_of_range("left rotating a list with empty right side");

	using SharedNode = std::shared_ptr<NiaveTreeNode<T>>;

	const SharedNode Head = tree.head;
	const SharedNode A = Head->right;
	const SharedNode B = (A == nullptr ? nullptr : A->left);
	const SharedNode C = Head->left;

	return  NiaveTree<T>( A->res, std::make_shared<NiaveTreeNode<T>>(Head->res, C, B), A->right);

};

template <typename T>
NiaveTree<T> rotateRight (const NiaveTree<T> tree)
{
	if(height(NiaveTree<T>(tree.head->left)) == 0)
		throw std::out_of_range("right rotating a list with empty left side");

	using SharedNode = std::shared_ptr<NiaveTreeNode<T>>;

	const SharedNode Head = tree.head;

	const SharedNode A = Head->left;
	const SharedNode B = (A == nullptr ? nullptr : A->right);
	const SharedNode C = Head->right;

	return  NiaveTree<T>( A->res, A->left, std::make_shared<NiaveTreeNode<T>>(Head->res, B, C));

};

template <typename T>
NiaveTree<T> balance (Color c, T res, NiaveTree<T> left, NiaveTree<T> right)
{
	if ( c == Black && doubleLeft(left))
		return NiaveTree<T>(
				left.head->res, paint<T>(left.head->left, Black).head, NiaveTree<T>(res, left.head->right, right.head, Black).head 
				,Red);

	if (c == Black && doubleRight(left))
		return NiaveTree<T>(
				left.head->right->res
				,NiaveTree<T>(left.head->res, left.head->left, left.head->right->left, Black).head
				,NiaveTree<T>(res, left.head->right, right.head, Black).head 
				,Red);

	if (c == Black && doubleLeft(right))
		return NiaveTree<T>(
				right.head->left->res
				,NiaveTree<T>(res, left.head, right.head->left->left, Black).head
				,NiaveTree<T>(right.head->res, right.head->left->right, right.head->right, Black).head
				,Red);

	if (c == Black && doubleRight(right))
		return NiaveTree<T>(
				right.head->res
				,NiaveTree<T>(res, left.head, right.head->left).head
				,paint<T>(right.head->right, Black).head
				,Red);

	return NiaveTree<T>(res, left.head, right.head, c);
}

template <typename T>
NiaveTree<T> invsert (const NiaveTree<T> tree, const T res)
{
	if(tree.head == nullptr)
		return NiaveTree<T>(res, Red);

	T y = tree.head->res; 
	Color c = tree.head->c;

	if(res < tree.head->res)
		return balance<T>(c, y, invsert<T>(tree.head->left, res).head, tree.head->right);

	if(res > tree.head->res)
		return balance<T>(c, y, tree.head->left, invsert<T>(tree.head->right, res).head);

	return tree.head;
}

//use ORD? 
template <typename T>
NiaveTree<T> push (const NiaveTree<T> tree, const T res)
{
	const NiaveTree<T> n = invsert(tree, res);

	return NiaveTree<T>(n.head->res, n.head->left, n.head->right, Black);
}

template<typename T>
bool contains (const NiaveTree<T> tree, const T res)
{
	if(tree.head == nullptr)
		return false;

	if(res < tree.head->res)
		return contains<T>(tree.head->left, res);

	if(res > tree.head->res)
		return contains<T>(tree.head->right, res);

	return true;
};



#endif
