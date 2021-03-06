#pragma once
#include <utility>
#include <memory>
#include <vector>
#include <unordered_map>
#include <random>
#include <functional>
#include <stack>
#include <algorithm>
#include <queue>

// Miguel Ramirez Chacon
// 25/06/17
// Burkhard Keller Tree - No recursion
// Paper: Some approaches to best-match file searching
// Author: W.Burkhard and R.Keller.

template<typename T>
class BKTree
{
private:
	
	struct Node
	{
		Node() {}
		Node(T data) : Data{ data } {}

		T Data;
		std::vector<std::pair<int, Node>> children;		
	};

	struct HeapItem
	{
		HeapItem(T data, int distance) :Data{ data }, Distance{ distance } {}
		T Data;
		int Distance;

		bool operator<(const HeapItem& o) const
		{
			return Distance < o.Distance;
		}
	};

	std::unique_ptr<Node> root;
	std::function<int(const T&, const T&)> distance;

public:
	BKTree() {}	

	void Add(const T& data)
	{
		if (root == nullptr)
		{
			root = std::make_unique<Node>(data);
		}
		else
		{			
			Add(Node(data));
		}
	}	

	void Add(Node& data)
	{
		std::stack <Node*> nodeStack;
		
		nodeStack.push(root.get());
		auto flag = false;

		Node* currentNode;
		int d;

		while (nodeStack.size() != 0)
		{
			currentNode = nodeStack.top();
			nodeStack.pop();

			d = distance(data.Data, currentNode->Data);
			flag = false;

			for (auto& child : currentNode->children)
			{
				if (d == child.first)
				{
					nodeStack.push(&(child.second));
					flag = true;
					break;
				}
			}

			if (flag == false)
			{
				currentNode->children.push_back(std::make_pair(d, Node(data)));
			}
			
		}
	}

	void Build(const std::vector<T>& db, std::function<unsigned(const T&, const T&)> dist)
	{
		distance = dist;

		auto data = db;
		while (data.size()>0)
		{
			std::mt19937 gen(0);
			std::uniform_int_distribution<> dis(0, data.size()-1);
			std::swap(data[data.size() - 1], data[dis(gen)]);			
			auto pivot = data[data.size() - 1];
			data.pop_back();
			Add(pivot);
		}

		SortChildren(*root);
	}

	void SortChildren(Node& node)
	{
		std::stack<Node*> nodeStack;
		nodeStack.push(&node);

		Node* currentNode;

		while (nodeStack.size() != 0)
		{
			currentNode = nodeStack.top();
			nodeStack.pop();

			if (currentNode->children.size() > 0)
			{
				std::sort(std::begin(currentNode->children),std::end(currentNode->children),
					[](const auto& pair1, const auto& pair2) {
					return pair1.first < pair2.first;
				});
			}

			for (auto& child : currentNode->children)
			{
				if (child.second.children.size() > 0)
				{
					nodeStack.push(&(child.second));
				}
			}
		}

	}
	
	void KNN(const T& target, const int k, std::vector<T>& results, std::vector<int>& distances) const
	{
		std::priority_queue<HeapItem> heap;

		int _tau = std::numeric_limits<int>::max();
		Search(*root, target, k, heap, _tau);

		results.clear();
		distances.clear();

		while (!heap.empty())
		{
			results.push_back(heap.top().Data);
			distances.push_back(heap.top().Distance);
			heap.pop();
		}

		std::reverse(std::begin(results), std::end(results));
		std::reverse(std::begin(distances), std::end(distances));
	}

	
	void Search(Node& node, const T& target, const int k,
		std::priority_queue<HeapItem>& heap, int& _tau) const
	{
		std::stack<Node*> snapshotStack;
		snapshotStack.push(&node);

		Node* currentNode;
		int dist;
		
		while (snapshotStack.size() != 0)
		{
			currentNode = snapshotStack.top();
			snapshotStack.pop();

			dist = distance(currentNode->Data, target);
			
			if (dist <= _tau)
			{
				if (heap.size() == k)
					heap.pop();

				heap.push(HeapItem(currentNode->Data, dist));

				if (heap.size() == k)
					_tau = heap.top().Distance;
			}

			for (int d = std::max(0,dist - _tau); d<= dist + _tau; d++)
			{				
				auto it = std::lower_bound(std::begin(currentNode->children), std::end(currentNode->children), std::make_pair(d, Node(0)),
					[](const auto& pair1, const auto& pair2) {return pair1.first < pair2.first; });
								
				if (it != std::end(currentNode->children) && (it->first == d))
				{
					snapshotStack.push(&(currentNode->children[std::distance(std::begin(currentNode->children), it)]).second);
				}
			}		
		}
	}
};