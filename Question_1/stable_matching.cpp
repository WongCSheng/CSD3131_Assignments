//start off by gale shapley algo
#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <algorithm>
#include <cstdlib>
#include <random>

//references:
//https://en.cppreference.com/cpp/string/byte/tolower
//https://medium.com/@lsc830621/algorithm-stable-matching-problem-implementation-using-c-b40c6c6fc71a
//https://en.cppreference.com/cpp/numeric/random/uniform_int_distribution

#define MIN_NUM 1
#define MAX_NUM 5

//Maybe the disciples' criteria is the type of craft the master has to offer
struct Disciple {
	int id;
	bool assigned;
	int assigned_id;
	std::vector<int> preferred_masters;
};

//The masters' criteria is how talented the disciple is? or how much the student is willing to commit (number of hours per day?)
struct Master {
	int id;
	bool assigned;
	int assigned_id;
	std::vector<int> preferred_disciples;
};

//The program will first start off by asking if the user is a master or disciple
//Then, based on whether they are master / disciple, they will be asked on their own details
//Like masters will be required to input the type of craft they are, based on a list of choice?
//The disciples will be asked to input how much they are willing to commit? (like the numbers of hours per day they can commit)
//Then, a list will be given where the master/disciple rank based on their preference
//Afterwards, the result will appear to say which master/disciple they are assigned to

std::vector<Master> masters;
std::vector<Disciple> disciples;

int generate_rand() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(MIN_NUM, MAX_NUM);
	return dist(gen);
}


//To randomly generate a list of masters and disciples
void initialization() {
	int num_masters = generate_rand();
	int num_disciple = generate_rand();
	for (int i = 0; i < num_masters; i++) {
		Master tmp;
		masters.push_back(tmp);
		masters[i].id = i;
		masters[i].assigned = false;
		masters[i].assigned_id = -1;
		for (int j = 0; j < num_disciple; j++) {
			masters[i].preferred_disciples.push_back(j);
		}
		std::random_shuffle(masters[i].preferred_disciples.begin(), masters[i].preferred_disciples.end());

		
	}

	for (int i = 0; i < num_disciple; i++) {
		Disciple tmp;
		disciples.push_back(tmp);
		disciples[i].id = i;
		disciples[i].assigned = false;
		disciples[i].assigned_id = -1;
		for (int j = 0; j < num_masters; j++) {
			disciples[i].preferred_masters.push_back(j);
		}
		std::random_shuffle(disciples[i].preferred_masters.begin(), disciples[i].preferred_masters.end());


	}

}

//
//void prompting() {
//	std::string first_input;
//	std::cout << "Welcome! Are you a Master or a Disciple: ";
//	std::cin >> first_input;
//	std::transform(first_input.begin(), first_input.end(), first_input.begin(), [](unsigned char ch) {return std::tolower(ch); });
//	//std::cout << first_input<<std::endl;
//	//first_input.
//	if (first_input == "master") {
//
//	}
//}

void set_assgined(int m_id, int d_id) {
	
	masters[m_id].assigned = true;
	masters[m_id].assigned_id = d_id;
	

	disciples[d_id].assigned = true;
	disciples[d_id].assigned_id = m_id;

	
	
}

bool compare_pref(int m_id, int d_id, int new_m_id) {
	int current_master = disciples[d_id].assigned_id;
	int current_index = 0;
	int new_index = 0;

	for (int i = 0; i < disciples[d_id].preferred_masters.size(); i++) {
		if (disciples[d_id].preferred_masters[i] == current_master) {
			current_index = i;
			break;
		}
	}

	for (int i = 0; i < disciples[d_id].preferred_masters.size(); i++) {
		if (disciples[d_id].preferred_masters[i] == new_m_id) {
			new_index = i;
			break;
		}
	}

	return (new_index < current_index);
}

void master_matching() {
	std::queue<int> free_master;
	//free_master.push(masters[0]);
	for (Master master : masters) {
		free_master.push(master.id);
	}
	std::vector<int> master_next(masters.size(), 0);
	
	while (!free_master.empty()) {
		int current_master_id = free_master.front();
		//std::cout << current_master_id << std::endl;;
		
		if (master_next[current_master_id] >= masters[current_master_id].preferred_disciples.size()) {
			free_master.pop();
			continue;
		}
		int current_disciple_id = masters[current_master_id].preferred_disciples[master_next[current_master_id]];


		if (!disciples[current_disciple_id].assigned) {
			set_assgined(current_master_id, current_disciple_id);
			free_master.pop();
		}
		else {
			int ex_master = disciples[current_disciple_id].assigned_id;
			if (compare_pref(ex_master, current_disciple_id, current_master_id)) {
				masters[ex_master].assigned = false;
				masters[ex_master].assigned_id = -1;
				
				set_assgined(current_master_id, current_disciple_id);

				free_master.pop();
				free_master.push(ex_master);
			}
			else {
				master_next[current_master_id]++;
				//continue;
			}
			
			//index++;
			//continue;
		}

		
	}
}

void disciple_matching() {

}

int main() {
	//init here first
	initialization();
	//std::string first_input;
	//std::cout << "Welcome! Are you a Master or a Disciple: ";
	//std::cin >> first_input;
	//std::transform(first_input.begin(), first_input.end(), first_input.begin(), [](unsigned char ch) {return std::tolower(ch); });
	////std::cout << first_input<<std::endl;
	////first_input.
	//if (first_input == "master") {

	//}
	//after the input, do the matching
	master_matching();

	for (Master master : masters) {
		std::cout << "Masters ranking: " << std::endl;

		for (int d_id : master.preferred_disciples) {
			std::cout << d_id << " ,";
		}
		std::cout<<std::endl;

		std::cout << "Assigned disciple id: " << master.assigned_id << std::endl;
		//std::cout << masters[id].preferred_disciples;
	}

	for (Disciple disciple : disciples) {
		std::cout << "Disciples ranking: " << std::endl;

		for (int m_id : disciple.preferred_masters) {
			std::cout << m_id << " ,";
		}
		std::cout<<std::endl;

		std::cout << "Assigned Master id: " << disciple.assigned_id << std::endl;
		//std::cout << masters[id].preferred_disciples;
	}

	return 0;
}