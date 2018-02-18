/*
    File: client_MP8.cpp

    Author: J. Higginbotham
    Department of Computer Science
    Texas A&M University
    Date  : 2016/05/21

    Based on original code by: Dr. R. Bettati, PhD
    Department of Computer Science
    Texas A&M University
    Date  : 2013/01/31

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/
#define DEFAULT_HOST_NAME "127.0.0.1"
#define DEFAULT_PORT_NAME "11111"
/* -- (none) -- */
/* -- This might be a good place to put the size of
    of the patient response buffers -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*
    As in MP8 no additional includes are required
    to complete the assignment, but you're welcome to use
    any that you think would help.
*/
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string>
#include <sstream>
#include <sys/time.h>
#include <assert.h>
#include <fstream>
#include <numeric>
#include <vector>
#include <signal.h>
#include <time.h>
#include "reqchannel.h"
#include "BoundedBuffer.h"
#include <chrono>

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/
typedef std::chrono::high_resolution_clock Clock;
 
struct Request {
    std::string patient; // Requester Name
    int n; // Number of Requests
    int w; // Number of Workers
    int r; // Number of Requesters
    BoundedBuffer* bounded_buffer; // Safe Buffer Address
};

struct Worker {
	//NetworkRequestChannel* channel; // Control Channel Address
	std::string host_name;
	std::string port_name;
	BoundedBuffer* request_buffer;	
	BoundedBuffer* response_john;
	BoundedBuffer* response_jane;
	BoundedBuffer* response_joe;
};

struct Statistic {
    std::string patient; // Requester Name
    int n; // Number of Requests per Person
	BoundedBuffer* response_buffer; // Response Buffer
	std::vector<int>* result_vector; // Result Vector
	pthread_mutex_t* vector_lock; // Protect Vector Uploads
};

struct Output {
	std::vector<int>* result_john; // Result Vector
	std::vector<int>* result_jane; // Result Vector
	std::vector<int>* result_joe; // Result Vector
	pthread_mutex_t* vector_lock; // Protect Vector Uploads
};

/*
    This class can be used to write to standard output
    in a multithreaded environment. It's primary purpose
    is printing debug messages while multiple threads
    are in execution. You probably saw the explanation of
    this in MP8, it hasn't changed since then so it
    won't be repeated.
 */
class atomic_standard_output {
    pthread_mutex_t console_lock;
public:
    atomic_standard_output() { pthread_mutex_init(&console_lock, NULL); }
    ~atomic_standard_output() { pthread_mutex_destroy(&console_lock); }
    void print(std::string s) {
        pthread_mutex_lock(&console_lock);
        std::cout << s << std::endl;
        pthread_mutex_unlock(&console_lock);
    }
};

atomic_standard_output threadsafe_standard_output;

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* HELPER FUNCTIONS */
/*--------------------------------------------------------------------------*/

std::string make_histogram_table(std::string name1, std::string name2,
                                 std::string name3, std::vector<int> *data1, std::vector<int> *data2,
                                 std::vector<int> *data3) {
    std::stringstream tablebuilder;
    tablebuilder << std::setw(25) << std::right << name1;
    tablebuilder << std::setw(15) << std::right << name2;
    tablebuilder << std::setw(15) << std::right << name3 << std::endl;
    for (int i = 0; i < data1->size(); ++i) {
        tablebuilder << std::setw(10) << std::left
                     << std::string(
                         std::to_string(i * 10) + "-"
                         + std::to_string((i * 10) + 9));
        tablebuilder << std::setw(15) << std::right
                     << std::to_string(data1->at(i));
        tablebuilder << std::setw(15) << std::right
                     << std::to_string(data2->at(i));
        tablebuilder << std::setw(15) << std::right
                     << std::to_string(data3->at(i)) << std::endl;
    }
    tablebuilder << std::setw(10) << std::left << "Total";
    tablebuilder << std::setw(15) << std::right
                 << accumulate(data1->begin(), data1->end(), 0);
    tablebuilder << std::setw(15) << std::right
                 << accumulate(data2->begin(), data2->end(), 0);
    tablebuilder << std::setw(15) << std::right
                 << accumulate(data3->begin(), data3->end(), 0) << std::endl;

    return tablebuilder.str();
}

/*
    You'll need to fill these in.
*/
void* request_thread_function(void* arg) {
    
	Request *r = (Request *)arg; // Collect Input Structure	 
	
	BoundedBuffer *s = (BoundedBuffer *) r->bounded_buffer;
	
	for(int i = 0; i < r->n; i++) {
		s->push_back("data "+r->patient);
		//sleep(1); // Statement checks that threads are parallel
	}
	// DEBUG
	//atomic_standard_output().println(r->patient);	
}

void* worker_thread_function(void* arg) {


	Worker *w = (Worker *)arg; // Collect Input Structure	 
  
    // Create Worker Channel
    NetworkRequestChannel *workerChannel = new NetworkRequestChannel(w->host_name, atoi((w->port_name).c_str()));

    // Request Buffer
    BoundedBuffer* request_buffer = (BoundedBuffer *) w->request_buffer;	
    
    // Response Buffer
	BoundedBuffer* response_john = (BoundedBuffer *) w->response_john;
    BoundedBuffer* response_jane = (BoundedBuffer *) w->response_jane;
    BoundedBuffer* response_joe = (BoundedBuffer *) w->response_joe;



    while(true) {
        std::string request = request_buffer->retrieve_front();
        std::string response = workerChannel->send_request(request);

		// DEBUG
        //std::string text = "---WORK---\n" + request + "\n" + response;
        //threadsafe_standard_output.print(text.c_str());		

		
        if(request == "data John Smith") {
            response_john->push_back(response);
        }
        else if(request == "data Jane Smith") {            
            response_jane->push_back(response);
        }
        else if(request == "data Joe Smith") {            
            response_joe->push_back(response);
        }
        else if(request == "quit") {
            delete workerChannel;
            break;
        }
    }



}

void* stat_thread_function(void* arg) {

	Statistic *s = (Statistic *)arg; // Collect Input Structure	 

    BoundedBuffer* response_buffer = (BoundedBuffer *) s->response_buffer;
    
    std::vector<int>* result_vector = (std::vector<int> *) s->result_vector;
    
    int i = 0;
        
    while(i < s->n) {
        
        std::string response = response_buffer->retrieve_front();
        
        pthread_mutex_lock(s->vector_lock);
        result_vector->at(stoi(response) / 10) += 1;
        pthread_mutex_unlock(s->vector_lock);
        
        
        // DEBUG
        //std::string text = "---STAT---\n" + s->patient + "\n" + response;
        //threadsafe_standard_output.print(text.c_str());
        
		i++;
    }   
    
    
    
}

// Screen Refresh Handler
void screen_refresh(int sig, siginfo_t *si, void *uc)
{
    
    
    Output* out = (Output*) si->si_value.sival_ptr;
    
    pthread_mutex_lock(out->vector_lock);
    
    std::string histogram_table = make_histogram_table("John Smith",
	    "Jane Smith", "Joe Smith", out->result_john,
	    out->result_jane, out->result_joe);

    pthread_mutex_unlock(out->vector_lock);

    system("clear");
    
    std::cout << histogram_table << std::endl;
    
    
    
    //signal(sig, SIG_IGN);
}


/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {
    int n = 10; //default number of requests per "patient"
    int b = 50; //default size of request_buffer
    int w = 10; //default number of worker threads
    string host_name=DEFAULT_HOST_NAME;
    string port_name=DEFAULT_PORT_NAME;
    bool USE_ALTERNATE_FILE_OUTPUT = false;
    int opt = 0;
    while ((opt = getopt(argc, argv, "n:b:w:h:p:m")) != -1) {
        switch (opt) {
        case 'n':
            n = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 'w':
            w = atoi(optarg);
            break;
        case 'm':
            USE_ALTERNATE_FILE_OUTPUT = true;
            break;
        case 'h':
        	host_name = optarg;
        	break;
        case 'p':
        	port_name = optarg;
        	break;
        default:
            std::cout << "This program can be invoked with the following flags:" << std::endl;
            std::cout << "-n [int]: number of requests per patient" << std::endl;
            std::cout << "-b [int]: maximum number of requests that will be allowed in the request buffer" << std::endl;
            std::cout << "-w [int]: number of worker threads" << std::endl;
            std::cout << "-m: use output2.txt instead of output.txt for all file output" << std::endl; //purely for convenience, you may find it helpful since you have to make two graphs instead of one, and they require different data
            std::cout << "-h: print this message and quit" << std::endl;
            std::cout << "Example: ./client_solution -n 10000 -b 50 -w 120 -m" << std::endl;
            std::cout << "If a given flag is not used, a default value will be given" << std::endl;
            std::cout << "to its corresponding variable. If an illegal option is detected," << std::endl;
            std::cout << "behavior is the same as using the -h flag." << std::endl;
            exit(0);
        }
    }
    

    struct timeval start_time;
    struct timeval finish_time;
    int64_t start_usecs;
    int64_t finish_usecs;
    
        std::ofstream ofs;
        if(USE_ALTERNATE_FILE_OUTPUT) ofs.open("output2.txt", std::ios::out | std::ios::app);
        else ofs.open("output.txt", std::ios::out | std::ios::app);
/*
        std::cout << "n == " << n << std::endl;
        std::cout << "b == " << b << std::endl;
        std::cout << "w == " << w << std::endl;
        std::cout << "host_name: " << host_name << std::endl;
        std::cout << "port_name: " << port_name << std::endl;

        std::cout << "CLIENT STARTED:" << std::endl;
*/         
        
        // Bounded Buffers
        BoundedBuffer request_buffer(b), response_john(b), response_jane(b), response_joe(b);

		// Histogram Container
        std::vector<int> john_frequency_count(10, 0);
        std::vector<int> jane_frequency_count(10, 0);
        std::vector<int> joe_frequency_count(10, 0);
        
        // Vector Lock
        pthread_mutex_t lock; 
		pthread_mutex_init(&lock,NULL);

        // Screen Refresh Signal Timer
        timer_t timerid;
        struct sigevent sev;
        struct itimerspec its;
        long long freq_nanosecs;
        sigset_t mask;
        struct sigaction sa;
        
        
        // Handler for Timer
        sa.sa_flags = SA_SIGINFO;
        sa.sa_sigaction = screen_refresh;
        sigemptyset(&sa.sa_mask);
        sigaction(SIGALRM,&sa,NULL);
        
        // Block Timer Signal
        sigemptyset(&mask);
        sigaddset(&mask,SIGALRM);
        sigprocmask(SIG_SETMASK,&mask,NULL);
        
        // Output Variable
        Output out;
        out.result_john = &john_frequency_count;
        out.result_jane = &jane_frequency_count;
        out.result_joe = &joe_frequency_count;
        out.vector_lock = &lock;
        
        // Create Timer
        sev.sigev_notify = SIGEV_SIGNAL;
        sev.sigev_signo = SIGALRM;
        sev.sigev_value.sival_ptr = &out;//timerid;
        timer_create(CLOCK_REALTIME, &sev, &timerid);
        
        freq_nanosecs = 2000000000; // 2 Second Timer
        its.it_value.tv_sec = freq_nanosecs / 1000000000;
        its.it_value.tv_nsec = freq_nanosecs % 1000000000;
        its.it_interval.tv_sec = its.it_value.tv_sec;
        its.it_interval.tv_nsec = its.it_value.tv_nsec;
        
        
        // ENABLE/DISABLE REFRESH HERE
        timer_settime(timerid, 0, &its, NULL);
        
        sigprocmask(SIG_UNBLOCK,&mask, NULL);
        
		/*-------------------------------------------*/
		/* START TIMER HERE */
		/*-------------------------------------------*/		
		auto timer1 = Clock::now();

        /////////////////////
        // Request Threads //
        /////////////////////
        
		// Requester Information
		Request r1, r2, r3;
		
		// John Smith
		r1.patient = "John Smith";        // Patient
		r1.n = n;                         // Number of Requests
		r1.w = w;                         // Number of Workers
		r1.r = 3;                         // Number of Requesters
		r1.bounded_buffer = &request_buffer; // BoundedBuffer Address
		// Jane Smith
		r2.patient = "Jane Smith";
		r2.n = n;
		r2.w = w;
		r2.r = 3;
		r2.bounded_buffer = &request_buffer;
		// Joe Smith
		r3.patient = "Joe Smith";
		r3.n = n;
		r3.w = w;
		r3.r = 3;
		r3.bounded_buffer = &request_buffer;        
        
        // Initiate Request Threads
        //std::cout << "Initiating Request Threads" << std::endl;
        fflush(NULL);
		
		pthread_t t1, t2, t3;
		
		// Create Threads
		pthread_create(&t1, NULL, request_thread_function, &r1);
		pthread_create(&t2, NULL, request_thread_function, &r2);
		pthread_create(&t3, NULL, request_thread_function, &r3); 
		
		////////////////////
		// Worker Threads //
		////////////////////
		
		// Initiating Worker Threads
        //std::cout << "Initiating Worker Threads" << std::endl;

		pthread_t worker_thread[w];

		// Standard Info for Worker Channels
		Worker worker_info;
		worker_info.host_name = host_name;
		worker_info.port_name = port_name;
		worker_info.request_buffer = &request_buffer; // Request Buffer
		worker_info.response_john = &response_john;   // Response Buffers
		worker_info.response_jane = &response_jane;
		worker_info.response_joe = &response_joe;

        // Create Threads
		for(int i = 0; i < w; ++i) {
			pthread_create(&worker_thread[i], NULL, worker_thread_function, &worker_info);
		}	
		
		
		///////////////////////
		// Statistic Threads //
		///////////////////////
		
		// Statistic Information
		Statistic john_stat, jane_stat, joe_stat;
		
		// John Smith
		john_stat.patient = "John Smith";          // Patient
		john_stat.n = n;
		john_stat.response_buffer = &response_john; // BoundedBuffer Address
		john_stat.result_vector = &john_frequency_count;
		john_stat.vector_lock = &lock;
		// Jane Smith
		jane_stat.patient = "Jane Smith";
		jane_stat.n = n;
		jane_stat.response_buffer = &response_jane;
		jane_stat.result_vector = &jane_frequency_count;
		jane_stat.vector_lock = &lock;
		// Joe Smith
		joe_stat.patient = "Joe Smith";
		joe_stat.n = n;
		joe_stat.response_buffer = &response_joe;  		
		joe_stat.result_vector = &joe_frequency_count;
		joe_stat.vector_lock = &lock;
		
		
        // Initiate Request Threads
        //std::cout << "Initiating Statistic Threads" << std::endl;
        fflush(NULL);
		
		pthread_t john_stat_t, jane_stat_t, joe_stat_t;
		
		// Create Threads
		pthread_create(&john_stat_t, NULL, stat_thread_function, &john_stat);
		pthread_create(&jane_stat_t, NULL, stat_thread_function, &jane_stat);
		pthread_create(&joe_stat_t,  NULL, stat_thread_function, &joe_stat); 		
		
		
		
		
		//////////////////////
		// JOIN ALL THREADS //
		//////////////////////
		
		// Requests
		pthread_join(t1,NULL);
		pthread_join(t2,NULL);
		pthread_join(t3,NULL);

		// Quit Requests
		for(int i = 0; i < w; ++i) request_buffer.push_back("quit");
		
		// Workers
		for(int i = 0; i < w; ++i) pthread_join(worker_thread[i], NULL);
		
		// Statistics
		pthread_join(john_stat_t, NULL);
		pthread_join(jane_stat_t, NULL);
		pthread_join(joe_stat_t, NULL);
		
		// DEBUG
		//while(request_buffer.size()>0) printf("%s\n",request_buffer.retrieve_front().c_str());
		
		// Destory Lock
		pthread_mutex_destroy(&lock);


		
        /*-------------------------------------------*/
        /* END TIMER HERE   */
        /*-------------------------------------------*/
        auto timer2 = Clock::now();		
		
        /////////////
        // RESULTS //
        /////////////

        
        

        system("clear");
		std::string histogram_table = make_histogram_table("John Smith",
		        "Jane Smith", "Joe Smith", &john_frequency_count,
		        &jane_frequency_count, &joe_frequency_count);

        std::cout << "Results for n == " << n << ", w == " << w << std::endl;
        
        std::cout << histogram_table << std::endl;
        
        // Delete Timer
        timer_delete(timerid);        

        // TIMER
		ofs << std::chrono::duration_cast<std::chrono::nanoseconds>(timer2-timer1).count() << " nanoseconds" << std::endl;        





        ofs.close();
        usleep(10000);

    
    
    
    // DEBUG
    /*
    int pid = fork();
    NetworkRequestChannel client1(host_name, atoi(port_name.c_str()));
    string test;

    if(pid){
        sleep(3);
        for(int i = 0; i < 5; i++)
        {
            test = client1.send_request("data");
            printf("%s\n",test.c_str());
            sleep(1);
            client1.send_request("quit");
        }   
    }
    else
    {
        for(;;)
        {
            test = client1.send_request("data");
            printf("%s\n",test.c_str());
            sleep(1);
        }    
    }
    */


}
