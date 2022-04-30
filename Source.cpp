#include <iostream>
#include <vector>
#include<string>
using namespace std;

template <typename T> 
class Operator
{
public:
	virtual T apply(T leftValue,T rightValue) = 0;
};

template <typename T>
class MinOperator : public Operator<T>
{
public:
	T apply(T leftValue, T rightValue)
	{
		if (leftValue > rightValue)
		{
			return rightValue;
		}
		else
		{
			return leftValue;
		}
	}
};

template <typename T> 
class SegmentTree
{
public:
	SegmentTree(vector<T>& initialValues,Operator<T>* op,T identity);
	T get(int index);
	void set(int index, T value);
	T queryRange(int start, int end);
	void buildTree(int rangeStart, int rangeEnd,int currentNode, vector<T>& initialValues);
	int size();
	~SegmentTree();

private:
	int length;
	int capacity;
	Operator<T>* oper;
	T identity;
	T* tree;
	int getMid(int start, int end)
	{
		return (start + ((end - start)/2));
	}
	
	int leftChild(int node)
	{
		return node + 1;
	}

	int rightChild(int node,int start,int mid)
	{
		int numberOfLeavesInTheLeftSubtree = mid - start + 1;
		int numberOfNodesInTheLeftSubtree = 2 * numberOfLeavesInTheLeftSubtree - 1;
		return node + numberOfNodesInTheLeftSubtree + 1;
	}

	T queryRange(int queryStart,int queryEnd,int rangeStart, int rangeEnd,int currentNode);
	void set(int index,int rangeStart,int rangeEnd,int currentNode,T value);
};

template <typename T>
SegmentTree<T>::SegmentTree(vector<T>& initialValues, Operator<T>* op, T identity)
{
	this->oper = op;
	this->identity = identity;
	this->length = this->capacity = initialValues.size();
	this->tree = new T[(2 * this->capacity) - 1];
	buildTree(0, capacity -1,0,initialValues);
}

template<typename T>
T SegmentTree<T>::get(int index)
{
	if (index < 0 || index >= length) throw ("index out of bounds: " + to_string(index));
	return queryRange(index, index);
}

template<typename T>
void SegmentTree<T>::set(int index, T value)
{
	return set(index, 0, length - 1, 0, value);
}

template<typename T>
void SegmentTree<T>::set(int index, int rangeStart, int rangeEnd, int currentNode, T value)
{
	if (rangeStart == rangeEnd && rangeEnd == index) 
	{
		// the currentNode represents a leaf. and the range contains only the element at the desried index
		tree[currentNode] = value;
		return;
	}
	int mid = getMid(rangeStart, rangeEnd);
	if (index <= mid)
		set(index, rangeStart, mid, leftChild(currentNode), value);
	else
		set(index, mid + 1, rangeEnd, rightChild(currentNode, rangeStart, mid), value);
	// because my children have changed, I gotta update my value as well
	tree[currentNode] = oper->apply(tree[leftChild(currentNode)], tree[rightChild(currentNode, rangeStart, mid)]);
}

template<typename T>
void SegmentTree<T>::buildTree(int rangeStart, int rangeEnd, int currentNode, vector<T>& initialValues)
{
	if (rangeStart == rangeEnd)
	{
		//the current node represents a leaf,
		//which has range of a single element
		tree[currentNode] = initialValues[rangeStart];
		return;
	}
	else
	{
		int mid = getMid(rangeStart,rangeEnd);
		// left subtree
		buildTree(rangeStart,mid,leftChild(currentNode),initialValues);
		// right subtree
		buildTree(mid+1, rangeEnd, rightChild(currentNode, rangeStart,mid), initialValues);
		// combine
		tree[currentNode] = oper->apply(tree[leftChild(currentNode)]
			,tree[rightChild(currentNode,rangeStart,mid)]);
	}
}

template<typename T>
int SegmentTree<T>::size()
{
	return this->length;
}

template <typename T>SegmentTree<T>::~SegmentTree()
{
	delete[] tree;
	tree = nullptr;
}

template<typename T>
T SegmentTree<T>::queryRange(int start, int end)
{
	return queryRange(start,end,0,length-1,0);
}

template<typename T>
T SegmentTree<T>::queryRange(int queryStart, int queryEnd, int rangeStart, int rangeEnd, int currentNode)
{
	if (queryStart <=rangeStart && rangeEnd<=queryEnd)
	{
		// the currentNode represents a range that is fully contained within the query
		// so we need tha value of this node completely
		return tree[currentNode];
	}
	else if (queryStart> rangeEnd || rangeStart > queryEnd)
	{
		//the query has no overlap wiith the range of currentNode
		return this->identity;
	}
	else
	{
		//partial overlap
		int mid = getMid(rangeStart,rangeEnd);
		T leftValue = queryRange(queryStart, queryEnd, rangeStart, mid,leftChild(currentNode));
		T rightValue = queryRange(queryStart, queryEnd, mid+1,rangeEnd,
			rightChild(currentNode,rangeStart,mid));
		return oper->apply(leftValue, rightValue);
	}
}


int main()
{
	MinOperator<int> intMinOp;
	vector<int> arr{1,6,8,9};
	SegmentTree<int>* segTree = new SegmentTree<int>(arr,&intMinOp,INT_MAX);

	cout<<segTree->queryRange(1, 1);
}