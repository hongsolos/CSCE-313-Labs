/* ------------------------------------------------------------------------- */
/* Developer: Andrew Kirfman                                                 */
/* Project: CSCE-313 Machine Problem #3                                      */
/*                                                                           */
/* File: ./proctest.cpp                                  */
/* ------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------- */
/* Standard Library Includes                                                 */
/* ------------------------------------------------------------------------- */

#include<iostream>
#include<vector>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<string>
#include<regex>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/wait.h>

/* ------------------------------------------------------------------------- */
/* User Defined Includes                                                     */
/* ------------------------------------------------------------------------- */

#include "proctest.h"

/* ------------------------------------------------------------------------- */
/* Proctest Private Member Functions                                         */
/* ------------------------------------------------------------------------- */

/* 
 * This function turns a char* buffer into a string given the address
 * to the buffer.  
 */ 
std::string Proctest::stringify_buffer(char *buffer)
{
    std::string buffer_string = "";
    int pos = 0;

    while(true)
    {
        if(buffer[pos] == '\0')
        {
            break;
        }
        else
        {
            buffer_string += buffer[pos];
            pos++;
        }
    }
    return buffer_string;
}

/* 
 * This function takes in a string and breaks it up into a vector
 * of space separated words.  
 */ 
std::vector<std::string> Proctest::split_on_spaces(std::string input)
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
    return components;
}

/* -------------------------------------------------------------------------- */
/* Proctest Public Member Functions                                           */
/* -------------------------------------------------------------------------- */

Proctest::Proctest(int process_id)
{
    /* Record process ID -----------------------------------------------------*/
    m_process_id = process_id;

    /* Initialize the read buffer --------------------------------------------*/
    m_read_buffer = new char[65536];

    /* Common Variables */
    int bytes_read;

    /* READ FROM STAT FILE ---------------------------------------------------*/
    std::string stat_file = "/proc/"+std::to_string(m_process_id)+"/stat";

    int fd_stat = open(stat_file.c_str(),O_RDONLY);
    
    bytes_read = 0;
    while (read(fd_stat, &m_read_buffer[bytes_read], 1) != 0) bytes_read++;
    m_read_buffer [bytes_read] = '\0'; // Terminate Buffer
    close(fd_stat);
    
    const std::string passing = stringify_buffer(m_read_buffer);
    m_stat_array.reserve(split_on_spaces(passing).size());
    m_stat_array = split_on_spaces(passing);
    
    /* READ FROM STATUS FILE -------------------------------------------------*/
    std::string status_file = "/proc/"+std::to_string(m_process_id)+"/status";
    
    int fd_status = open(status_file.c_str(),O_RDONLY);
    
    memset(m_read_buffer, '\0', sizeof(m_read_buffer));    
    bytes_read = 0;
    while (read(fd_status, &m_read_buffer[bytes_read], 1) != 0) bytes_read++;
    m_read_buffer [bytes_read] = '\0'; // Terminate Buffer
    close(fd_stat);
    
    //Split by lines
    std::vector<std::string> lines;
    std::string token = "\n";
    const std::string chunk = stringify_buffer(m_read_buffer);
    
    for(unsigned short int i = 0; i < chunk.length(); i++){
        if(chunk[i] != '\n'){
            token += chunk[i];
        } else {
            lines.push_back(token);
            token = "\n";
        }
    }
    
    //Push in status array
    m_status_array.resize(lines.size());
    for (int i = 0; i < lines.size(); i++){
        std::string s = lines[i];
        std::vector<std::string> internal;
        std::istringstream iss(s);
        for(std::string s; iss >> s; ){
            internal.push_back(s);
        }
        m_status_array[i].resize(internal.size());
        m_status_array[i] = internal;
    }


   
    /* Read Memory Map */
    std::string map_file = "/proc/"+std::to_string(m_process_id)+"/maps";
    
    int pipe_map[2];
    
    if (pipe(pipe_map) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }  
	
	if(fork() == 0) // Child
	{
	    close(pipe_map[0]); // Remove Read Pipe
	    dup2 (pipe_map[1],STDOUT_FILENO); // Connect STDOUT to Write Pipe
	    close(pipe_map[1]);
	    execlp("cat","cat",map_file.c_str(),(char*)NULL);
	    exit(0);
	}
	else // Parent
	{
	    close(pipe_map[1]);
		wait(NULL);
	}
        
    bytes_read = 0;   
    memset(m_read_buffer, '\0', sizeof(m_read_buffer));
    while (read(pipe_map[0], &m_read_buffer[bytes_read], 1) != 0) bytes_read++;
    m_read_buffer [bytes_read] = '\0'; // Terminate Buffer
	close(pipe_map[0]); // Remove Read Pipe
	    

    int mem_read = bytes_read;

    std::vector<std::string> liness;
    std::string tokens = "\0";
    

    
    if(mem_read>10){
        const std::string chunks = stringify_buffer(m_read_buffer);
        
        for(unsigned short int i = 0; i < chunks.length(); i++){
            if(chunks[i] != '\n'){
                tokens += chunks[i];
            } else {
                liness.push_back(tokens);
                tokens = "\0";
            }
        }
        m_mem_array = liness;   
    }
    else {
        
        liness.push_back("Permission denied");
        m_mem_array = liness;
        m_num_fds = "Permission denied";
    }    
    

    
    
    
    /* FIND THE NUMBER OF OPEN FILE DESCRIPTORS BY COUNTING /proc/[pid]/fd ---*/
    //fprintf(stderr, "The number of opened file descriptors: ");
    if(mem_read > 10){
        std::string fd_files = "/proc/" + std::to_string(m_process_id) + "/fd";
        char *cmd1[] = {"/bin/ls", (char*)fd_files.c_str(), 0};
        char *cmd2[] = {"/usr/bin/wc", "-l", 0};
        
        const int READ = 0;
        const int WRITE = 1;
        int pipe_fds[2]; //Pipe for wc -l
        int term_fds[2]; //Pipe for capture output of fds
        int pid1, pid2;
        
        if (pipe(pipe_fds) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
        
        if((pid1=fork()) == 0) { //Child -------------------------
    	    close(pipe_fds[READ]);
    	    dup2(pipe_fds[WRITE], 1);
    	    close(pipe_fds[WRITE]);
    	    execvp(cmd1[0],cmd1);
    	    exit(0);
        } else {                 //Parent ------------------------
            wait(NULL);
            
            if (pipe(term_fds) == -1) {
                perror("pipe");
                exit(EXIT_FAILURE);
            }        
            
            if((pid2=fork()) == 0){
                while ((dup2(pipe_fds[READ], 0) == -1) && (errno == EINTR)) {}            
                close(pipe_fds[READ]);
        	    close(pipe_fds[WRITE]);
                
                while ((dup2(term_fds[WRITE], 1) == -1) && (errno == EINTR)) {}            
                close(term_fds[WRITE]);
                close(term_fds[READ]);
    
                execvp(cmd2[0],cmd2); 
                
                exit(0);
            } else {
                close(pipe_fds[READ]);
                close(pipe_fds[WRITE]);
                close(term_fds[WRITE]);
                
                wait(NULL);
                
                char buffer[16];
                close(term_fds[WRITE]);
                //read(term_fds[READ],&buffer,1);
                bytes_read = 0;
                while(read(term_fds[READ],&buffer[bytes_read],1)!=0) bytes_read++;
                buffer [bytes_read-1] = '\0'; // Terminate Buffer (Overwrite New Line)
                close(term_fds[READ]);
                
                //printf("%i \n",(int)(buffer-'0'));
                //printf("%s",buffer);
                
                m_num_fds = buffer;
                
            }
        }
    }
    

    
    
    
    
    

}

Proctest::~Proctest()
{
    //Free buffer memory & clear all vectors
    free(m_read_buffer);
    m_stat_array.clear();
    m_status_array.clear();
    m_mem_array.clear();
    m_process_id = 0;
}


std::string Proctest::getpid()
{
    return m_stat_array[0];
}

std::string Proctest::getppid()
{
    return m_stat_array[3];
}

std::string Proctest::geteuid()
{
    int where = 0;
    for (unsigned int i = 0; i < m_status_array.size(); i++){
        std::string x = m_status_array[i][0].c_str();
        if (x.find("Uid") == 0){
            where = i;
        }
    }
    return m_status_array[where][2];
}

std::string Proctest::getegid()
{
    int where = 0;
    for (unsigned int i = 0; i < m_status_array.size(); i++){
        std::string x = m_status_array[i][0].c_str();
        if (x.find("Gid") == 0){
            where = i;
        }
    }
    return m_status_array[where][2];
}

std::string Proctest::getruid()
{
    int where = 0;
    for (unsigned int i = 0; i < m_status_array.size(); i++){
        std::string x = m_status_array[i][0].c_str();
        if (x.find("Uid") == 0){
            where = i;
        }
    }
    return m_status_array[where][1];
}

std::string Proctest::getrgid()
{
        int where = 0;
    for (unsigned int i = 0; i < m_status_array.size(); i++){
        std::string x = m_status_array[i][0].c_str();
        if (x.find("Gid") == 0){
            where = i;
        }
    }
    return m_status_array[where][1];
}

std::string Proctest::getfsuid()
{
        int where = 0;
    for (unsigned int i = 0; i < m_status_array.size(); i++){
        std::string x = m_status_array[i][0].c_str();
        if (x.find("Uid") == 0){
            where = i;
        }
    }
    return m_status_array[where][4];
}

std::string Proctest::getfsgid()
{
    int where = 0;
    for (unsigned int i = 0; i < m_status_array.size(); i++){
        std::string x = m_status_array[i][0].c_str();
        if (x.find("Gid") == 0){
            where = i;
        }
    }
    return m_status_array[where][4];
}

std::string Proctest::getstate()
{
    return m_stat_array[2];
}

std::string Proctest::getthread_count()
{
    return m_stat_array[19];
}

std::string Proctest::getpriority()
{
    return m_stat_array[17];
}

std::string Proctest::getniceness()
{
    return m_stat_array[18];
}

std::string Proctest::getstime()
{
    return m_stat_array[14];
}

std::string Proctest::getutime()
{
    return m_stat_array[13];
}

std::string Proctest::getcstime()
{
    return m_stat_array[16];
}

std::string Proctest::getcutime()
{
    return m_stat_array[15];
}

std::string Proctest::getstartcode()
{
    return m_stat_array[25];
}

std::string Proctest::getendcode()
{
    return m_stat_array[26];
}

std::string Proctest::getesp()
{
    return m_stat_array[28];
}

std::string Proctest::geteip()
{
    return m_stat_array[29];
}

std::string Proctest::getfiles()
{
    return m_num_fds;
}

std::string Proctest::getvoluntary_context_switches()
{
    int where = 0;
    for (unsigned int i = 0; i < m_status_array.size(); i++){
        std::string x = m_status_array[i][0].c_str();
        if (x.find("voluntary_ctxt_switches") == 0){
            where = i;
        }
    }
    return m_status_array[where][1];
}

std::string Proctest::getnonvoluntary_context_switches()
{
    int where = 0;
    for (unsigned int i = 0; i < m_status_array.size(); i++){
        std::string x = m_status_array[i][0].c_str();
        if (x.find("nonvoluntary_ctxt_switches") == 0){
            where = i;
        }
    }
    return m_status_array[where][1];
}

std::string Proctest::getlast_cpu()
{
    int where = 0;
    for (unsigned int i = 0; i < m_status_array.size(); i++){
        std::string x = m_status_array[i][0].c_str();
        if (x.find("Cpus_allowed") == 0){
            where = i;
            break;
        }
    }
    return m_status_array[where][1];
}

std::string Proctest::getallowed_cpus()
{
    int where = 0;
    for (unsigned int i = 0; i < m_status_array.size(); i++){
        std::string x = m_status_array[i][0].c_str();
        if (x.find("Cpus_allowed_list") == 0){
            where = i;
        }
    }
    return m_status_array[where][1];
}

std::vector<std::string> Proctest::getmemory_map()
{
    return m_mem_array;
}
