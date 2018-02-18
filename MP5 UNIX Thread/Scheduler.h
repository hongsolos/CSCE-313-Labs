#ifndef SCHEDULER_HEADER
#define SCHEDULER_HEADER
#include <string>
#include <vector>
#include <fstream>
#include <tuple>
#include <algorithm>
#include <iostream>
using namespace std;
//pid, arrival time, execution time
typedef tuple<int, int, int> ProcessInfo;
//Generic Base Class
class Scheduler{
public:
	string policy_name;
	vector<ProcessInfo> process_info;
	Scheduler(){}
	Scheduler(string name){ policy_name = name; }
	void extractProcessInfo(string file);
};

#endif