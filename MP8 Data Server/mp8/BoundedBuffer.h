//
//  BoundedBuffer.h
//  
//
//  Created by Joshua Higginbotham on 11/4/15.
//
//

#ifndef BoundedBuffer_h
#define BoundedBuffer_h

#include <stdio.h>
#include <queue>
#include <string>
#include <pthread.h>
#include <semaphore.h>


class BoundedBuffer {
	/*
		Only two data members are really necessary for this class:
		a mutex, and a data structure to hold elements. Recall
		that BoundedBuffer is supposed to be FIFO, so something
		like std::vector will adversely affect performance if
		used here. We recommend something like std::list
		or std::queue, because std::vector is very inefficient when
		being modified from the front.
	*/
	std::queue<std::string> bounded_buffer_queue; // FIFO, holding requests (string)
	pthread_mutex_t buffer_lock; // Control access to the buffer
	sem_t empty, full; // Semaphor

public:
    BoundedBuffer(int);
	~BoundedBuffer();
	int size();
    void push_back(std::string str);
    std::string retrieve_front();
};

#endif /* BoundedBuffer_ */
