/* --------------------------------------------------------------------------- */
/* Developer: Nathaniel Glasser, Han Hong                                      */
/* Project: CSCE-313 Machine Problem #1                                        */
/*                                                                             */
/* File: ./MP1/linked_list2.cpp                                                */
/* --------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------- */
/* Standard Library Includes                                                   */
/* --------------------------------------------------------------------------- */

#include <iostream>
#include <string.h>
#include <climits>
#include <math.h>
/* --------------------------------------------------------------------------- */
/* User Defined Includes                                                       */
/* --------------------------------------------------------------------------- */

#include "linked_list2.h"

linked_list2::linked_list2()
{
	mem_size = 0;
	block_size = 0;
	max_data_size = 0;
	num_tiers = 0;
	
	head_pointer = NULL;
	front_pointer = NULL;
	free_pointer = NULL; 
	free_data_pointer = NULL;
	
	initialized = false;
}

void linked_list2::Init(int M, int b, int t)
{
	mem_size = M;
	block_size = b;
	max_data_size = M/b;
	num_tiers = t;
	
	//Allocate M bytes to create the memory pool
	//The pool is divided tiers, each has M/t bytes
	head_pointer = (char**)malloc(t*sizeof(char*));
	front_pointer = new node*[t];
	free_pointer = new node*[t];
	free_data_pointer = new node*[t];
	for (int i = 0; i < num_tiers; i++){
		head_pointer[i] = (char*)malloc(M/t);
		front_pointer[i] = (struct node*) head_pointer[i];
		free_pointer[i] = front_pointer[i];
		free_data_pointer[i] = front_pointer[i];
	}
	
	//Initialize pointers in the List
	for (int i = 0; i < t; i++){
		struct node* current_node = front_pointer[i];
		for (int j = 0; j < max_data_size/t; j++){
			current_node->next = (struct node*) ((char*)current_node + block_size); // Advance by Bytes
			current_node = current_node->next;
		}
		current_node->next = NULL;
	}
	
	initialized = true;
}


void linked_list2::Destroy()
{
	//Iterate through the list, set everything back to NULL and free the each tier
	for (int i = 0; i < num_tiers; i++){
		free(head_pointer[i]);
		front_pointer[i] = NULL;
		free_pointer[i] = NULL;
		free_data_pointer[i] = NULL;
	}
	
	free(head_pointer);
	
	mem_size = 0;
	block_size = 0;
	max_data_size = 0;
	
	initialized = false;
}

void linked_list2::Insert(int k,char * data_ptr, int data_len)
{
	// Check input data size against block size
	if( data_len + sizeof(node) > block_size ){
		printf("\nERROR with Insert(%d,ptr,%d): Input Exceeds Block Size\n\n",k,data_len);
		return;	
	}
	//Determines the tier this node belong to
	int tier = Find_tier(k);
	
	// Check if that tier is full
	if((unsigned long int)(free_data_pointer[tier]) + block_size > (unsigned long int)(head_pointer[tier]) + mem_size){ 
			printf("\nERROR with Insert(%d,ptr,%d): Input Exceeds Preallocated Memory in This Tier\n\n",k,data_len);
			return;
	}
	
	// Use preallocated space starting at free_data_pointer
	struct node* new_node = (struct node*) free_data_pointer[tier]; 
	
	// Advance free_pointer by block_size
	//free_pointer = (struct node*) ((char*)free_pointer	+ block_size); // Advance by Bytes
	free_pointer[tier] = new_node;
	free_data_pointer[tier] = new_node->next;
	
	// Populate node data fields
	//new_node->next = free_pointer; 
	new_node->key = k; 
	new_node->value_len = data_len;
	
	//free_pointer->next = NULL;
	
	// Advance new_node pointer by sizeof(node*) to highlight data field
	new_node += 1; 
	
	// Copy external data into data field
	memcpy(new_node, data_ptr, data_len);
	
}

int linked_list2::Delete(int delete_key)
{
	//Determines the tier this key belong to
	int tier = Find_tier(delete_key);
	
	//Assign next_node to run ahead of currentn_node, so the target
	//can be found one node ahead, and assign the pointer to skip that target node
	struct node* prev_node;
	struct node* current_node = front_pointer[tier];

	// Special Case for Front Node
	if(current_node->key == delete_key){

		free_pointer[tier]->next = current_node;
				
		front_pointer[tier] = current_node->next;
		
		current_node->next = free_data_pointer[tier];
		free_data_pointer[tier] = current_node;

		return delete_key;
	}
	else{
		prev_node = current_node;
		current_node = current_node->next;
		
		// Middle Nodes
		int index = 0; //Location of deletion
		while(current_node != free_data_pointer[tier]){
			if(current_node->key == delete_key){
				
				free_pointer[tier]->next = current_node;
				
				prev_node->next = current_node->next;
				
				current_node->next = free_data_pointer[tier];
				free_data_pointer[tier] = current_node;
				
				return delete_key;
			}
			index++;
			prev_node = current_node;
			current_node = current_node->next;
		}
	}	
	return 0;
}

node* linked_list2::Lookup(int lookup_key)
{
	//Determines the tier this key belong to
	int tier = Find_tier(lookup_key);
	
	struct node* current_node = front_pointer[tier];
	
	// Check 0 through END-1 Nodes
	while (current_node != free_data_pointer[tier]){
		if (current_node->key == lookup_key)
			return current_node;
		current_node = current_node->next;
	}
		
	return NULL;
}

void linked_list2::PrintList()
{
	for (int i = 0; i < num_tiers; i++){
		if (free_data_pointer[i] != front_pointer[i]){
			std::cout << "Tier " << i << std::endl;
			struct node* current_node = front_pointer[i]; // Start at front_pointer
			
			while(current_node != free_data_pointer[i]){
				char* data_field = (char*) ((struct node*)current_node + 1);
				
				char A[current_node->value_len];
				memcpy((void*)A,(void*)data_field,current_node->value_len);
				
				// Print Current Node		
				std::cout << "Node: " << std::endl;
				std::cout << " - Key: " << current_node->key << std::endl;
				std::cout << " - Data: ";
				
				for(char c : A){
					//std::cout << std::hex << c;
					printf("%x", c);
				}
				std::cout << std::endl;
		
				// Jump to Next Node
				current_node = current_node->next;
			}
		}
	}
}

int linked_list2::Find_tier(int key)
{
	//Determines the tier this key belong to
	int tier = 1;
	while (key > tier*(floor(INT_MAX/num_tiers))){
		tier++;
	}
	tier--;
	return tier;
}

/* Getter Functions */
char** linked_list2::getHeadPointer()
{
	return head_pointer;
}

node** linked_list2::getFrontPointer()
{
	return front_pointer;
}

node** linked_list2::getFreePointer()
{
	return free_pointer;
}

node** linked_list2::getFreeDataPointer()
{
	return free_data_pointer;
}

int linked_list2::getBlockSize()
{
	return block_size;
}

int linked_list2::getMemSize()
{	
	return mem_size;
}

int linked_list2::getMaxDataSize()
{
	return max_data_size;
}

int linked_list2::getNumTiers()
{
	return num_tiers;
}

int linked_list2::getInitialized()
{
	if (initialized == true)
		return 1;
	return 0;
}

/* Setter Functions */
void linked_list2::setHeadPointer(char** new_head_pointer)
{
	head_pointer = new_head_pointer;
}

void linked_list2::setFrontPointer(node** new_front_pointer)
{
	front_pointer = new_front_pointer;
}

void linked_list2::setFreePointer(node** new_free_pointer)
{
	free_pointer = new_free_pointer;
}

void linked_list2::setFreeDataPointer(node** new_free_data_pointer)
{	
	free_data_pointer = new_free_data_pointer;
}

void linked_list2::setBlockSize(int new_block_size)
{
	block_size = new_block_size;
}

void linked_list2::setMemSize(int new_mem_size)
{
	mem_size = new_mem_size;
}

void linked_list2::setMaxDataSize(int new_max_data_size)
{
	max_data_size = new_max_data_size;
}

void linked_list2::setNumTiers(int new_num_tiers)
{
	num_tiers = new_num_tiers;
}

void linked_list2::setInitialized(bool new_initialized)
{
	initialized = new_initialized;
}
