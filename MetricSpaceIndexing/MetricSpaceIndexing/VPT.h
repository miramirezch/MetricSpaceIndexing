#pragma once
#include "HeapItem.h"
#include <vector>
#include <functional>
#include <cmath>
#include <random>
#include <algorithm>
#include <queue>
#include <stack>
// Miguel Ramirez Chacon
// 28/05/17
// Array based Vantage Point Tree
// Based on https://github.com/gregorburger/vp-tree (Pointer based)
// T: Point class(2D)


template<typename T>
struct VPTNode
{
	T Data;
	double Threshold;
};



template<typename T>
class VpTree
{
private:
	std::vector<VPTNode<T>> tree_;
	std::vector<bool> flags_;
	std::function<double(const T&, const T&)> distance;

public:

	VpTree() {}

	void Build(std::vector<T>& data, std::function<double(const T&, const T&)> dist)
	{
		distance = dist;
		auto power = static_cast<unsigned>(std::floor(std::log2(data.size()) + 1));
		auto size = 1 << power;

		std::vector<VPTNode<T>> temp(size - 1);
		tree_ = std::move(temp);

		flags_.reserve(size - 1);

		for (auto i = 0; i < (size - 1); i++)
		{
			flags_.push_back(false);
		}

		std::random_device rd;
		std::mt19937 gen(0);
		std::uniform_real_distribution<> dis(0, 1);

		Build(data, 1, dis, gen);
	}

	void Build(std::vector<T>& data, int position, std::uniform_real_distribution<> dis, std::mt19937 gen)
	{
		if (data.size() == 1)
		{
			VPTNode<T> node;
			node.Threshold = 0;
			node.Data = data[0];

			tree_[position - 1] = node;
			flags_[position - 1] = true;
			return;
		}

		if (data.size() == 0)
			return;

		int i = static_cast<int>(dis(gen)*(data.size() - 1));
		std::swap(data[data.size() - 1], data[i]);
		auto pivot = data[data.size() - 1];
		data.pop_back();

		auto d = distance;

		int median = data.size() / 2;

		std::nth_element(
			std::begin(data),
			std::begin(data) + median,
			std::end(data),
			[pivot, d](const T& a, const T& b)
		{
			return d(pivot, a) < d(pivot, b);
		});

		VPTNode<T> node;

		node.Threshold = distance(pivot, data[median]);
		node.Data = pivot;

		tree_[position - 1] = node;
		flags_[position - 1] = true;

		std::vector<T> leftSubTreeData(std::begin(data), std::begin(data) + median);
		std::vector<T> rightSubTreeData(std::begin(data) + median, std::end(data));

		Build(leftSubTreeData, 2 * position, dis, gen);
		Build(rightSubTreeData, 2 * position + 1, dis, gen);
	}

	void KNN(const T& target, const unsigned k, std::vector<T>& results, std::vector<double>& distances) const
	{
		std::priority_queue<HeapItem<T>> heap;

		double _tau = std::numeric_limits<double>::max();
		Search(1, target, k, heap, _tau);

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

	void Search(unsigned position, const T& target, const size_t k,
		std::priority_queue<HeapItem<T>>& heap, double& _tau) const
	{
		std::stack<unsigned> snapshotStack;
		snapshotStack.push(position);

		while (snapshotStack.size() != 0)
		{
			auto currentPosition = snapshotStack.top();
			snapshotStack.pop();

			if (currentPosition > tree_.size() || flags_[currentPosition - 1] == false)
			{
				continue;
			}

			double dist = distance(tree_[currentPosition - 1].Data, target);

			if (dist < _tau)
			{
				if (heap.size() == k)
					heap.pop();

				heap.push(HeapItem<T>(tree_[currentPosition - 1].Data, dist));

				if (heap.size() == k)
					_tau = heap.top().Distance;
			}

			double dm = tree_[currentPosition - 1].Threshold;

			if (dist < dm)
			{
				if (dist - _tau <= dm)
				{
					snapshotStack.push(2 * currentPosition);
				}

				if (dist + _tau >= dm)
				{
					snapshotStack.push(2 * currentPosition + 1);
				}

			}
			else
			{
				if (dist + _tau >= dm)
				{
					snapshotStack.push(2 * currentPosition + 1);
				}

				if (dist - _tau <= dm)
				{
					snapshotStack.push(2 * currentPosition);
				}
			}
		}
	}

};
