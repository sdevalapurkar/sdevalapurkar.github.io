//Author: Shreyas Devalapurkar
//Student Number: V00827994
//Course: CSC 360: Operating Systems
//Term: Fall 2016
//Instructor: Dr. Kui Wu

//including all required libraries
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <limits.h>

//defining the structure that will be used throughout the assignment. The proc_list will contain a link to a pid field. Ex: proc_list[0]->pid will return the pid of the first element/process in proc_list
typedef struct {
    pid_t pid;
} 
proc;

int MAXPROC = 1000; //define a max number of processes that can be run simultaneously in the background
proc* proc_list[1000]; //define the proc_list

//This function allows for removing a process from the process list
void removeProcess(int id) 
{
    int x;
    
    //go through the max number of processes that could be potentially running in the background 
    for (x = 0; x < MAXPROC; x++) 
    {
        if(proc_list[x] != NULL && id == proc_list[x]->pid) 
        {
            proc_list[x] = NULL;
            break; //once completed, break out of the loop
        }
    }
    
    return;
}

//a boolean process for error checking
//make sure that the location at which we are trying to access the process isn't NULL. Also, if the pid of the process at that particular location is equal to the id we received (to remove), then set that location in proc_list to NULL so it is signified later as an empty location that doesn't contain a process
bool hasProcess(char* tokens[100], proc** proc_list)
{
    int x;
    int token = atoi(tokens[0]);

    //go through all the processes and make sure that they are not null, and also that the pid attribute within the proc_list is equal to the token which contains the pid that was entered by the user following a command such as bgstop
    for(x = 0; x < MAXPROC; x++)
    {
        if(proc_list[x] != NULL && proc_list[x]->pid == token)
        {
            return true;
        }
    }
    
    //if process is not found, then return false so we know that the user typed in some random value (error)
    return false;
}

//use waitpid to check what is happening with the child process.
//The value can be 0 meaning wait for any child process whose process group is id is equal to that of the calling process.
//The meaning can be > 0 meaning wait for the child whose process id is equal to the value of pid
void update_bg_process() 
{
    int status;
    
    while(1) 
    {
        int id = waitpid(-1, &status, WNOHANG);
        
        if(id == 0) 
        {
            break;
        } 
        else if(id > 0) 
        {
            //we have reached the parent process, so the child has been terminated
            printf("Process %d terminated\n", id);
            removeProcess(id); //call the removeProcess() function in order to remove the process from the list since it has been terminated
        } 
        else if(id < 0) 
        {
            break;
        }
    }
}

//function to add a new process given a process id
void addProcess(int id) 
{
    int i;
    
    for(i = 0; i < MAXPROC; i++) //go through all the processes
    {
        if(proc_list[i] == NULL) //make sure value in proc_list is not NULL, because otherwise we don't want to do anything 
        {
            proc_list[i] = malloc(sizeof(proc)); //allocate memory for the location i
            proc_list[i]->pid = id; //assign the proper id entered in the pid attribute of the proc_list
            break;
        }
    }
    
    return;
}

//send a SIGTERM signal to kill a process with the proper pid
void killProcess(char* tokens[100])
{
    kill(atoi(tokens[0]), SIGTERM);
    printf("PROCESS HAS BEEN KILLED\n");
    update_bg_process(); //call update_bg_process since we need to check the state of the child
}

//send a SIGCONT signal to restart a process that was currently stopped
void startProcess(char* tokens[100])
{
    kill(atoi(tokens[0]), SIGCONT);
    printf("PROCESS HAS BEEN STARTED\n");
    update_bg_process(); //call update
}

//send a SIGSTOP signal to stop a process that is running
void stopProcess(char* tokens[100])
{
    kill(atoi(tokens[0]), SIGSTOP);
    printf("PROCESS HAS BEEN STOPPED\n");
    update_bg_process(); //call update
}

//pstat function to get and read values from the /proc/pid/stat and /proc/pid/status pseudofiles (executable)
void pstatFunction(char* tokens[100], proc** proc_list)
{
    //many variables to store data in
    char path[100];
    FILE* fp;
    int unused;
    char comm[1000];
    char state;
    int utime;
    int stime;
    int rss;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    int num[] = {};
                
    snprintf(path, 100, "/proc/%s/stat", tokens[0]); //get the string into path
    fp = fopen(path, "r"); //open the path for reading
                
    fscanf(fp, "%d %s %c %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %d %d %*d %*d %*d %*d %*d %*d %*d %*d %d", &unused, comm, &state, &utime, &stime, &rss); //get the proper values by skipping through unneccesary ones
                
    printf("Comm: %s\n", comm);
    printf("State: %c\n", state);
    printf("uTime: %d\n", utime);
    printf("sTime: %d\n", stime);
    printf("RSS: %d, \n", rss);

    fclose(fp); //close the file
  
    //same logic as above, but for the status file instead of stat
    snprintf(path, 100, "/proc/%s/status", tokens[0]);
    fp = fopen(path, "r");
                
    while((read = getline(&line, &len, fp)) != -1) 
    {
        if(strncmp(line, "voluntary_ctxt_switches:", 24) == 0) 
        {
            printf("%s", line);
        } 
        if(strncmp(line, "nonvoluntary_ctxt_switches:", 27) == 0) 
        {
            printf("%s", line);
        }
    }
                
    fclose(fp);
}

int main(int argc, char* argv[]) 
{
    while(1) 
    {
        //variables used to get user input 
        char currInput[100];
        char* tokens[100];
        char *token;
        char* argument;
        
        //print the main PMan line
        printf("PMan: > ");
        
        //get the input from user
        fgets(currInput, 100, stdin);
        
        //error check for end of the string
        if(strcmp(currInput, "\n") != 0) 
        {
            currInput[strcspn(currInput, "\n")] = '\0';
        }
    
        //tokenize the input
        argument = strtok(currInput, " \t\r\n\a");
        token = strtok(NULL, " \t\r\n\a");
        
        int i = 0;
        //store it in tokens at necessary indices
        while(token != NULL) 
        {
            tokens[i] = token;
            token = strtok(NULL, " \t\r\n\a");
            i++;
        }
        
        update_bg_process(); //call update
        
        //check if first word entered is bgkill
        if(strcmp(argument, "bgkill") == 0) 
        {
            //call hasProcess to check if the process entered is valid for killing
            if(hasProcess(tokens, proc_list))
            {
                killProcess(tokens);
            }
            else
            {
                printf("PROCESS DOES NOT EXIST\n");
            }
        } 
        
        //same as above for bgstart
        else if(strcmp(argument, "bgstart") == 0) 
        {
            if(hasProcess(tokens, proc_list))
            {
                startProcess(tokens);
            }
            else
            {
                printf("PROCESS DOES NOT EXIST\n");
            }
        }
        
        //same as above for bgstop
        else if(strcmp(argument, "bgstop") == 0) 
        {
            if(hasProcess(tokens, proc_list))
            {    
                stopProcess(tokens);
            }
            else
            {
                printf("PROCESS DOES NOT EXIST\n");
            }
        } 
        
        //same as above for bg
        else if (strcmp(argument, "bg") == 0) 
        { 
            pid_t pid = fork(); //fork to create a child process
            
            if(pid >= 0) 
            { 
                if(pid == 0) //if child, then execute the proper commands
                {
                    if(execvp(tokens[0], tokens) == -1) 
                    {
                        perror("ERROR");
                    }
                } 
                else 
                {   
                    addProcess(pid); //add the process if pid > 0 meaning that parent. Parent needs to add child process to list
                }
            } 
            else 
            {
                perror("ERROR\n");
                exit(-1);
            }
        } 
        
        //same thing but for pstat
        else if(strcmp(argument, "pstat") == 0) 
        {
            if(hasProcess(tokens, proc_list))
            {    
                pstatFunction(tokens, proc_list);
            }        
            else
            {
                printf("PROCESS DOES NOT EXIST\n");
            }
        } 
        
        //same but for bglist
        else if (strcmp(argument, "bglist") == 0) 
        {
            int counter = 0;
            int i;
            
            //go through all processes
            for(i = 0; i < MAXPROC; i++) 
            {
                //make sure not NULL and the pid attribute is not zero
                if(proc_list[i] != NULL && proc_list[i]->pid != 0) 
                {
                    char path[PATH_MAX+1];
                    char dest1[1000];
                    
                    //store the string /proc/pid/exe into dest1 since that is the file we need to retrieve the path from 
                    sprintf(dest1, "/proc/%d/exe", proc_list[i]->pid);
                    
                    //read the link (get path from the file)
                    if (readlink(dest1, path, sizeof(path)) != -1) 
                    {
                        //perror("Readlink Error");
                    }
                    
                    printf("%i: %s \n", proc_list[i]->pid, path);
                    
                    counter++;
                }
            }
            
            printf("Total Background Jobs: %d\n", counter);
        } 
        
        //some random argument means that don't do anything, tell user that argument not found
        else 
        {
            printf("%s: Argument not found\n", argument);
        }
    }
}
