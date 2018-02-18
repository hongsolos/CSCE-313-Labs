//
//  SafeBuffer.cpp
//
//
//  Created by Joshua Higginbotham on 11/4/15.
//
//

#include "SafeBuffer.h"
#include <string>
#include <queue>

SafeBuffer::SafeBuffer() {
	// Initialize the buffer

	// Initialize the mutex
	pthread_mutex_init(&buffer_lock, NULL);
}

SafeBuffer::~SafeBuffer() {
	// Destroy mutex
	pthread_mutex_destroy(&buffer_lock);
	
	// Destroy queue
	std::queue<std::string> empty;
    std::swap(safe_buffer_queue, empty);
}

int SafeBuffer::size() {
    //DEBUG--
    //printf("%i", safe_buffer_queue.size());
    
    //-------
    return safe_buffer_queue.size();
}

void SafeBuffer::push_back(std::string str) {
    //Locking
    pthread_mutex_lock(&buffer_lock);
    
    //Put it in the buffer queue
    safe_buffer_queue.push(str);
    
    //DEBUG--
    //printf ("%s", safe_buffer_queue.back().c_str());
    
    //Unlocking
    pthread_mutex_unlock(&buffer_lock);
    
    //-------
	return;
}

std::string SafeBuffer::retrieve_front() {
    //Locking
    pthread_mutex_lock(&buffer_lock);
    
    //Retrieve
    std::string result = safe_buffer_queue.front().c_str();
    safe_buffer_queue.pop();
    
    //DEBUG--
    //printf ("%s", result.c_str());
    
    //Unlocking
    pthread_mutex_unlock(&buffer_lock);
    
    //-------
	return result;
}
