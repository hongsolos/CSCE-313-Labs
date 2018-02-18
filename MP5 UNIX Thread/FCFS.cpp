#include "FCFS.h"

//Use Base class's function to extract process information from file
//And fill in the fcfs_queue; the priority in this queue is the arrival time; 
//Make sure you did the ArrivalEarlierThan Comparator
FCFS::FCFS(string file)
{
	Scheduler::extractProcessInfo(file);
	
	for (int i = 0; i < Scheduler::process_info.size(); i++){
		Process input (get<0>(Scheduler::process_info[i]), 
					   get<1>(Scheduler::process_info[i]),		
					   get<2>(Scheduler::process_info[i]));
		fcfs_queue.push(input);
	}
}

void FCFS::print(){
	cout<<"pid\t"<<"  arrival time\t"<<"\tburst time:\n";
	for(auto ele:process_info){
		cout<<get<0>(ele)<<"\t"<<get<1>(ele)<<"\t"<<get<2>(ele)<<endl;
	}
}

//Assuming the time the CPU starts working with process is system time 0
void FCFS::schedule_tasks(){
	
	int size = fcfs_queue.size();
	int run_time = 0;
	
	int wait_time = 0;
	int service_time = 0;
	int response_time = 0;


	double avg_wait_time = 0;
	double avg_service_time = 0; 
	double avg_response_time = 0;
	double avg_n = 0;
	
	printf("%10s\t%10s\t%10s\t%10s\n","PID","SERVICE","WAIT","RESPONSE");
	
	for(int i = 0; i < size; i++){
		// Pop off Queue
		Process temp = fcfs_queue.top();
		fcfs_queue.pop();
		
		int tempPID = temp.getPid();
		
		// Wait Until Next Task Arrives
		for(int i = 0; run_time < temp.get_arrival_time(); i++){
			//printf("%s%-10i%s\n", "t = ", run_time, ".........Do Nothing");
			run_time++;
		}
		
		for (int j = 0; j < temp.get_cpu_burst_time(); j++){
			temp.Run(1);
			//printf("%s%-10i%s%-4i%s\n", "t = ", run_time, ".........[PID = ", tempPID, "] Running");
			run_time++;
		}
		//printf("  -> %s%-5i%s%-4i%s\n", "t = ", (run_time), ".........[PID = ", tempPID, "] Finished");
		
		// Calculations
		response_time = (run_time - temp.get_arrival_time());
		service_time = temp.get_cpu_burst_time();
		wait_time = response_time - service_time;
		
		printf("%10i\t%10i\t%10i\t%10i\n",tempPID,service_time,wait_time,response_time);

		// Average Calculations
		avg_n++;
		avg_response_time += response_time;
		avg_service_time += service_time;
		avg_wait_time += wait_time;
	}
	
	avg_response_time /= 1.0*avg_n;
	avg_service_time /= 1.0*avg_n;
	avg_wait_time /= 1.0*avg_n;
	
	std::cout << std::endl << "Average Service Time: " << avg_service_time << std::endl;
	std::cout << "Average Wait Time: " << avg_wait_time << std::endl;
	std::cout << "Average Response Time: " << avg_response_time << std::endl;
	
}


FCFS::FCFS(){}