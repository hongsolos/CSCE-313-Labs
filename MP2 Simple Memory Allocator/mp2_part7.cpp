/* -------------------------------------------------------------------------- */
/* Developer: Andrew Kirfman                                                  */
/* Project: CSCE-313 Machine Problem #3                                       */
/*                                                                            */
/* File: ./mp3_part7.cpp                                                      */
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
    char* cmd1[] = { "/bin/ls", "-la", 0 };
    char* cmd2[] = { "/usr/bin/tr", "a-zA-Z0-9", "a", 0 };

    int pipe_fds[2]; // 0: Read
                     // 1: Write
                     
    char buf[4096];
    
    if (pipe(pipe_fds) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

	pid_t pid = fork();
	
	if(pid == 0) // Child
	{
	    //printf("In Child");
	    close(pipe_fds[0]);

	    dup2(pipe_fds[1], 1);
	    
	    close(pipe_fds[1]);
	    
	    execvp(cmd1[0],cmd1);
	    
	}
	else // Parent
	{
		wait(NULL);
		
		// printf("In Parent");
        close(pipe_fds[1]);

        dup2(pipe_fds[0], 0);
        
        close(pipe_fds[0]);
        
        execvp(cmd2[0],cmd2);
	}
	
    exit(0);
}

