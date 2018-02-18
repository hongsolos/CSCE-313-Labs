/* -------------------------------------------------------------------------- */
/* Developer: Andrew Kirfman                                                  */
/* Project: CSCE-313 Machine Problem #3                                       */
/*                                                                            */
/* File: ./mp3_part5.cpp                                                      */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Standard Library Includes                                                  */
/* -------------------------------------------------------------------------- */

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <iostream>


int main()
{
    // Parent Print World
    // Child Print Hello
    // use wait
	pid_t pid = fork();
	if(pid == 0)
	{
    	printf("Hello");
	}
	else
	{
		wait(NULL);
	    printf("World!\n");
	}
	exit (0);
}

