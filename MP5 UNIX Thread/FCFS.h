#ifndef FCFS_HEADER
#define FCFS_HEADER

#include <iostream>
#include <fstream>
#include <tuple>
#include <vector>
#include <algorithm>
#include <string>
#include <queue>
using namespace std;


#include "Scheduler.h"
#include "ArrivalEarlierThan.h"

//This is the First Come Firsst Serve Class
//It basically use the member variable process_info from its parent Class Scheduler
//It has a priority_queue which uses arrival time as the comparing key, which follows the first in first out rule
class FCFS : public Scheduler{	
public:
	priority_queue<Process, vector<Process>, ArrivalEarlierThan> fcfs_queue;
	void print();
	void schedule_tasks();
	FCFS(string file);
	FCFS();
};
#endif
