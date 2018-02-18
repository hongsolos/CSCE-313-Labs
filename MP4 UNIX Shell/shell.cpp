/* ------------------------------------------------------------------------- */
/* Developer: Andrew Kirfman                                                 */ 
/* Project: CSCE-313 Machine Problem #4                                              */
/*                                                                           */
/* File: ./shell.cpp                                                         */ 
/* ------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------- */
/* Standard Libaray Includes                                                 */
/* ------------------------------------------------------------------------- */

#include<iostream>
#include<cstdlib>
#include<ctime>
#include<unistd.h>
#include<stdlib.h>
#include<regex>
#include<string>
#include<vector>
#include<stdio.h>
#include<string.h>
#include<string>
#include<sstream>
#include<sys/wait.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

/* ------------------------------------------------------------------------- */
/* Global Variables                                                          */
/* ------------------------------------------------------------------------- */

// Put any global variables here
char buf[1024];
std::string raw;
void execute_vector(std::vector<std::string>);

//char user_custom_promt;
/* User Definable Custom Prompt:
 * 
 * Allows users to define custom prompts using a simple command to change the string.  
 * The following set of macros will be replaced as follows:
 *
 *   - DIR: Current directory 
 *   - WHO: User's name
 *   - DATE: Current date
 *   - TIME: Current time
 */
void update_prompt()
{	
	char* reset[] = {"pwd", NULL};
	if (fork() == 0){
		execvp(reset[0], reset);
	} else {
		wait(NULL);
	}
}

std::vector<std::string> split_on_spaces(std::string input)
{
    std::vector<std::string> components;
    std::string token = "";

    for(unsigned short int i=0; i<input.length(); i++)
    {
        if(input[i] != ' ')
        {
            token += input[i];
        }
        else
        {
            components.push_back(token);
            token = "";
        }
    }
    components.push_back(token);
    return components;
}

std::vector<std::string> split_on_pipe(std::string input)
{
    std::vector<std::string> components;
    std::string token = "";

    for(unsigned short int i=0; i<input.length(); i++)
    {
        if(input[i] != '|')
        {
            token += input[i];
        }
        else
        {
            components.push_back(token);
            token = "";
        }
    }
    components.push_back(token);
    return components;
}

std::string read_input()
{
	memset(buf,'\0',1024);
	char b = '\0';
	int i = 0;
	
	while(b != '\n'){
		read(STDIN_FILENO, &b, 1);
		buf[i] = b;
		i++;
	}
	buf[i-1]='\0'; // Remove New Line Character
	raw = buf;
	return raw;
}

bool check_special_command(std::string s){
	return false;
}


bool check_command_name(std::string s)
{
	std::vector<std::string> cmd = split_on_spaces(s);
	
	// Allow up to 10 arguments ...
	//int size = cmd.size();
	//for(int i = size+1; i < 10; i++){
	//	cmd.push_back("\0");
	//}

	//cmd[0] = "ls";
	//cmd[1] = "-la";
	
	char* b0;
	char* b1;
	char* b2;
	char* b3;
	char* b4;
	char* b5;
	
	if(cmd.size()>0){
		b0 = new char[cmd[0].size()];
		strcpy(b0, cmd[0].c_str());
	} else{
		b0 = (char*)NULL;
	}
	
	if(cmd.size()>1){
		b1 = new char[cmd[1].size()];
		strcpy(b1, cmd[1].c_str());
	} else{
		b1 = (char*)NULL;
	}
	
	if(cmd.size()>2){
		b2 = new char[cmd[2].size()];
		strcpy(b2, cmd[2].c_str());
	} else{
		b2 = (char*)NULL;
	}
	
	if ( cmd.size() > 3 ){
		b3 = new char[cmd[3].size()];
		strcpy(b3, cmd[3].c_str());
	} else{
		b3 = (char*)NULL;
	}
	
	if(cmd.size()>4){
		b4 = new char[cmd[4].size()];
		strcpy(b4, cmd[4].c_str());
	} else{
		b4 = (char*)NULL;
	}	
	
	
	char *temp[] = {b0,
					b1,
					b2,
					b3,
					b4,
					(char*)NULL};
	
    execvp(temp[0],temp);

	return false;
}

bool check_command_args(std::string s)
{
	return false;
}

bool check_unix_command(std::string s)
{
	for(unsigned int i = 0; i < s.size(); i++){
		// PIPE
		if(s[i]=='|'){
			//check_command_args();
		}
		else{
			// DO NOTHING
		}
	}
	
	check_command_name(s);
	
	//check_command_args(s);
	
	
	return true;
}

bool check_string()
{
	std::string token = "";
	
	int begin = 0;
	int end  = raw.size();
	
	// Parse String
	for(unsigned int i = 0; i < raw.size(); i++){
		// PIPE
		if(raw[i]=='|'){
			std::string a = raw.substr(begin,i-1);
			// Increment Token Window
			begin = i+1;
		}
		// REDIRECTION <
		else if(raw[i]=='<') {
			
		}
		// REDIRECTION >
		else if(raw[i]=='>'){
			
		}
		// AMP
		else if(raw[i]=='&'){
		}
		// DO NOTHING
		else{
			// DO NOTHING
		}
	}
	// UNIX COMMAND
	if(check_unix_command(raw)){
		
	}
	
	// SPECIAL COMMAND
	if(check_special_command(raw)){
		
	}
	
	// COMMAND NOT SUPPORTED
	return false;
}

bool detect_cd(std::vector<std::string> cmd){
	if(!cmd[0].find("cd") > 0){
		if (fork() == 0){
			chdir (cmd[1].c_str());
		}
		else{
			wait(NULL);
		}
		return true;
	}
	return false;
}

bool detect_redirection(std::vector<std::string> cmd)
{
	// Detect Redirection
	bool redirection_detected = false;
	for(unsigned int i = 0; i < cmd.size(); i++){
					
		// Redirect Command Output into File
		if(cmd[i]==">"){
			redirection_detected = true;
					
			int pipe_red[2];
					
			if(pipe(pipe_red) == -1){
				perror("pipe");
				exit(EXIT_FAILURE);
			}

			pid_t pid = fork();
						
			if(pid == 0) // Child
			{
				close(pipe_red[0]);
				dup2(pipe_red[1], 1);
				close(pipe_red[1]);
						
				std::vector<std::string> s;
				for(int j = 0; j < i; j++){
					s.push_back(cmd[j]);
				}
							
				execute_vector(s);
			}
			else // Parent
			{
				wait(NULL);
							
				close(pipe_red[1]);
							
				char* b;

				int file_fd = open(cmd[i+1].c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

				while(read(pipe_red[0],&b,1)!=0) write(file_fd,&b,1);
					
				close(file_fd);
				close(pipe_red[0]);
			}
			break;
		}
					
		// Redirect File into Command
		if(cmd[i]=="<"){
			redirection_detected = true;
						
			pid_t pid = fork();
						
			if(pid == 0) // Child
			{
				int file_fd = open(cmd[i+1].c_str(), O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
				
				dup2(file_fd, 0);
				
				std::vector<std::string> s;
				for(int j = 0; j < i; j++){
					s.push_back(cmd[j]);
				}
				
				close(file_fd);
				execute_vector(s);
			}
			else // Parent
			{
				wait(NULL);
			}
			break;
		}	
					
					
					
	}				
				
	
	return redirection_detected;
}

void execute_vector(std::vector<std::string> cmd)
{

	// INPUT REDIRECTION
	if(!detect_redirection(cmd) && !detect_cd(cmd))
		{
		char* b0;
		char* b1;
		char* b2;
		char* b3;
		
		if(cmd.size()>0){
			b0 = new char[cmd[0].size()];
			strcpy(b0, cmd[0].c_str());
		} else{
			b0 = (char*)NULL;
		}
		
		if(cmd.size()>1){
			b1 = new char[cmd[1].size()];
			strcpy(b1, cmd[1].c_str());
		} else{
			b1 = (char*)NULL;
		}
		
		if(cmd.size()>2){
			b2 = new char[cmd[2].size()];
			strcpy(b2, cmd[2].c_str());
		} else{
			b2 = (char*)NULL;
		}
		
		if(cmd.size()>3){
			b3 = new char[cmd[3].size()];
			strcpy(b3, cmd[3].c_str());
		} else{
			b3 = (char*)NULL;
		}
		
		
		char *temp[] = {b0,
						b1,
						b2,
						b3,
						(char*)NULL};
		
	    execvp(temp[0],temp);
	}
}

int main(int argc, char** argv)
{
	// READ INPUT ON NEWLINE
	
	/* Parse command line arguments */

	/* Initialize prompt to default (current directory followed by a colon */

	/* Vector to maintain background processes */

	while(true){
		update_prompt();
	
		/* TOKENIZE INPUT COMMAND */
		std::string input = read_input();
		
		// Detect Background Character "&"
		bool background = false;
		for(unsigned int c = 0; c < input.size(); c++){
			if(input[c]=='&'){
				background = true;
				input[c]=' ';
			}
		}
		
		// Split on Pipe
		std::vector<std::string> pipes = split_on_pipe(input);
	
		// Split on Spaces
		std::vector<std::vector<std::string>> commands;
		for(unsigned int p = 0; p < pipes.size(); p++){
			std::vector<std::string> spaces = split_on_spaces(pipes[p]);
			
			// Clean Empty Tokens and Quotations
			std::vector<std::string> clean_spaces;
			for(unsigned int j = 0; j < spaces.size(); j++){
				if(spaces[j]!=" " && spaces[j]!="\0"){
					clean_spaces.push_back(spaces[j]);
				}
			}
			
			// Add Cleaned Command to Vector
			commands.push_back(clean_spaces);		
		}
		
	//	bool spc = check_Special(commands[0]);
		
		// PIPED
		if(commands.size()>1){
	
			// CREATE PIPES
			int pipe_fds[commands.size()-1][2];
			
			for(unsigned int p = 0; p < commands.size()-1; p++){
				if( pipe(pipe_fds[p]) == -1 ){
					perror("pipe");
					exit(EXIT_FAILURE);
				}
			}
			
			for(unsigned int p = 0; p < commands.size(); p++){
				pid_t pid = fork();
				
				// Child 
				if(pid==0){ 
					
					// Configure Input
					if(p>0)
					{
						close(pipe_fds[p-1][1]);
						dup2 (pipe_fds[p-1][0], 0); // Standard Input -> Pipe
						close(pipe_fds[p-1][0]);
					}				
					
					// Configure Output
					if(p < commands.size()-1){
						close(pipe_fds[p][0]);
						dup2 (pipe_fds[p][1], 1); // Standard Output -> Pipe
						close(pipe_fds[p][1]);
					} 
	
					execute_vector(commands[p]);			
					
					exit(0);
				} 
				// Parent
				else{ 
					wait(NULL);
					
					if(p > 0) close(pipe_fds[p-1][0]);
					if(p < commands.size() - 1) close(pipe_fds[p][1]);
				}
			}
			
			for(unsigned int p = 0; p < commands.size()-1; p ++){
				close( pipe_fds[p][0] );
				close( pipe_fds[p][1] );
			}
			
			
		}
		// NOT PIPED
		else{
			if(fork()==0)
				execute_vector(commands[0]);
			else
				wait(NULL);
				
			/*
				// Detect Redirection
				bool redirection_detected = false;
				for(unsigned int i = 0; i < commands[0].size(); i++){
					
					// Redirect Command Output into File
					if(commands[0][i]==">"){
						redirection_detected = true;
						
						int pipe_red[2];
						
						if(pipe(pipe_red) == -1){
							perror("pipe");
							exit(EXIT_FAILURE);
						}

						pid_t pid = fork();
						
						if(pid == 0) // Child
						{
							close(pipe_red[0]);
							dup2(pipe_red[1], 1);
							close(pipe_red[1]);
							
							std::vector<std::string> s;
							for(int j = 0; j < i; j++){
								s.push_back(commands[0][j]);
							}
							
							execute_vector(s);
						}
						else // Parent
						{
							wait(NULL);
							
							close(pipe_red[1]);
							
							char* b;

							int file_fd = open(commands[0][i+1].c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

							while(read(pipe_red[0],&b,1)!=0) write(file_fd,&b,1);
							
							close(file_fd);
							close(pipe_red[0]);
						}
						break;
					}
					
					// Redirect File into Command
					if(commands[0][i]=="<"){
						redirection_detected = true;
						
						pid_t pid = fork();
						
						if(pid == 0) // Child
						{

							int file_fd = open(commands[0][i+1].c_str(), O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
							
							dup2(file_fd, 0);
							
							std::vector<std::string> s;
							for(int j = 0; j < i; j++){
								s.push_back(commands[0][j]);
							}
							
							close(file_fd);
							execute_vector(s);
						}
						else // Parent
						{
							wait(NULL);
						}
						break;
					}	
					
					
					
				}				
				
				if( !redirection_detected ){
					if(fork()==0)
						execute_vector(commands[0]);
					else
						wait(NULL);
				}
			*/
			
		}
		
		
		// DEBUG
		/*
		for(unsigned int i = 0; i < commands.size(); i++){
			for(unsigned int j = 0; j < commands[i].size(); j++){
				printf("%s\t",commands[i][j].c_str());
			}
			printf("\n");
		}
		*/
	}

	return 0;
}
