#ifndef SRTF_HEADER
#define SRTF_HEADER

#include <iostream>
#include <fstream>
#include <tuple>
#include <vector>
#include <algorithm>
#include <string>
#include <queue>
using namespace std;


#include "Scheduler.h"
#include "RemainTimeShorterThan.h"

//Shortest Remaining Time First Class
//It basically use the process_info member variable from the Base class Scheduler
class SRTF : public Scheduler{
public:
	priority_queue<shared_ptr<Process>, vector<shared_ptr<Process> >, RemainTimeShorterThan> SRTF_queue;
	void printByArrivalTime();
	void schedule_tasks();
	SRTF(string file);
};

#endif