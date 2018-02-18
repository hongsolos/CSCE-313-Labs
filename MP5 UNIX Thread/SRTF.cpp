#include "SRTF.h"

void SRTF::printByArrivalTime(){
	cout << "pid   " << "  arrival time     " << "     burst time:\n";
	for (auto ele : process_info){
		cout << get<0>(ele) << "\t" << get<1>(ele) << "\t" << get<2>(ele) << endl;
	}
}

SRTF::SRTF(string file)
{
	extractProcessInfo(file);
	std::sort(begin(process_info), end(process_info), [](ProcessInfo const &t1, ProcessInfo const &t2) {
		return get<1>(t1) < get<1>(t2);
	});
}
//scheduling implementation
void SRTF::schedule_tasks(){
	
	int t = 0;
	
	// Order Processes by Run Time
	for (int i = 0; i < process_info.size(); i++){
		Process input (get<0>(process_info[i]), get<1>(process_info[i]), get<2>(process_info[i]));
		SRTF_queue.push(make_shared<Process> (input)); //New arrival added, sorted by Remainin
	}	
	
	int wait_time = 0;
	int service_time = 0;
	int response_time = 0;


	double avg_wait_time = 0;
	double avg_service_time = 0; 
	double avg_response_time = 0;
	double avg_n = 0;
	
	//printf("%10s\t%10s\t%10s\t%10s\n","PID","SERVICE","WAIT","RESPONSE");
		
	while(SRTF_queue.size()>0){
	
		// Execute Process
		bool executed = false;
		priority_queue<shared_ptr<Process>, vector<shared_ptr<Process> >, RemainTimeShorterThan> temp;
		int SRTF_size = SRTF_queue.size();
		for(int i = 0; i < SRTF_size; i++){
			// DEBUG
			//std::cout << SRTF_queue.size() << ": ";
	        //std::cout << SRTF_queue.top()->getPid() << " ";
	        //std::cout << SRTF_queue.top()->get_remaining_time() << " ";
	        //std::cout << std::endl;			
			
			// Remove Completed Processes
			if(SRTF_queue.top()->get_remaining_time() <= 0){
				printf("  -> %s%-5i%s%-4i%s\n", "t = ", (t), ".........[PID = ", SRTF_queue.top()->getPid(), "] Finished");
				
				// Calculations
				response_time = (t - SRTF_queue.top()->get_arrival_time());
				service_time = SRTF_queue.top()->get_cpu_burst_time();
				wait_time = response_time - service_time;
				
				//printf("%10i\t%10i\t%10i\t%10i\n",SRTF_queue.top()->getPid(),service_time,wait_time,response_time);
		
				// Average Calculations
				avg_n++;
				avg_response_time += response_time;
				avg_service_time += service_time;
				avg_wait_time += wait_time;				
				
				
				SRTF_queue.pop();
				SRTF_size = SRTF_queue.size();	
				if(!(i<SRTF_size)) break;
			}
			
			// If Process Not Arrived, Next Shortest Process
			if(t < SRTF_queue.top()->get_arrival_time()){
				temp.push(SRTF_queue.top());
				SRTF_queue.pop();
			}
			// Execute
			else{
				SRTF_queue.top()->Run(1);
				printf("%s%-10i%s%-4i%s\n", "t = ", t, ".........[PID = ", SRTF_queue.top()->getPid(), "] Running");
				t++;
				
				executed = true;
				
				// Restore Queue
				for(int j = 0; j < i; j++){
					SRTF_queue.push(temp.top());
					temp.pop();
				}
				break;
			}			
		}
		
		// Nothing Available for Execution
		if(!executed) {
			// Restore Queue
			while(temp.size()>0){
				SRTF_queue.push(temp.top());
				temp.pop();
			}	
			
			if(SRTF_queue.size()<=0) break;
			
			printf("%s%-10i%s\n", "t = ", t, ".........Do Nothing");
			t++;			
		}
	
	
		// DEBUG
		//printf("%d\n",SRTF_queue.size());
	}
	
	avg_response_time /= 1.0*avg_n;
	avg_service_time /= 1.0*avg_n;
	avg_wait_time /= 1.0*avg_n;
	
	//std::cout << std::endl << "Average Service Time: " << avg_service_time << std::endl;
	//std::cout << "Average Wait Time: " << avg_wait_time << std::endl;
	//std::cout << "Average Response Time: " << avg_response_time << std::endl;
		
	
	
	
	
	// DEBUG
	//while(!SRTF_queue.empty()) {
	//    std::cout << SRTF_queue.top()->getPid() << " ";
	//    std::cout << SRTF_queue.top()->get_remaining_time() << " ";
	//    std::cout << std::endl;
	//    SRTF_queue.pop();
	//}
	//std::cout << '\n';
	
	/*
	priority_queue<shared_ptr<Process>, vector< shared_ptr <Process> >, RemainTimeShorterThan> temp = SRTF_queue;
	int run_time = 0;
	bool all_done = false;
	vector <ProcessInfo> process_sorted = process_info;
	
	
		//At every run time, checking for new arrival Process
		for (int i = 0; i < process_sorted.size(); i++){

				Process input (get<0>(process_sorted[i]), 
							   get<1>(process_sorted[i]),		
							   get<2>(process_sorted[i]));
							   
				input.print();					
							   
				SRTF_queue.push(make_shared<Process> (input)); //New arrival added, sorted by Remainin
				process_sorted.pop_back();
			
		}
		
		printf("%d",SRTF_queue.size());
		
	    while(!SRTF_queue.empty()) {
	        std::cout << SRTF_queue.top()->getPid() << " ";
	        SRTF_queue.pop();
	    }
	    std::cout << '\n';
	*/
	
	
	
	
	
	
		
	/*
	while (!all_done){
		//At every run time, checking for new arrival Process
		for (int i = 0; i < process_sorted.size(); i++){
			if (get<1>(process_sorted[i]) <= run_time){
				Process input (get<0>(process_sorted[i]), 
							   get<1>(process_sorted[i]),		
							   get<2>(process_sorted[i]));
				SRTF_queue.push(make_shared<Process> (input)); //New arrival added, sorted by Remainin
				process_sorted.pop_back();
			}
		}
		
		//Take the first job in SRTF queue and do it
		shared_ptr<Process> job = SRTF_queue.top();
		SRTF_queue.pop();
		//Process job = load;
		int tempPID = job->getPid();
		for (int i = 0; i < job->get_remaining_time(); i++){
			job->Run(1);
			printf("%s%i%s%i%s", "System Time[", run_time, "].........Process[PID=", tempPID, "] is Running");
			run_time++;
			
			if (job->is_Completed()){ //Check if process is completed
					printf("%s%i%s%i%s", "System Time[", run_time, "].........Process[PID=", tempPID, "] is Finished");
			}
		}
		if (SRTF_queue.size() == 0){
			all_done = true;
		}
		
	}
	*/
	
}