//start off by gale shapley algo
#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <algorithm>
#include <cstdlib>
#include <random>
#include <array>

//references:
//https://en.cppreference.com/cpp/string/byte/tolower
//https://medium.com/@lsc830621/algorithm-stable-matching-problem-implementation-using-c-b40c6c6fc71a
//https://en.cppreference.com/cpp/numeric/random/uniform_int_distribution
//https://stackoverflow.com/questions/257091/how-do-i-flush-the-cin-buffer

#define MIN_NUM 5
#define MAX_NUM 20

//Have to add criteria and sort preference list based on that
//for masters, what type of craft they offer
enum craft {
	NYONYA_KEBAYA,
	PERANAKAN_BEADWORK,
	LANTERN_MAKING,
	RATTAN_WEAVING,
	TRADITIONAL_PASTRY_MAKING
};

//for disciple, what was their background before wanting to learn from the masters
enum background {
	ART,
	COOKING

};

//Maybe the disciples' criteria is the type of craft the master has to offer
struct Disciple {
	int id;
	bool assigned;
	int assigned_id;
	background bg;
	std::vector<craft> preferred_craft;
	std::vector<int> preferred_masters;
};

//The masters' criteria is how talented the disciple is? or how much the student is willing to commit (number of hours per day?)
struct Master {
	int id;
	bool assigned;
	int assigned_id;
	craft type;
	std::vector<background> preferred_bg;
	std::vector<int> preferred_disciples;
};

//The program will first start off by asking if the user is a master or disciple
//Then, based on whether they are master / disciple, they will be asked on their own details
//Like masters will be required to input the type of craft they are, based on a list of choice?
//The disciples will be asked to input how much they are willing to commit? (like the numbers of hours per day they can commit)
//Then, a list will be given where the master/disciple rank based on their preference
//Afterwards, the result will appear to say which master/disciple they are assigned to

std::array<std::string, 5> craft_list_string{ "NYONYA KEBAYA", "PERANAKAN BEADWORK", "LANTERN MAKING","RATTAN_WEAVING", "TRADITIONAL_PASTRY_MAKING" };
std::array<std::string, 2> bg_list_string{ "ART", "COOKING" };

std::array<craft, 5> craft_list_enum{ NYONYA_KEBAYA, PERANAKAN_BEADWORK, LANTERN_MAKING, RATTAN_WEAVING, TRADITIONAL_PASTRY_MAKING };
std::array<background, 2> bg_list_enum{ ART,
	COOKING };

std::vector<Master> masters;
std::vector<Disciple> disciples;

int generate_rand(int min, int max) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(min, max);
	return dist(gen);
}

void init_user_master() {
	//Add the master first
	Master tmp;
	masters.push_back(tmp);
	int id = masters.size() - 1;
	while (true) {
		int craft_type = -1;
		std::cout << R"(Based on the list:
0 - NYONYA_KEBAYA,
1 -	PERANAKAN_BEADWORK,
2 -	LANTERN_MAKING,
3 -	RATTAN_WEAVING,
4 -	TRADITIONAL_PASTRY_MAKING

What type of craft do you teach: )";
		//std::cin >> craft_type;

		if (std::cin >> craft_type && craft_type >= 0 && craft_type < 5) {
			masters[id].type = craft_list_enum[craft_type];
			break;
		}
		//
		//if (craft_type == 0) {
		//	masters[id].type = craft::NYONYA_KEBAYA;
		//	break;
		//}
		//else if (craft_type == 1) {
		//	masters[id].type = craft::PERANAKAN_BEADWORK;
		//	break;
		//}
		//else if (craft_type == 2) {
		//	masters[id].type = craft::LANTERN_MAKING;
		//	break;
		//}
		//else if (craft_type == 3) {
		//	masters[id].type = craft::RATTAN_WEAVING;
		//	break;
		//}
		//else if (craft_type == 4) {
		//	masters[id].type = craft::TRADITIONAL_PASTRY_MAKING;
		//	break;
		//}
		else {
			std::cout << "Please enter numbers from 0 to 4 only.\n";
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			continue;
		}


	}
	std::cout << "Please rank the background of the disciples based on most preferred to least preferred\n";
	//std::cout << "Please rank the disciples from most preferred to least preferred\n";
	for (int i = 0; i < bg_list_string.size(); i++) {
		std::cout << i << " - " << bg_list_string[i] << std::endl;
	}
	
	for (int i = 0; i < bg_list_enum.size(); i++) {
		
		int bg = -1;
		while (true) {
			//std::cin >> bg;
			std::cout << "Which background do you prefer: ";
			if (std::cin >> bg&& bg >= 0 && bg < bg_list_enum.size()) {
				bool valid = true;
				for (background bg_check : masters[id].preferred_bg) {
					if (masters[id].preferred_bg.size() == 1 && masters[id].preferred_bg[0] == bg) {
						valid = false;
					}
					else if (masters[id].preferred_bg.size() > 1) {
						if (std::find(masters[id].preferred_bg.begin(), masters[id].preferred_bg.end(), bg_check) != masters[id].preferred_bg.end()) {
							valid = false;
						}
					}
					
				}
				if (!valid) {
					std::cout << "Please enter a different number.\n";
					std::cin.clear();
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					continue;
				}
				break;
			}
			else {
				std::cout << "Please enter numbers from 0 to 1 only.\n";
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			}
		}
		masters[id].preferred_bg.push_back(bg_list_enum[bg]);

	}

	//for (Disciple d : disciples) {
	//	std::cout << "ID: " << d.id << std::endl;
	//	std::cout << "Background: " << bg_list_string[static_cast<int>(d.bg)] << std::endl;
	//}

	//for (int i = 0; i < disciples.size(); i++) {
	//	int d_id = -1;
	//	while (true) {
	//		//int d_id = -1;
	//		std::cin >> d_id;
	//		if (d_id >= 0 && d_id < disciples.size()) {
	//			break;
	//		}
	//		else {
	//			"Please enter a valid number.\n";
	//			continue;
	//		}
	//	}
	//	masters[id].preferred_disciples.push_back(d_id);

	//}

	masters[id].assigned = false;
	masters[id].assigned_id = -1;
	masters[id].id = id;
}

void init_masters(int num_masters, int num_disciples, int master_init) {
	for (int i = master_init; i < num_masters; i++) {
		Master tmp;
		masters.push_back(tmp);
		masters[i].id = i;
		masters[i].assigned = false;
		masters[i].assigned_id = -1;
		masters[i].type = craft_list_enum[generate_rand(0, 4)];
		for (int j = 0; j < num_disciples; j++) {
			masters[i].preferred_disciples.push_back(j);
		}
		std::random_shuffle(masters[i].preferred_disciples.begin(), masters[i].preferred_disciples.end());


	}
}

void init_disciples(int num_masters, int num_disciples, int disciple_init) {
	for (int i = disciple_init; i < num_disciples; i++) {
		Disciple tmp;
		disciples.push_back(tmp);
		disciples[i].id = i;
		disciples[i].assigned = false;
		disciples[i].assigned_id = -1;
		disciples[i].bg = bg_list_enum[generate_rand(0, 1)];
		for (int j = 0; j < num_masters; j++) {
			disciples[i].preferred_masters.push_back(j);
		}
		std::random_shuffle(disciples[i].preferred_masters.begin(), disciples[i].preferred_masters.end());


	}
}

void rank_masters() {

}
void rank_disciples() {

}


//To randomly generate a list of masters and disciples
void initialization(int master) {
	int num_masters = generate_rand(MIN_NUM, MAX_NUM);
	int num_disciple = generate_rand(MIN_NUM, MAX_NUM);

	int master_init = 0;
	int disciple_init = 0;

	if (master == 0) {

		init_disciples(num_masters, num_disciple, disciple_init);
		master_init++;
		init_user_master();
		init_masters(num_masters, num_disciple, master_init);
	}
	else if(master == 1) {
		init_masters(num_masters, num_disciple, master_init);
		disciple_init++;
		//init_user_disciple();
		init_disciples(num_masters, num_disciple, disciple_init);
	}
	else {
		//potentially for test case
		//Empty by defult
	}

	/*for (int i = master_init; i < num_masters; i++) {
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

	for (int i = disciple_init; i < num_disciple; i++) {
		Disciple tmp;
		disciples.push_back(tmp);
		disciples[i].id = i;
		disciples[i].assigned = false;
		disciples[i].assigned_id = -1;
		for (int j = 0; j < num_masters; j++) {
			disciples[i].preferred_masters.push_back(j);
		}
		std::random_shuffle(disciples[i].preferred_masters.begin(), disciples[i].preferred_masters.end());


	}*/

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
			}

		}

		
	}
}

void disciple_matching() {
	std::queue<int> free_disciple;

	for (Disciple disciple : disciples) {
		free_disciple.push(disciple.id);
	}
	std::vector<int> disciple_next(disciples.size(), 0);

	while (!free_disciple.empty()) {
		int current_disciple_id = free_disciple.front();

		if (disciple_next[current_disciple_id] >= disciples[current_disciple_id].preferred_masters.size()) {
			free_disciple.pop();
			continue;
		}
		int current_master_id = disciples[current_disciple_id].preferred_masters[disciple_next[current_disciple_id]];


		if (!masters[current_master_id].assigned) {
			set_assgined(current_master_id, current_disciple_id);
			free_disciple.pop();
		}
		else {
			int ex_disciple = masters[current_master_id].assigned_id;
			if (compare_pref(ex_disciple, current_disciple_id, current_master_id)) {
				disciples[ex_disciple].assigned = false;
				disciples[ex_disciple].assigned_id = -1;

				set_assgined(current_master_id, current_disciple_id);

				free_disciple.pop();
				free_disciple.push(ex_disciple);
			}
			else {
				disciple_next[current_disciple_id]++;
			}

		}


	}
}

int main() {
	//init here first
	//initialization();
	std::string first_input;
	while (true) {
		//std::string first_input;
		std::cout << "Welcome! Are you a Master or a Disciple: ";
		std::cin >> first_input;
		std::transform(first_input.begin(), first_input.end(), first_input.begin(), [](unsigned char ch) {return std::tolower(ch); });
		//std::cout << first_input<<std::endl;
		//first_input.
		if (first_input == "master") {
			/*//Add the master first
			Master tmp;
			masters.push_back(tmp);
			int id = masters.size()-1;
			while (true) {
				int craft_type = -1;
				std::cout << R"(Based on the list:
0 - NYONYA_KEBAYA,
1 -	PERANAKAN_BEADWORK,
2 -	LANTERN_MAKING,
3 -	RATTAN_WEAVING,
4 -	TRADITIONAL_PASTRY_MAKING

What type of craft do you teach: )";
				std::cin >> craft_type;

				if (craft_type == 0) {
					masters[id].type = craft::NYONYA_KEBAYA;
					break;
				}
				else if (craft_type == 1) {
					masters[id].type = craft::PERANAKAN_BEADWORK;
					break;
				}
				else if (craft_type == 2) {
					masters[id].type = craft::LANTERN_MAKING;
					break;
				}
				else if (craft_type == 3) {
					masters[id].type = craft::RATTAN_WEAVING;
					break;
				}
				else if (craft_type == 4) {
					masters[id].type = craft::TRADITIONAL_PASTRY_MAKING;
					break;
				}
				else {
					std::cout << "Please enter numbers from 0 to 4 only.\n";
					continue;
				}
				
				
			}
			masters[id].assigned = false;
			masters[id].assigned_id = -1;
			masters[id].id = 0;*/
			
			

			initialization(0);
			break;


			//after the input, do the matching

			//master_matching();
		}
		else if (first_input == "disciple") {
			Disciple tmp;
			disciples.push_back(tmp);
			initialization(1);
			break;
		}
		else if (first_input == "t") {
			//for testing
			initialization(2);
			break;
		}
		else {
			std::cout << "Please only enter 'master' or 'disciple'.\n";
		}
	}

	if (first_input == "master") {
		master_matching();
	}
	else if(first_input == "disciple"){
		disciple_matching();
	}

	
	

	//for (Master master : masters) {
	//	std::cout << "Masters ranking: " << std::endl;

	//	for (int d_id : master.preferred_disciples) {
	//		std::cout << d_id << " ,";
	//	}
	//	std::cout<<std::endl;

	//	std::cout << "Assigned disciple id: " << (master.assigned_id == -1? "None": std::to_string(master.assigned_id)) << std::endl;
	//	//std::cout << masters[id].preferred_disciples;
	//}

	//for (Disciple disciple : disciples) {
	//	std::cout << "Disciples ranking: " << std::endl;

	//	for (int m_id : disciple.preferred_masters) {
	//		std::cout << m_id << " ,";
	//	}
	//	std::cout<<std::endl;

	//	std::cout << "Assigned Master id: " << (disciple.assigned_id == -1? "None": std::to_string(disciple.assigned_id)) << std::endl;
	//	//std::cout << masters[id].preferred_disciples;
	//}

	return 0;
}