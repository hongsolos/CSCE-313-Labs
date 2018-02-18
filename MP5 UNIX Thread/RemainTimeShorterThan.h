#ifndef REMAINTIMESHORTERTHAN_HEADER
#define REMAINTIMESHORTERTHAN_HEADER
#include <memory>
#include "Process.h"

//A Comparator Class which compares the remainig time;
struct RemainTimeShorterThan{
	bool operator()(shared_ptr<Process> p1, shared_ptr<Process> p2) const{
		return p1->get_remaining_time() > p2->get_remaining_time();
	}
};

#endif