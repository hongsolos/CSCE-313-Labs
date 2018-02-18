#include "MLFQ.h"

//The goal of this function is to fill the 4 levels with process_info 
//You should sort the process_info by the arrival_time of the Process;
//Process with smaller arrival time will have smaller index in the vector

//After filling in the top 3 levels, then do insertion for the fcfs level
MLFQ::MLFQ(string file) {
	//Load from file
	extractProcessInfo(file);

	int first_level, second_level, third_level = 0;
	
	//Filling the bottom levels with inputs from process_info, sorted by arrival time
	for (int i = 0; i < process_info.size(); i++){
		Process input (get<0>(process_info[i]), 
					   get<1>(process_info[i]),		
					   get<2>(process_info[i]));
		lowestLevel.push(input);
	}
	//DEBUG
	//cout << lowestLevel.size() << endl;
	
	//Initializing the top three levels
	for (int i = 0; i < 3; i++){
		vector<shared_ptr<Process>> temp (NULL);
		upperLevels.push_back(temp);
	}
	
	//Take items from lowest level to be processed into top 3 levels
	//After the loop, lowest level should only contains items with the highest arrival time
	int counter = lowestLevel.size();
	for (int i = 0; i < counter; i++){
		shared_ptr <Process> temp = make_shared<Process> (lowestLevel.top());
		if (i < 20){
			upperLevels[0].push_back(temp);
			lowestLevel.pop();
		} else if (i < 40){
			upperLevels[1].push_back(temp);
			lowestLevel.pop();
		} else if (i < 60){
			upperLevels[2].push_back(temp);
			lowestLevel.pop();
		} else {
			continue;
		}
	}
	//DEBUG
	// cout << lowestLevel.size() << endl;
	// cout << upperLevels[0].size() << endl;
	// cout << upperLevels[1].size() << endl;
	// cout << upperLevels[2].size() << endl;
}
//This function is used to keep track of the process who entered the fcfs queue most recently
//The purpose is to properly adjust the upcoming process's arrival time
//If the arrival time is changed, you can still check its original arrival time by accessing the 
//process_info variable
void MLFQ::update_last_process_fcfs(Process process_to_fcfs) {
	last_process_fcfs = process_to_fcfs;
}

//This function has the following assumptions:
/*
	1. The start_level is not equal goal_level;
	2. goal_level is larger then start_level
	3. When you use this function, you should know the process should not jump from level 0 to level 2 or 3 if the level 1 has a space there.
	   Generally, when you degrade a process, it tries to go to the level below by one level and if that level is full, it will keep going down 
	   until it finds a level which has space there.
	4. Successful jump will return 1, else 0
	5. To successfully jump to the goal_level, the process must go to the end of the vector corresponding to goal_level
*/
//start_level is the level the process is located at, it is one value of 0 , 1, 2;
//pos is its index in the vector
//goal_level is the level it tries to enter
int MLFQ::level_jump(shared_ptr<Process> p, unsigned int start_level, unsigned int pos, unsigned int goal_level) {
	for (int i = start_level + 1; i < goal_level; i++){
		if (upperLevels[i].size() < 20){ //Upper Level Check
			upperLevels[i].push_back(p);
			upperLevels[start_level].erase(upperLevels[start_level].begin() + pos);
			return 1;
		}
		
		if (goal_level == 3){ //Lowest Level it is
			lowestLevel.push(*p);
			upperLevels[start_level].erase(upperLevels[start_level].begin() + pos);
			return 1;
		}
	}
	return 0;
}


/*
 p is the process which is going to be degrade to lower levle
 levle is the level it is located currently
 legal value of level can be: 0, 1, 2 Since no need to degrade for last level
 pos is the its index in the vector

0: it is located at the top level
1:  it is located at the second highest level
2:  it is located at the third highest level
3: it is in the fcfs level, no need to degrade, it will stay there until finishing the job and leave
*/

//pos is the index of the process in the vector
//Your goal is to degrade this process by one level
//You can use level_jump() function here based on which level the process is going to jump
void MLFQ::degrade_process(shared_ptr<Process> p, unsigned int level, unsigned int pos) {
	if (level < 3){
		int goal = 3;
		level_jump(p, level, pos, goal);
	}
}

/*
You can use multiple loops here to do the job based on the document;
Make sure print out the timing information correctly
*/
void MLFQ::schedule_tasks(){
	
	int quantum_upper = 16;
	
	int run_time = 0;
	bool running = true;
	
	int wait_time = 0;
	int service_time = 0;
	int response_time = 0;


	double avg_wait_time = 0;
	double avg_service_time = 0; 
	double avg_response_time = 0;
	double avg_n = 0;
	//DEBUG
	//cout << upperLevels[0].size();
	printf("%10s\t%10s\t%10s\t%10s\n","PID","SERVICE","WAIT","RESPONSE");
	
	while (running){ //Running the processes
		//For each level 1, 2 and 3 -------Apply Round Robin--------------------
		for (int i = 0; i < 3; i++){ 
			cout << "Processing Level " << i+1 << endl;

			for(std::vector<shared_ptr<Process>>::iterator it = upperLevels[i].begin(); upperLevels[i].size()>0; /*Nothing*/){
				
				// If Process Has Not Arrived, Sleep
				if(run_time < (*it)->get_arrival_time()){
				// Cycle Through Other Processes
					std::vector<shared_ptr<Process>>::iterator begin = it;
					it++;
					if(it >= upperLevels[i].end()) it = upperLevels[i].begin();
					while(run_time < (*it)->get_arrival_time() && it!=begin){
						//printf("%d %p\n",(*it)->getPid(),it);
						it++;
						if(it >= upperLevels[i].end()) it = upperLevels[i].begin();
					}
				
					// Do Nothing if Nothing Else Is Ready
					if(begin == it){
						//printf("%s%-10i%s\n", "t = ", run_time, ".........Do Nothing");
						run_time++;				
					}
				} else { //Otherwise, Execute
					// Run Process up to Completion or Time Quantum (4, 8 and 16 according to the level)
					for(int j = 0; (!(*it)->is_Completed()) && (j < quantum_upper*pow(2, i)); j++){
						(*it)->Run(1);
						//printf("%s%-10i%s%-4i%s\n", "t = ", run_time, ".........[PID = ", (*it)->getPid(), "] Running");
						run_time++;
					}
					// Print "Finished" if Process Is Completed
					
					if((*it)->is_Completed()){
						//printf("  -> %s%-5i%s%-4i%s\n", "t = ", (run_time), ".........[PID = ", (*it)->getPid(), "] Finished");
						// Calculations
						response_time = (run_time - (*it)->get_arrival_time());
						service_time = (*it)->get_cpu_burst_time();
						wait_time = response_time - service_time;
				
						printf("%10i\t%10i\t%10i\t%10i\n",(*it)->getPid(),service_time,wait_time,response_time);
		
						// Average Calculations
						avg_n++;
						avg_response_time += response_time;
						avg_service_time += service_time;
						avg_wait_time += wait_time;
								
						// Erase Process
						upperLevels[i].erase(it);
					} else {
						it++;
					}
					//DEBUG
					//cout << upperLevels[i].size() << endl;
					
					// Loop Around
					if(it >= upperLevels[i].end()) it = upperLevels[i].begin();
				}
			}
		}
		//Level 4 --- Limit of 5 processes
			cout << "Processing Level " << 4 << endl;
		int size = lowestLevel.size();
		//cout << size << endl;
		if (size > 5){
			size = 5;
		}
		for(int i = 0; i < size; i++){
			// Pop off Queue
			Process temp = lowestLevel.top();
			lowestLevel.pop();
			
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
			//printf("  -> %s%-5i%s%-4i%s\n", "t = ", (run_time-1), ".........[PID = ", tempPID, "] Finished");
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
		

		
		//Check if everything is done
		if (upperLevels[0].size() == 0 && upperLevels[1].size() == 0 && upperLevels[2].size() == 0 && lowestLevel.size() == 0){
			running = false;
		}
	}
	
	avg_response_time /= 1.0*avg_n;
	avg_service_time /= 1.0*avg_n;
	avg_wait_time /= 1.0*avg_n;
		
	std::cout << std::endl << "Average Service Time: " << avg_service_time << std::endl;
	std::cout << "Average Wait Time: " << avg_wait_time << std::endl;
	std::cout << "Average Response Time: " << avg_response_time << std::endl;
	
}
