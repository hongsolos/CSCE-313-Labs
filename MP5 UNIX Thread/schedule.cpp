#include "FCFS.h"
#include "RoundRobin.h"
#include "SRTF.h"
#include "MLFQ.h"
#include <getopt.h>
#include <iostream>
#include <string>

//Example is from this website: https://codeyarns.com/2015/01/30/how-to-parse-program-options-in-c-using-getopt_long/
unsigned int quantum = 0;
std::string policy = "";
std::string input_file = "";

void PrintHelp()
{

	cout << "Usage: ./schedule --input_file <file> --policy <policy> --quantum <quantum>\n\n";
	cout<<"--------------------------------------------------------------------------\n";
	cout<<"<policy>:  "<<"FCFS | RR | SRTF | MLFQ\n";
	cout<<"--------------------------------------------------------------------------\n";
	cout<<"<file>:    "<<"file with PID, Arrival_Time and Execution Time, separated with a tab\n";
	cout<<"--------------------------------------------------------------------------\n";
	cout<<"<quantum>: "<<"positive integer value\n";
	cout<<"--------------------------------------------------------------------------\n";
	cout << "Only use quantum when you want to use RoundRobin policy and make sure quantum is a positive number\n";
	exit(1);
}

void ProcessArgs(int argc, char** argv)
{
	if (argc < 5) {
		PrintHelp();
		//exit(0);
	}
	else if (argc == 5 && string(argv[4]) == "RR"){
		PrintHelp();
		//exit(1);
	}
	else if (argc == 6 && string(argv[4]) != "RR"){
		PrintHelp();
		//exit(0);
	}
	else if (argc >7){
		PrintHelp();
		//exit(0);
	}
	
	const char* const short_opts = "f:p:q:h:";
	const option long_opts[] = {
		{ "input_file", 1, nullptr, 'f' },
		{ "policy", 1, nullptr, 'p' },
		{ "quantum", 1, nullptr, 'q' },
		{ "help", 0, nullptr, 'h' },
		{ nullptr, 0, nullptr, 0 }
	};


	while (true)
	{
		const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

		if (-1 == opt)
			break;


		std::string temp = "";
		switch (opt)
		{
			case 'f':
				input_file = std::string(optarg);
				break;
			case 'p':
				policy = std::string(optarg);
				break;
			case 'q':
				quantum = std::stol(optarg);
				break;
			case 'h':
				PrintHelp();
				break;
			default:
				PrintHelp();
				break;
		}
	}
	
}

int main(int argc, char **argv)
{
	bool legal_command = false;
	ProcessArgs(argc, argv);
	
	// DEBUG
	//cout << "Input File: " << input_file << endl;
	//cout << "Policy:     " << policy << endl;
	//cout << "Quantum:    " << quantum << endl;
	
	if (policy == "RR") {
		RoundRobin rr_scheduler = RoundRobin(input_file, quantum);
		rr_scheduler.schedule_tasks();
		cout << "<--Finished Round Robin-->" << endl;
	}
	else if (policy == "SRTF") {
		SRTF srtf_scheduler = SRTF(input_file);
		srtf_scheduler.schedule_tasks();
		cout << "<--Finished Shortest Run Time First-->" << endl;
	}
	else if (policy == "FCFS") {
		FCFS fcfs_scheduler = FCFS(input_file);
		fcfs_scheduler.schedule_tasks();
	}
	else if (policy == "MLFQ") {
		MLFQ mlfq_scheduler = MLFQ(input_file);
		mlfq_scheduler.schedule_tasks();
	}
	return 0;
}
