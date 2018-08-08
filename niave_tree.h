#ifndef NIAVE_TREE_H
#define NIAVE_TREE_H 
#include "utility.h" 
#include <iostream>
#include <cassert>
#include "list.h"

//node that holds resource and points to other members
//shared by different trees (persistent tree)
template<typename T>
struct TreeNode
{
	using SharedNode = std::shared_ptr<TreeNode<T>>;

	const T res; 
	const SharedNode left; 
	const SharedNode right;
	const int height;

	TreeNode (T res , SharedNode l = nullptr 
						 ,SharedNode  r = nullptr, int h = 1)
		:res(res), left(l), right(r), height(h)
	{}
};

//holds a head and some constructors
template <typename T>
struct Tree 
{
	using Node = TreeNode<T>;
	using SharedNode = std::shared_ptr<TreeNode<T>>;
	const SharedNode head; 

	Tree (const SharedNode h)
		:head(h)
	{}

	explicit Tree (T res)
		: head(std::make_shared<Node>(res))
	{}

	explicit Tree (T res, SharedNode left, SharedNode right, int h = 1)
		: head(std::make_shared<Node>(res, left, right, h))
	{}

	explicit Tree (void)
		: head(nullptr)
	{}
};

//returns the number of elements in a tree
template <typename T>
int size (const Tree<T> tree)
{
	if(tree.head == nullptr)
		return 0;

	return 1 + size<T>(tree.head->left) + size<T>(tree.head->right);
}

template <typename T>
int readHeight (const Tree<T> tree)
{
	if (tree.head == nullptr)
		return 0; 

	return tree.head->height;
}

template <typename T>
Tree<T> writeHeight (const Tree<T> tree, int h)
{
	if (tree.head == nullptr)
		return tree; 

	return Tree<T>( tree.head->res, tree.head->left, tree.head->right, h);
}


//returns the height of a tree
template <typename T>
int height (const Tree<T> tree)
{
	if (tree.head == nullptr)
		return 0; 

	return max(readHeight<T>(tree.head->left), readHeight<T>(tree.head->right)) + 1;
}

template <typename T>
Tree<T> minValueNode (const Tree<T> tree)
{
	if (tree.head->left == nullptr)
		return tree;

	return minValueNode<T>(tree.head->left);
}


//returns if the tree is balanced
template <typename T>
int balanced (const Tree<T> tree)
{
	if(tree.head == nullptr)
		return 0;

	assert(height<T>(tree.head->left) == readHeight<T>(tree.head->left));
	assert(height<T>(tree.head->right) == readHeight<T>(tree.head->right));
	const int left = height<T>(tree.head->left);
	const int right = height<T>(tree.head->right);

	return left - right;

}


template <typename T>
Tree<T> rotateLeft (const Tree<T> tree)
{
	if(height(Tree<T>(tree.head->right)) == 0)
		throw std::out_of_range("left rotating a list with empty right side");

	using SharedNode = std::shared_ptr<TreeNode<T>>;

	const SharedNode Head = tree.head;
	const SharedNode A = Head->right;
	const SharedNode B = (A == nullptr ? nullptr : A->left);
	const SharedNode C = Head->left;

	const SharedNode newLeft = std::make_shared<TreeNode<T>>(Head->res, C, B);

	const Tree<T> rotated = Tree<T>( A->res, writeHeight<T>( newLeft, max(readHeight<T>(newLeft->left), readHeight<T>(newLeft->right)) +1).head,  A->right);
	
	return writeHeight<T>(rotated, max( readHeight<T>(rotated.head->left), readHeight<T>(rotated.head->right)) +1);

};

template <typename T>
Tree<T> rotateRight (const Tree<T> tree)
{
	assert(height(Tree<T>(tree.head->left)) == readHeight<T>(tree.head->left));
	if(height(Tree<T>(tree.head->left)) == 0)
		throw std::out_of_range("right rotating a list with empty left side");

	using SharedNode = std::shared_ptr<TreeNode<T>>;

	const SharedNode Head = tree.head;

	const SharedNode A = Head->left;
	const SharedNode B = (A == nullptr ? nullptr : A->right);
	const SharedNode C = Head->right;

	const SharedNode newRight = std::make_shared<TreeNode<T>>(Head->res, B, C);
	const Tree<T> rotated = Tree<T>( A->res, A->left, writeHeight<T>( newRight, max( readHeight<T>(newRight->left), readHeight<T>(newRight->right)) + 1).head);

	return writeHeight<T>(rotated, max( readHeight<T>(rotated.head->left), readHeight<T>(rotated.head->right)) +1);


};

template <typename T>
Tree<T> doubleRotateRight (const Tree<T> tree)
{
	std::cout << "double right rotate\n";

	return rotateRight<T>( Tree<T>(tree.head->res, rotateLeft<T>(tree.head->left).head , tree.head->right));
};

template <typename T>
Tree<T> doubleRotateLeft (const Tree<T> tree)
{

	std::cout << "double left rotate\n";

	return rotateLeft<T> ( Tree<T>(tree.head->res, tree.head->left, rotateRight<T>(tree.head->right).head));
}



template <typename T>
Tree<T> insertBalance (const Tree<T> tree, const T key)
{

	if (balanced(tree) > 1 && key < tree.head->left->res)
		return rotateRight(tree);

	if (balanced(tree) < -1 && key > tree.head->right->res)
		return rotateLeft(tree);

	if (balanced(tree) > 1)
		return doubleRotateRight(tree);

	if (balanced(tree) < -1)
		return doubleRotateLeft(tree);

	return tree;
}

template<typename T>
Tree<T> removeBalance (const Tree<T> tree, const T key)
{
	if(tree.head == nullptr)
		return tree; 

	if (balanced<T>(tree) > 1 && balanced<T>(tree.head->left) >= 0)
		return rotateRight(tree);

	if(balanced<T>(tree) > 1 && balanced<T>(tree.head->left) < 0)
		return doubleRotateRight(tree);

	if(balanced<T>(tree) < -1 && balanced<T>(tree.head->right) <= 0)
		return rotateLeft(tree);

	if(balanced<T>(tree) < -1 && balanced<T>(tree.head->right) > 0)
		return doubleRotateLeft(tree);

	return tree;
}


template <typename T>
Tree<T> push (const Tree<T> tree, const T res)
{
	if(tree.head == nullptr)
		return Tree<T>(res);

	T y = tree.head->res; 

	if(res < tree.head->res)
	{
		const Tree<T> niaveInsert = Tree<T>(y, push<T>(tree.head->left, res).head, tree.head->right);
		const Tree<T> balanced = insertBalance(niaveInsert, res);
		const Tree<T> updatedHeight = writeHeight(balanced, 1 + max( readHeight<T>( balanced.head->left), readHeight<T>(balanced.head->right)));

		return updatedHeight;
	}

	if(res > tree.head->res)
	{
		const Tree<T> niaveInsert = Tree<T>(y, tree.head->left, push<T>(tree.head->right, res).head);
		const Tree<T> balanced = insertBalance(niaveInsert, res);
		const Tree<T> updatedHeight = writeHeight(balanced, 1 + max(readHeight<T>(balanced.head->left), readHeight<T>(balanced.head->right)));

		return updatedHeight;
	}

	return tree.head;
}

template <typename T>
Tree<T> remove (const Tree<T> tree, const T res)
{
	if(tree.head == nullptr)
		return tree; 

	T y = tree.head->res; 

	if(res < y)
		return removeBalance( Tree<T>(y, remove<T>(tree.head->left, res).head, tree.head->right), res);

	if(res > y)
		return removeBalance( Tree<T>(y, tree.head->left, remove<T>(tree.head->left, res).head), res);

	//one child case
	if( tree.head->left == nullptr || tree.head->right == nullptr)
	{
		const Tree<T> temp = tree.head->left ? tree.head->left : tree.head->right;

		return  writeHeight(temp, 1 + max( readHeight<T>( temp.head->left), readHeight<T>(temp.head->right)));

	}

	//two child case 
	else 
	{
		const Tree<T> temp = minValueNode<T>(tree.head->right);
		const  Tree<T> remmed  = (temp.head->res, tree.head->left, remove<T>(temp, temp.head->res).head);


		return  writeHeight(remmed, 1 + max( readHeight<T>( remmed.head->left), readHeight<T>(remmed.head->right)));
	}

}


template<typename T>
bool contains (const Tree<T> tree, const T res)
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
