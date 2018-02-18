#ifndef ARRIVALEARLIERTHAN_HEADER
#define ARRIVALEARLIERTHAN_HEADER
#include "Process.h"
//Just a Comparator Structure for Picking the process which comes earlier
struct ArrivalEarlierThan{
	bool operator()(const Process& p1, const Process& p2) const{
		return p1.get_arrival_time() > p2.get_arrival_time();
	}
};

#endif