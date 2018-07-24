#ifndef NIAVE_TREE_H
#define NIAVE_TREE_H 
#include "utility.h" 
#include "list.h"

enum Color {Red, Black};
//node that holds resource and points to other members
//shared by different trees (persistent tree)
template<typename T>
struct RBTreeNode
{
	using SharedNode = std::shared_ptr<RBTreeNode<T>>;

	Color c;

	const T res; 
	const SharedNode left; 
	const SharedNode right;

	RBTreeNode (Color c, T res , SharedNode l = nullptr 
						 ,SharedNode  r = nullptr)
		:c(c), res(res), left(l), right(r)
	{}
};

//holds a head and some constructors
template <typename T>
struct RBTree 
{
	using Node = RBTreeNode<T>;
	using SharedNode = std::shared_ptr<RBTreeNode<T>>;
	const SharedNode head; 

	RBTree (const SharedNode h)
		:head(h)
	{}

	explicit RBTree (T res, Color c = Black)
		: head(std::make_shared<Node>(c, res))
	{}

	explicit RBTree (T res, SharedNode left, SharedNode right, Color c = Black)
		: head(std::make_shared<Node>(c, res, left, right))
	{}

	explicit RBTree (void)
		: head(nullptr)
	{}
};

//returns the number of elements in a tree
template <typename T>
int size (const RBTree<T> tree)
{
	if(tree.head == nullptr)
		return 0;

	return 1 + size<T>(tree.head->left) + size<T>(tree.head->right);
}

//returns the height of a tree
template <typename T>
int height (const RBTree<T> tree)
{
	if(tree.head == nullptr)
		return 0;

	return  max(1 +  height<T>(tree.head->left),  1 + height<T>(tree.head->right));
}

template <typename T>
int blackHeight (const RBTree<T> tree)
{
	if (tree.head == nullptr)
		return 1; 

	if (tree.head->c == Red)
		return max(  blackHeight<T>(tree.head->left), blackHeight<T>(tree.head->right));

	return max( 1 +  blackHeight<T>(tree.head->left), 1 + blackHeight<T>(tree.head->right));
}

template <typename T>
bool doubleLeft (const RBTree<T> tree)
{
	return tree.head && tree.head->c == Red && height<T>(tree.head->left) && tree.head->left->c == Red; 
}

template <typename T>
bool doubleRight (const RBTree<T> tree)
{
	return height(tree) && tree.head->c == Red && height<T>(tree.head->right) && tree.head->right->c == Red;
}

template <typename T>
RBTree<T> paint (const RBTree<T> tree, Color c)
{
	if(!height(tree))
		return tree; 

	return RBTree<T>(tree.head->res, tree.head->left, tree.head->right, c);
}
	

//returns if the tree is balanced
template <typename T>
bool balanced (const RBTree<T> tree)
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
bool blackBalanced (const RBTree<T> tree)
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
RBTree<T> rotateLeft (const RBTree<T> tree)
{
	if(height(RBTree<T>(tree.head->right)) == 0)
		throw std::out_of_range("left rotating a list with empty right side");

	using SharedNode = std::shared_ptr<RBTreeNode<T>>;

	const SharedNode Head = tree.head;
	const SharedNode A = Head->right;
	const SharedNode B = (A == nullptr ? nullptr : A->left);
	const SharedNode C = Head->left;

	return  RBTree<T>( A->res, std::make_shared<RBTreeNode<T>>(Head->res, C, B), A->right);

};

template <typename T>
RBTree<T> rotateRight (const RBTree<T> tree)
{
	if(height(RBTree<T>(tree.head->left)) == 0)
		throw std::out_of_range("right rotating a list with empty left side");

	using SharedNode = std::shared_ptr<RBTreeNode<T>>;

	const SharedNode Head = tree.head;

	const SharedNode A = Head->left;
	const SharedNode B = (A == nullptr ? nullptr : A->right);
	const SharedNode C = Head->right;

	return  RBTree<T>( A->res, A->left, std::make_shared<RBTreeNode<T>>(Head->res, B, C));

};

template <typename T>
RBTree<T> balance (Color c, T res, RBTree<T> left, RBTree<T> right)
{
	if ( c == Black && doubleLeft(left))
		return RBTree<T>(
				left.head->res, paint<T>(left.head->left, Black).head, RBTree<T>(res, left.head->right, right.head, Black).head 
				,Red);

	if (c == Black && doubleRight(left))
		return RBTree<T>(
				left.head->right->res
				,RBTree<T>(left.head->res, left.head->left, left.head->right->left, Black).head
				,RBTree<T>(res, left.head->right, right.head, Black).head 
				,Red);

	if (c == Black && doubleLeft(right))
		return RBTree<T>(
				right.head->left->res
				,RBTree<T>(res, left.head, right.head->left->left, Black).head
				,RBTree<T>(right.head->res, right.head->left->right, right.head->right, Black).head
				,Red);

	if (c == Black && doubleRight(right))
		return RBTree<T>(
				right.head->res
				,RBTree<T>(res, left.head, right.head->left).head
				,paint<T>(right.head->right, Black).head
				,Red);

	return RBTree<T>(res, left.head, right.head, c);
}

template <typename T>
RBTree<T> invsert (const RBTree<T> tree, const T res)
{
	if(tree.head == nullptr)
		return RBTree<T>(res, Red);

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
RBTree<T> push (const RBTree<T> tree, const T res)
{
	const RBTree<T> n = invsert(tree, res);

	return RBTree<T>(n.head->res, n.head->left, n.head->right, Black);
}

template<typename T>
bool contains (const RBTree<T> tree, const T res)
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
