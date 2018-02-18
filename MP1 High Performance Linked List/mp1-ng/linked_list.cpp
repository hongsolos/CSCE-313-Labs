/* --------------------------------------------------------------------------- */
/* Developer: Andrew Kirfman, Margaret Baxter                                  */
/* Project: CSCE-313 Machine Problem #1                                        */
/*                                                                             */
/* File: ./MP1/linked_list.cpp                                                 */
/* --------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------- */
/* User Defined Includes                                                       */
/* --------------------------------------------------------------------------- */

#include "linked_list.h"


/* Constructor */
linked_list::linked_list()
{
	mem_size = 0;
	block_size = 0;
	max_data_size = 0;
	
	head_pointer = NULL;
	front_pointer = NULL;
	free_pointer = NULL; 
	free_data_pointer = NULL;
	
	initialized = false;
}


void linked_list::Init(int M, int b)
{
	mem_size = M;
	block_size = b;
	max_data_size = M/b;
	
	//Allocate M bytes to create the memory pool
	head_pointer = 	    (char*)malloc(M);
	front_pointer =		(struct node*)head_pointer;
	free_pointer =		front_pointer;
	free_data_pointer = front_pointer;
	
	
	// Initialize Pointers in List
	struct node* current_node = front_pointer;
	for(int i = 0; i < max_data_size; i++){
		current_node->next = (struct node*) ((char*)current_node + block_size); // Advance by Bytes
		current_node = current_node->next;
	}
	current_node->next = NULL;
	
	initialized = true;
	
	
}

void linked_list::Destroy()
{
	free(head_pointer);
	head_pointer = NULL;
	free_pointer = NULL;
	free_data_pointer = NULL;
	mem_size = 0;
	block_size = 0;
	max_data_size = 0;
	
	initialized = false;
} 

/* Insert an element into the list with a given key, given data element, and with a given length*/
void linked_list::Insert (int k, char * data_ptr, int data_len)
{
	// Check input data size against block size
	if( data_len + sizeof(node) > block_size ){
		printf("\nERROR with Insert(%d,ptr,%d): Input Exceeds Block Size\n\n",k,data_len);
		return;	
	}
	else if((unsigned long int)(free_data_pointer) + block_size > (unsigned long int)(head_pointer) + mem_size){ 
			printf("\nERROR with Insert(%d,ptr,%d): Input Exceeds Preallocated Memory\n\n",k,data_len);
			return;
	}
	
	// Use preallocated space starting at free_data_pointer
	struct node* new_node = (struct node*) free_data_pointer; 
	
	// Advance free_pointer by block_size
	//free_pointer = (struct node*) ((char*)free_pointer	+ block_size); // Advance by Bytes
	free_pointer = new_node;
	free_data_pointer = new_node->next;
	
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

int linked_list::Delete(int delete_key){
	//Assign next_node to run ahead of currentn_node, so the target
	//can be found one node ahead, and assign the pointer to skip that target node
	struct node* prev_node;
	struct node* current_node = front_pointer;

	// Special Case for Front Node
	if(current_node->key == delete_key){

		free_pointer->next = current_node;
				
		front_pointer = current_node->next;
		
		current_node->next = free_data_pointer;
		free_data_pointer = current_node;

		return delete_key;
	}
	else{
		prev_node = current_node;
		current_node = current_node->next;
		
		// Middle Nodes
		int index = 0; //Location of deletion
		while(current_node != free_data_pointer){
			if(current_node->key ==delete_key){
				
				free_pointer->next = current_node;
				
				prev_node->next = current_node->next;
				
				current_node->next = free_data_pointer;
				free_data_pointer = current_node;
				
				return delete_key;
			}
			index++;
			prev_node = current_node;
			current_node = current_node->next;
		}
		
		// End Node
		/*
		if(current_node->key == delete_key){
			prev_node->next = NULL;
			free_pointer = current_node;
			return delete_key;
		}
		*/
	}	
	return 0;		
}

/* Iterate through the list, if a given key exists, return the pointer to it's node */
/* otherwise, return NULL                                                           */
struct node* linked_list::Lookup(int lookup_key)
{
	struct node* current_node = front_pointer;
	
	// Check 0 through END-1 Nodes
	while (current_node != free_data_pointer){
		if (current_node->key == lookup_key)
			return current_node;
		current_node = current_node->next;
	}
	
	// Check END Node
	//if (current_node->key == lookup_key)
	//	return current_node;
		
	return NULL;
}

/* Prints the list by printing the key and the data of each node */ 
void linked_list::PrintList()
{
	struct node* current_node = front_pointer; // Start at front_pointer
	
	while(current_node != free_data_pointer){
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
		
		
		printf("current_node: %p\n",(void*)current_node);
		printf("current_node->next: %p\n",(void*)current_node->next);		

		// Jump to Next Node
		current_node = current_node->next;
	}
}

/* Getter Functions */
char* linked_list::getHeadPointer()
{
	return head_pointer;
}

node* linked_list::getFrontPointer()
{
	return front_pointer;
}

node* linked_list::getFreePointer()
{
	return free_pointer;
}

node* linked_list::getFreeDataPointer()
{
	return free_data_pointer;
}

int linked_list::getBlockSize()
{	
	return block_size;
}

int linked_list::getMemSize()
{
	return mem_size;
}

int linked_list::getMaxDataSize()
{
	return max_data_size;
}

bool linked_list::getInitialized()
{
	return initialized;
}

/* Setter Functions */
void linked_list::setHeadPointer(char *new_pointer)
{
	head_pointer = new_pointer;
}

void linked_list::setFrontPointer(node* new_pointer)
{
	front_pointer = new_pointer;
}

void linked_list::setFreePointer(node* new_pointer)
{
	free_pointer = new_pointer;
}

void linked_list::setFreeDataPointer(node* new_pointer)
{
	free_data_pointer = new_pointer;
}

void linked_list::setBlockSize(int new_block_size)
{
	block_size = new_block_size;
}

void linked_list::setMemSize(int new_mem_size)
{
	mem_size = new_mem_size;
}

void linked_list::setMaxDataSize(int new_max_data_size)
{
	max_data_size = new_max_data_size;
}

void linked_list::setInitialized(bool new_initialized)
{
	initialized = new_initialized;
}
