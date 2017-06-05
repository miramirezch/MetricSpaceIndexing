#pragma once
//#include"VPT.h"
#include "BKT.h"
#include <iostream>

int main()
{
	std::vector<int> test{ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,101 };
	std::vector<int>cosa(10);

	auto dist = [](const int& p1, const int& p2) {
		return std::abs((p1 - p2));
	};
	
	//VpTree<int> tree;
	BKT<int> tree2;	

	std::vector<int> respuestas;
	std::vector<double> distancias;

	//tree.Build(test, dist);
	tree2.Build(test, dist);
	tree2.KNN(110, 1, respuestas, distancias);

	std::cout << "Hello World" << std::endl;
}