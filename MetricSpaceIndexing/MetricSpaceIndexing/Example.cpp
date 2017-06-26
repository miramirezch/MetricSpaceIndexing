#pragma once
#include "VPT.h"
#include "BKT.h"
#include "RevLC.h"
#include <iostream>

int main()
{
	std::vector<int> test{ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,110};
	std::vector<int>cosa(10);

	auto dist = [](const int& p1, const int& p2) {
		return std::abs((p1 - p2));
	};

	auto DiscreteDist = [](const int& p1, const int& p2) {
		return static_cast<unsigned>(std::abs((p1 - p2)));
	};
	
	VpTree<int> vpt;
	vpt.Build(test, dist);
	
	BKTree<int> bkt;			
	bkt.Build(test, DiscreteDist);
	
	RevLC<int> lc;
	lc.Build(test, dist, 4);


	std::vector<int> respuestas1;
	std::vector<double> distancias1;
	std::vector<int> respuestas2;
	std::vector<int> distancias2;
	std::vector<int> respuestas3;
	std::vector<double> distancias3;
	
	vpt.KNN(111, 1, respuestas1, distancias1);
	bkt.KNN(110, 1, respuestas2, distancias2);
	lc.KNN(110, 1, respuestas3, distancias3);

	std::cout << "Hello World" << std::endl;
}