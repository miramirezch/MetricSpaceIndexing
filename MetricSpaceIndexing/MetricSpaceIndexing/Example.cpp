#pragma once
#include"VPT.h"
#include <iostream>

using namespace metricindexes;

double dist2(const int& p1, const int& p2)
{
	return std::abs((p1 - p2));
}

int main()
{
	std::vector<int> test{ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,101 };
	std::vector<int>cosa(10);

	auto dist = [](const int& p1, const int& p2) {
		return std::abs((p1 - p2));
	};

	VpTree<int> tree;

	std::vector<int> respuestas;
	std::vector<double> distancias;

	tree.Build(test, dist);
	tree.KNN(110, 1, respuestas, distancias);

	std::cout << "Hello World" << std::endl;
}