#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <ctype.h>
#include <unistd.h>
#include "ackerman.h"
#include "my_allocator.h"

int main(int argc, char ** argv) {

  // input parameters (basic block size, memory length)
  int opt = 0;
  int block = 128;
  int memory_length = 524288;
  
//   while ((opt = getopt (argc, argv, "b:s:")) != -1){
//     switch (opt){
// 			  case 'b':
// 				  block = atoi(optarg);
// 				  break;
// 			  case 's':
// 				  memory_length = atoi(optarg);
// 				  break;
// 		}
// 	}
// 	printf("s=%i,b=%i\n",memory_length,block);
  // init_allocator(basic block size, memory length)
  int i = init_allocator (block, memory_length);
  printf("Display Code: %i\n", i);
  ackerman_main();

  // release_allocator()
  release_allocator();
}
