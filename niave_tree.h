#ifndef NIAVE_TREE_H
#define NIAVE_TREE_H 
#include "utility.h" 

template<typename T>
struct NiaveTreeNode
{
	using SharedNode = std::shared_ptr<NiaveTreeNode<T>>;

	const T res; 
	const SharedNode left; 
	const SharedNode right;

	NiaveTreeNode (T res , SharedNode l = nullptr 
						 ,SharedNode  r = nullptr)
		:res(res), left(l), right(r)
	{}
};

template <typename T>
struct NiaveTree 
{
	using Node = NiaveTreeNode<T>;
	using SharedNode = std::shared_ptr<NiaveTreeNode<T>>;
	const SharedNode head; 

	NiaveTree (SharedNode h)
		:head(h)
	{}

	explicit NiaveTree (T res)
		: head(std::make_shared<Node>(res))
	{}

	explicit NiaveTree (T res, SharedNode left, SharedNode right)
		: head(std::make_shared<Node>(res, left, right))
	{}

	explicit NiaveTree (void)
		: head(nullptr)
	{}
};

//would be prettier if it took a pointer, too bad 
template <typename T>
int size (const NiaveTree<T> tree)
{
	if(tree.head == nullptr)
		return 0;

	return 1 + size(NiaveTree<T>(tree.head->left)) + size(NiaveTree<T>(tree.head->right));
}

template <typename T>
int height (const NiaveTree<T> tree)
{
	if(tree.head == nullptr)
		return 0;

	return  max(1 +  height(NiaveTree<T>(tree.head->left)),  1 + height(NiaveTree<T>(tree.head->right)));
}

template <typename T>
bool balanced (const NiaveTree<T> tree)
{
	if(tree.head == nullptr)
		return true;

	const int left = height(NiaveTree<T>(tree.head->left));
	const int right = height(NiaveTree<T>(tree.head->right));

	if ( max(left, right) - min(left, right) < 2)
		return true;

	return false;
}


template <typename T>
NiaveTree<T> rotateLeft (const NiaveTree<T> tree)
{
	if(height(NiaveTree<T>(tree.head->right)) == 0)
		return tree; 

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
		return tree; 

	using SharedNode = std::shared_ptr<NiaveTreeNode<T>>;

	const SharedNode Head = tree.head;

	const SharedNode A = Head->left;
	const SharedNode B = (A == nullptr ? nullptr : A->right);
	const SharedNode C = Head->right;

	return  NiaveTree<T>( A->res, A->left, std::make_shared<NiaveTreeNode<T>>(Head->res, B, C));

};

template <typename T>
NiaveTree<T> push (const NiaveTree<T> tree, const T res)
{

	if(tree.head == nullptr)
		return NiaveTree<T>(res);

	if(res < tree.head->res)
		return NiaveTree<T>(res, push( NiaveTree<T>(tree.head->left), res).head, tree.head->right);

	if(res > tree.head->res)
		return NiaveTree<T>(res, tree.head->left, push( NiaveTree<T>(tree.head->right), res).head);

	return tree.head;
}

#endif
