/* -------------------------------------------------------------------------- */
/* Developer: Andrew Kirfman                                                  */
/* Project: CSCE-313 Machine Problem #3                                       */
/*                                                                            */
/* File: ./mp3_part6.cpp                                                      */
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
	    close(pipe_fds[0]); // Remove Read Pipe
	    
	    dup2 (pipe_fds[1],STDOUT_FILENO); // Connect STDOUT to Write Pipe
	    close(pipe_fds[1]); // Remove Write Pipe
	    
	    execl("/bin/ls","/bin/ls","-la",(char*)NULL);
	    
	}
	else // Parent
	{
		wait(NULL);
		
		// printf("In Parent");

	    close(pipe_fds[1]); // Remove Write Pipe
	    
	    int nbytes = read(pipe_fds[0], buf, sizeof(buf));
	    
	    write(STDOUT_FILENO, buf, nbytes);
	    
	    close(pipe_fds[0]); // Remove Read Pipe
	}
    exit(0);
}

