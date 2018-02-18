#include "RoundRobin.h"

//No need to change it
RoundRobin::RoundRobin() {
	time_quantum = 0;
}


/*
This is a constructor for RoundRobin Scheduler, you should use the extractProcessInfo function first
to load process information to process_info and then sort process by arrival time;
Also, fill in the procesVec with shared_ptr

Also initialize time_quantum
*/
RoundRobin::RoundRobin(string file, int time_quantum) {
	
	// Load from file
	Scheduler::extractProcessInfo(file);
	
	// Sort by Arrival Time
	std::sort(begin(process_info), end(process_info), [](ProcessInfo const &t1, ProcessInfo const &t2) {
		return get<1>(t1) < get<1>(t2);
	});
	
	// Fill processVec
	for (int i = 0; i < Scheduler::process_info.size(); i++){
		processVec.push_back(shared_ptr<Process>(new Process(get<0>(process_info[i]), get<1>(process_info[i]), get<2>(process_info[i]))));
	}
	
	// Fill time_quantum
	this->time_quantum = time_quantum;
	
	// Check Order of Queue
	//this->print();
	
	
	
}

void RoundRobin::set_time_quantum(int quantum) {
	this->time_quantum = quantum;
}

int RoundRobin::get_time_quantum() {
	return time_quantum;
}


//Schedule tasks based on RoundRobin Rule
//the jobs are put in the order the arrived
//Make sure you print out the information like we put in the document
void RoundRobin::schedule_tasks() {
	
	int t = 0; // System Timer
	
	int wait_time = 0;
	int service_time = 0;
	int response_time = 0;


	double avg_wait_time = 0;
	double avg_service_time = 0; 
	double avg_response_time = 0;
	double avg_n = 0;
	
	printf("%10s\t%10s\t%10s\t%10s\n","PID","SERVICE","WAIT","RESPONSE");
	
	for(std::vector<shared_ptr<Process>>::iterator it = processVec.begin(); processVec.size()>0; /*Nothing*/){

		// If Process Has Not Arrived, Sleep
		if(t < (*it)->get_arrival_time()){
			
			// Cycle Through Other Processes
			std::vector<shared_ptr<Process>>::iterator begin = it;
			
			it++;
			if(it >= processVec.end()) it = processVec.begin();
		
			while(t < (*it)->get_arrival_time() && it!=begin){
				//printf("%d %p\n",(*it)->getPid(),it);
				it++;
				if(it >= processVec.end()) it = processVec.begin();
			}
			
			// Do Nothing if Nothing Else Is Ready
			if(begin == it){
				//printf("%s%-10i%s\n", "t = ", t, ".........Do Nothing");
				t++;				
			}

		}
		// Otherwise, Execute
		else{
			// Run Process up to Completion or Time Quantum
			for(int j = 0; ( (!(*it)->is_Completed()) && (j < time_quantum) ); ++j)
			{
				(*it)->Run(1);
				//printf("%s%-10i%s%-4i%s\n", "t = ", t, ".........[PID = ", (*it)->getPid(), "] Running");
				t++;
			}
			
			// Print "Finished" if Process Is Completed
			if((*it)->is_Completed()){
				//printf("  -> %s%-5i%s%-4i%s\n", "t = ", (t), ".........[PID = ", (*it)->getPid(), "] Finished");
						
				// Calculations
				response_time = (t - (*it)->get_arrival_time());
				service_time = (*it)->get_cpu_burst_time();
				wait_time = response_time - service_time;
				
				printf("%10i\t%10i\t%10i\t%10i\n",(*it)->getPid(),service_time,wait_time,response_time);
		
				// Average Calculations
				avg_n++;
				avg_response_time += response_time;
				avg_service_time += service_time;
				avg_wait_time += wait_time;				
				
				// Erase Process
				processVec.erase(it);
			}
			else{
				it++;
			}
	
			// Loop Around
			if(it >= processVec.end()) it = processVec.begin();
		}
		
		//this->print();
	}
	
	
	avg_response_time /= 1.0*avg_n;
	avg_service_time /= 1.0*avg_n;
	avg_wait_time /= 1.0*avg_n;
	
	std::cout << std::endl << "Average Service Time: " << avg_service_time << std::endl;
	std::cout << "Average Wait Time: " << avg_wait_time << std::endl;
	std::cout << "Average Response Time: " << avg_response_time << std::endl;
	
	std::cout << "Time Quantum: " << time_quantum << std::endl;
	
	
	
}

//Fill in the procesVec with shared_ptr
//And set time_quantum, or you can also use set_time_quantum function for setting quantum
RoundRobin::RoundRobin(vector<ProcessInfo> &process_info, int time_quantum){
	for (int i = 0; i < process_info.size(); i++){
		processVec.push_back(shared_ptr<Process>(new Process(get<0>(process_info[i]), get<1>(process_info[i]), get<2>(process_info[i]))));
		set_time_quantum(time_quantum);
	}
}

void RoundRobin::print(){
	for (auto ele : processVec){
		cout << ele->getPid() << "; " << ele->get_arrival_time() << "; " << ele->get_cpu_burst_time() << endl;
	}
}