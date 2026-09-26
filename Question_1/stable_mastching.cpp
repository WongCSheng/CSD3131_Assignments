//start off by gale shapley algo
#include <iostream>
#include <vector>
#include <string>

struct Disciple {
	int id;
	std::string name;
	std::vector<int> preferred_masters;
};

struct Master {
	int id;
	std::string name;
	std::vector<int> preferred_disciples;
};
