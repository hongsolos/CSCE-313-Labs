/* 
    File: my_allocator.c

    Author: <your name>
            Department of Computer Science
            Texas A&M University
    Date  : <date>

    Modified: 

    This file contains the implementation of the module "MY_ALLOCATOR".

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdexcept>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <cmath>
#include <iostream>
#include "my_allocator.h"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/
struct node {
    struct node *next;
    int size;
    bool is_full;
};

struct node** free_list;
/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/
int block_size;
int length;
int free_list_size;
Addr starting_address;
int counting = 0;

Addr buddy;
int buddy_size;

/*--------------------------------------------------------------------------*/
/* FUNCTIONS FOR MODULE MY_ALLOCATOR */
/*--------------------------------------------------------------------------*/

/* Don't forget to implement "init_allocator" and "release_allocator"! */
unsigned int init_allocator(unsigned int _basic_block_size, unsigned int _length){
    //Initializing constants
    block_size = _basic_block_size;
    length = _length;
    //printf ("%i",block_size);
    //ERROR BLOCK -----------------------------------------/
    //Check if block_size is smaller than Memory
    if (block_size > length){
        return 0;
    }
    
    //Check if block size is power of 2, if not make it
    //block_size = next_pow2(block_size);
    
    //Check if length is power of 2, if not make it
    length = next_pow2(length);
    printf("%d ", length);
    
    //Create the free_list --------------------------------/
    //Obtain start address
    starting_address = (char*)malloc(length);
    
    //Calculate the number of blocks

    free_list_size = (int) (log2(length / block_size)) + 1;
    printf ("%i ",free_list_size);
    //Free list creation
    struct node* header = (struct node*) starting_address;
    header->size = length;
    header->is_full = false;
    header->next = NULL;

    free_list = new struct node*[length] ;
    free_list[length] = header;
    
    int len = length;
    for (int i = length/2; i >= block_size; i /= 2){
        len = len /2;
        // struct node* headers = (struct node*) (starting_address + len);
        // headers->size = len;
        // headers->is_full = false;
        // headers->next = NULL;
        free_list[i] = NULL;
    }
    
    print_list();
    return 1;
}

int release_allocator(){
    block_size = 0;
	length = 0; 
	free(starting_address); 
}

Addr my_malloc(unsigned int _length) {
    int target_block = next_pow2(_length + sizeof (struct node));
    
    if (target_block < block_size)
        target_block = block_size;
        
    if (target_block > length){
        printf("DOESNT FIT");
        return NULL;
    }
    //printf("%i", target_block);
    struct node* start = NULL;
    int sta = target_block;
    //start = free_list[sta];
    while (start == NULL && sta <= length){
        //struct node* vim = (struct node*) starting_address + target_block;
        //free_list[sta] = vim;
        start = free_list[sta];
        sta = sta * 2;
    }
    
    if (start == NULL){
        //printf ("BLOCK NOT FOUND");
        return NULL;
    }
    //printf ("Passed check");
    
    while (start->size > target_block){
        split_block(start);
    }
    
    deleted(start);
    start->is_full = true;

    return (Addr) (start + 1);

    //return malloc((size_t)_length);
}

void print_list(){
    printf ("Free List: \n");
    for (int i = length; i >= block_size; i /= 2){
        printf("%i >> ", i);
        int num = 0;
        struct node* tr_start = free_list[i];
        while (tr_start != NULL){
            num++;
            //printf("( %c )", (char*) tr_start);
            printf("\nSize: %i\n", tr_start->size);
            tr_start = tr_start->next;
        }
        printf ("Number of nodes: %i\n", num);
    }
}

Addr buddy_address(Addr block_address){ //FLAGED
    struct node* b = (struct node*) block_address;
    //int b_size = b->size/2;
    Addr target = (Addr) ((char*)block_address - (char*)starting_address); //*****
    Addr buddy = (Addr) ( ((long)target) ^ block_size);
    return (Addr) ( ((long)buddy) + ((long)starting_address) );
}

void deleted (struct node* bloc){
    int index = bloc->size;
    struct node* hea = free_list[index];
    
    if (hea == bloc){
        free_list[index] = bloc->next;
    } else {
        while (hea->next != bloc){
            hea = hea->next;
        }
        hea->next = bloc->next;
    }
    //free_list_size = free_list_size - 1;
    //printf ("Deleted");
}

int next_pow2 (int num){
    bool check = true;
    while (check){
        int x = num;
        while (((x % 2) == 0) && x > 1) /* While x is even and > 1 */
            x /= 2;
        if (x == 1){
            check = false;
            return num;
        }
        num++;
    }
}

bool split_block(struct node* addr_1){
    deleted(addr_1);
    
    //printf("Addr_1: %i", addr_1->size);
    struct node* addr_2 = (struct node*) ((char*)addr_1 + (addr_1->size /2));
    addr_1->size /= 2;
    addr_2->size = addr_1->size;
    addr_2->is_full = false;
    addr_1->is_full = false;
    
    int index1 = addr_1->size;
    addr_1->next = free_list[index1];
    free_list[index1] = addr_1;
    
    int index2 = addr_2->size;
    addr_2->next = free_list[index2];
    free_list[index2] = addr_2;
    
    return true;
}

bool combine_blocks(Addr start_address1, Addr start_address2){
    struct node* combine1 = (struct node*) start_address1;
    struct node* combine2 = (struct node*) start_address2;
    
    deleted(combine1);
    deleted(combine2);
    
    struct node* heading;
    if (combine1 < combine2){
        heading = combine1;
    } else {
        heading = combine2;
    }
    heading->size *= 2;
    
    int index = heading->size;
    heading->next = free_list[index];
    free_list[index] = heading;
}

int my_free(Addr _a) {
    //Get address for the block to be free
    struct node* head = (struct node*)((char*)_a - sizeof(struct node));
    head->is_full = false;
    
    int index = head->size;
    head->next = free_list[index];
    free_list[index] = head;

    //Empty it and then consolidate with the rest
    while (head->size < length){
        struct node* comrade = (struct node*) (buddy_address ((Addr)head));
        if (!comrade->is_full && comrade->size == head->size){
            combine_blocks((Addr) head, (Addr) comrade);
        } else {
            break;
        }
    }
    return 0;
}