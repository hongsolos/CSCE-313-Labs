//
//  BoundedBuffer.cpp
//
//
//  Created by Joshua Higginbotham on 11/4/15.
//
//

#include "BoundedBuffer.h"

BoundedBuffer::BoundedBuffer(int max_size) {
	// Initialize the buffer

	// Initialize the mutex
	pthread_mutex_init(&buffer_lock, NULL);

    // Initialize the semaphore
    sem_init(&empty, 0, max_size);
    sem_init(&full,  0, 0);
}

BoundedBuffer::~BoundedBuffer() {
	// Destroy mutex
	pthread_mutex_destroy(&buffer_lock);
	
    // Destroy semaphore
    sem_destroy(&empty);
	sem_destroy(&full);
	
	// Destroy queue
	std::queue<std::string> empty_string;
    std::swap(bounded_buffer_queue, empty_string);
}

int BoundedBuffer::size() {
    //DEBUG--
    //printf("%i", safe_buffer_queue.size());
    
    //-------
    return bounded_buffer_queue.size();
}

void BoundedBuffer::push_back(std::string str) {
    
    // PRODUCER
    
    // DEBUG
    //int value;
    //sem_getvalue(&empty, &value);
    //printf("empty: %d\n",value);
    //sem_getvalue(&full,  &value);
    //printf("full:  %d\n",value);
    
    
    // Decrease the counter of empty slots
    sem_wait(&empty);
    
    //Locking
    pthread_mutex_lock(&buffer_lock);
    
    //Put it in the buffer queue
    bounded_buffer_queue.push(str);
    
    //DEBUG--
    //printf ("%s", safe_buffer_queue.back().c_str());
    
    //Unlocking
    pthread_mutex_unlock(&buffer_lock);
    
    // Increase the counter of items
    sem_post(&full);
    
    //-------
	return;
}

std::string BoundedBuffer::retrieve_front() {
    
    // CONSUMER
    
    // DEBUG
    //int value;
    //sem_getvalue(&empty, &value);
    //printf("empty: %d\n",value);
    //sem_getvalue(&full,  &value);
    //printf("full:  %d\n",value);    
    
    // Decrease the counter of items
    sem_wait(&full);
    
    //Locking
    pthread_mutex_lock(&buffer_lock);
    
    //Retrieve
    std::string result = bounded_buffer_queue.front().c_str();
    bounded_buffer_queue.pop();
    
    //DEBUG--
    //printf ("%s", result.c_str());
    
    //Unlocking
    pthread_mutex_unlock(&buffer_lock);
    
    // Increase the counter of empty slots
    sem_post(&empty);
    
    //-------
	return result;
}
