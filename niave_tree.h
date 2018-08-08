#ifndef NIAVE_TREE_H
#define NIAVE_TREE_H 
#include "utility.h" 
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
class Tree 
{

	using Node = TreeNode<T>;
	using SharedNode = std::shared_ptr<TreeNode<T>>;
	SharedNode head; 



	int readHeight (void) const
	{
		if (head == nullptr)
			return 0; 

		return head->height;
	}

	Tree writeHeight (int h) const
	{
		if (head == nullptr)
			return *this; 

		return Tree( head->res, head->left, head->right, h);
	}

	//returns the height of a tree
	int height (void) const
	{
		if (head == nullptr)
			return 0; 

		return max( Tree(head->left).readHeight(), Tree(head->right).readHeight()) +1;
	}

	Tree minValueNode (void) const
	{
		if (head->left == nullptr)
			return *this;

		return Tree(head->left).minValueNode();
	}


	//returns if the tree is balanced
	int balanced (void) const
	{
		if(head == nullptr)
			return 0;

		const int left = Tree(head->left).height();
		const int right = Tree(head->right).height();

		return left - right;

	}

	Tree rotateLeft (void) const
	{

		using SharedNode = std::shared_ptr<TreeNode<T>>;

		const SharedNode Head = head;
		const SharedNode A = Head->right;
		const SharedNode B = (A == nullptr ? nullptr : A->left);
		const SharedNode C = Head->left;

		const SharedNode newLeft = std::make_shared<TreeNode<T>>(Head->res, C, B);

		const Tree rotated = Tree(A->res, Tree(newLeft).writeHeight( 1 + max( Tree(newLeft->left).readHeight(), Tree(newLeft->right).readHeight())).head ,A->right);

		return rotated.writeHeight( max( Tree(rotated.head->left).readHeight(), Tree(rotated.head->right).readHeight()) + 1);

	};

	Tree rotateRight (void) const
	{
		using SharedNode = std::shared_ptr<TreeNode<T>>;

		const SharedNode Head = head;

		const SharedNode A = Head->left;
		const SharedNode B = (A == nullptr ? nullptr : A->right);
		const SharedNode C = Head->right;

		const SharedNode newRight = std::make_shared<TreeNode<T>>(Head->res, B, C);
		const Tree rotated = Tree(A->res, A->left, Tree(newRight).writeHeight( max( Tree(newRight->left).readHeight(), Tree(newRight->right).readHeight()) + 1).head );

		return rotated.writeHeight( max( Tree(rotated.head->left).readHeight(), Tree(rotated.head->right).readHeight() +1));
	};

	Tree doubleRotateRight (void) const
	{
		return Tree(head->res, Tree(head->left).rotateLeft().head , head->right).rotateRight();

	};

	Tree doubleRotateLeft (void) const
	{

		return Tree(head->res, head->left, Tree(head->right).rotateRight().head).rotateLeft();
	}

	Tree insertBalance (const T key) const
	{

		if (balanced() > 1 && key < head->left->res)
			return rotateRight();

		if (balanced() < -1 && key > head->right->res)
			return rotateLeft();

		if (balanced() > 1)
			return doubleRotateRight();

		if (balanced() < -1)
			return doubleRotateLeft();

		return *this;

	}

	Tree removeBalance (const T key) const
	{
		if(head == nullptr)
		{
			return *this; 
		}

		if (balanced() > 1 && Tree(head->left).balanced() >= 0)
			return rotateRight();

		if (balanced() > 1 && Tree(head->left).balanced() < 0)
			return doubleRotateRight();

		if (balanced() < -1 && Tree(head->right).balanced() <= 0)
			return rotateLeft();

		if (balanced() < -1 && Tree(head->right).balanced() > 0)
			return doubleRotateLeft();


		return *this;
	}

	Tree removeOperation (const T res) const
	{
		if(head == nullptr)
		{
			*this;
		}


		T y = head->res; 

		if(res < y)
		{
			const Tree niaveRemove = Tree(y, Tree(head->left).removeOperation(res).head, head->right);
			const Tree balanced = niaveRemove.removeBalance(res);

			return balanced.writeHeight( 1 + max( Tree(balanced.head->left).readHeight(), Tree(balanced.head->right).readHeight())); 

		}

		if(res > y)
		{
			const Tree niaveRemove = Tree(y, head->left, Tree(head->right).removeOperation(res).head);
			const Tree balanced = niaveRemove.removeBalance(res);

			return balanced.writeHeight( 1 + max( Tree(balanced.head->left).readHeight(), Tree(balanced.head->right).readHeight())); 
		}


		if(head->left == nullptr && head->right == nullptr)
		{
			return Tree(nullptr);
		}

		if( head->left == nullptr || head->right == nullptr)
		{

			const Tree temp = head->left == nullptr ? Tree(head->right) : Tree(head->left);

			return temp.writeHeight( 1 + max( Tree(temp.head->left).readHeight(), Tree(temp.head->right).readHeight()));

		}

		//two child case 
		else 
		{

			const Tree temp = Tree(head->right).minValueNode();
			const Tree remmed = Tree(temp.head->res, head->left, Tree(head->right).remove(temp.head->res).head);

			return remmed.writeHeight( 1 + max( Tree(remmed.head->left).readHeight(),  Tree(remmed.head->right).readHeight()));

		}

	}


public:
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

	Tree (const Tree& a) 
		: head(a.head)
	{}

	Tree (const Tree&& a)
		: head(a.head)
	{}

	Tree& operator = (const Tree& a)
	{
		head = a.head;
		return *this;
	}

	List<T> toList (const List<T> list = List<T>(nullptr)) const
	{
		if (head == nullptr)
			return list;

		//lmao autism
		//make + overloads 
		const auto left  = Tree(head->left).toList(list);
		const auto right = Tree(head->right).toList(list);

		return left + head->res + right;
	}

	//returns the number of elements in a tree
	int size (void) const
	{
		if(head == nullptr)
			return 0;

		return 1 + Tree(head->left).size() + Tree(head->right).size();
	}


	Tree push (const T res) const
	{
		if (head == nullptr)
			return Tree(res);

		T y = head->res; 

		if(res < head->res)
		{
			const Tree niaveInsert = Tree(y, Tree(head->left).push(res).head, head->right);
			const Tree balanced = niaveInsert.insertBalance(res);
			const Tree updatedHeight = balanced.writeHeight( 1 + max( Tree(balanced.head->left).readHeight(), Tree(balanced.head->right).readHeight()));

			return updatedHeight;
		}

		if(res > head->res)
		{

			const Tree niaveInsert = Tree(y, head->left , Tree(head->right).push(res).head);
			const Tree balanced = niaveInsert.insertBalance(res);
			const Tree updatedHeight = balanced.writeHeight( 1 + max( Tree(balanced.head->left).readHeight(), Tree(balanced.head->right).readHeight()));

			return updatedHeight;
		}

		return *this;
	}

	/*
	Tree push (const Tree a) const 
	{
	*/


	Tree remove (const T res) const
	{
		if (!contains(res))
			return *this;

		return removeOperation(res);
	}





	bool contains (const T res) const
	{
		if(head == nullptr)
			return false;

		if(res < head->res)
			return Tree(head->left).contains(res);

		if(res > head->res)
			return Tree(head->right).contains(res);

		return true;
	};

};

/*
//should these be implicit conversions?
//also these are all unbalancing if the list gets sorted
template <typename T, typename F = T(T)>
Tree<T> map (const Tree<T> tree, F fun)
{
	return ListToTree(map(TreeToList(tree), fun));
}

template <typename T, typename F = T(T,T)>
T fold (const Tree<T> tree, F fun, T init)
{
	return ListToTree( fold( TreeToList(tree), fun, init));
}
*/












#endif
