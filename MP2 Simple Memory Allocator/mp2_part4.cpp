/* -------------------------------------------------------------------------- */
/* Developer: Andrew Kirfman                                                  */
/* Project: CSCE-313 Machine Problem #3                                       */
/*                                                                            */
/* File: ./mp3_part4.cpp                                                      */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Standard Library Includes                                                  */
/* -------------------------------------------------------------------------- */

#include <stdlib.h>
#include <unistd.h>
#include <string>

int main()
{

    std::string s = "aaaaHELLO";
    std::string t = "b";
    
    int i = (s.find(t));
    printf("%d",i);
    
    //char *temp[] = {"ls","-la",(char*)NULL};
    //execvp("ls",temp);


    exit(0);
}

