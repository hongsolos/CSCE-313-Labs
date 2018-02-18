#include "Scheduler.h"

//Read a process file to extract process information
//All content goes to proces_info vector
void Scheduler::extractProcessInfo(string file){
	std::ifstream input(file);
	string a, b, c;
	while (input >> a >> b >> c){
		ProcessInfo take_in (std::stoi(a),std::stoi(b), std::stoi(c));
		process_info.push_back(take_in);
		//DEBUG
		//cout << a << b << c;
	}
}