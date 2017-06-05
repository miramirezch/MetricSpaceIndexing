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

template<typename T>
struct Node
{
	Node() {}
	Node(T data) : Data{ data } {}

	T Data;
	std::unordered_map<unsigned, Node<T>> children;
};

template<typename T>
struct HeapItem
{
	HeapItem(T data, double distance) :Data{ data }, Distance{ distance } {}
	T Data;
	double Distance;

	bool operator<(const HeapItem& o) const
	{
		return Distance < o.Distance;
	}
};


template<typename T>
class BKT
{
private:
	std::unique_ptr<Node<T>> root;
	std::function<unsigned(const T&, const T&)> distance;

public:
	BKT(std::function<unsigned(const T&, const T&)> dist) :distance{ dist } {}

	void Add(const T& data)
	{
		if (root == nullptr)
		{
			root = std::make_unique<Node<T>>(data);
		}
		else
		{
			Add(Node<T>(data), *root);
		}
	}	

	void Add(Node<T>& data, Node<T>& node)
	{
		std::stack < Node<T>*> nodeStack;
		nodeStack.push(&node);

		while (nodeStack.size() != 0)
		{
			auto currentPair = nodeStack.top();
			nodeStack.pop();

			auto d = distance(data.Data, currentPair->Data);

			for (auto& child : currentPair->children)
			{
				if (d == child.first)
				{
					nodeStack.push(&(child.second));
					continue;
				}
			}
			currentPair->children.insert({ d, data });
		}
	}

	void Build(const std::vector<T>& Data)
	{
		auto data = Data;
		while (data.size()>0)
		{
			std::mt19937 gen(0);
			std::uniform_int_distribution<> dis(0, data.size()-1);
			int i = dis(gen);
			std::swap(data[data.size() - 1], data[i]);
			auto pivot = data[data.size() - 1];
			data.pop_back();

			Add(pivot);
		}				
	}

	void KNN(T target, unsigned k, std::vector<T>& results, std::vector<double>& distances) const
	{
		std::priority_queue<HeapItem<T>> heap;

		double _tau = std::numeric_limits<unsigned>::max();
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

	void Search(Node<T>& node, T& target, unsigned k,
		std::priority_queue<HeapItem<T>>& heap, double& _tau) const
	{
		std::stack<Node<T>*> snapshotStack;
		snapshotStack.push(&node);

		while (snapshotStack.size() != 0)
		{
			auto currentNode = snapshotStack.top();
			snapshotStack.pop();

			auto dist = distance(currentNode->Data, target);
			
			if (dist <= _tau)
			{
				if (heap.size() == k)
					heap.pop();

				heap.push(HeapItem<T>(currentNode->Data, dist));

				if (heap.size() == k)
					_tau = heap.top().Distance;
			}

			for (int d = dist - _tau; d<= dist + _tau; d++)
			{
				if (currentNode->children.find(d) != currentNode->children.end())
				{
					snapshotStack.push(&(currentNode->children[d]));
				}
			}			
		}
	}
};