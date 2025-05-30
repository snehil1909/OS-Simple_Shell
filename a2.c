#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>


#define MAX_CMD_LENGTH 100
#define MAX_ARG_LENGTH 100
#define MAX_HISTORY_LENGTH 100
#define MAX_PIPE_COMMAND 5
#define CNT_HISTORY_DETAILS 100

char pipe_commands[MAX_PIPE_COMMAND][MAX_CMD_LENGTH];

struct history_details
{
    int pid;
    //time_t currentTime;
    char currentTime[100];
    double timeDuration;
};

struct history_details cmd_history_details[CNT_HISTORY_DETAILS];
int cmd_history_details_ctr;

//Dividing the input line into arguements and commands array 
//1. this means seperating the line with spaces 
//2. and finding the command in the line 
//3. arguements also we need to find 

int finding_cmd_arg(char input[], char* command, char* argument[],int counter)
{

   char *token=NULL;
   
   token = strtok(input," \n");

    if (!token)
       return 0;
   //first word is a command 
   strcpy(command,token);
//   printf("Command: %s\n",command);

   int argument_counter = 0;

   while (token != NULL)
   {
        argument[argument_counter] = token;
        token=strtok(NULL," \n");
        argument_counter++;
   }

   argument[argument_counter] = NULL;

   counter=argument_counter;
   return argument_counter;
}
//check if pipe exists in command
int check_for_pipe(char input[])
{
    char search='|';
    char *result=strchr(input,search);
    if(!result)
    {
        return 0;
    }
    return 1;
}

// check if it's a background process
int check_for_bkgnd_process(char input[])
{
    char search='&';
    char *result=strchr(input,search);
    if(!result)
    {
        return 0;
    }
    return 1;
}

//implementing pipe functioning 
void execute_pipe(char* input) 
{
    char* token=NULL;
    char* token1=NULL;
    char* del = "|";
    clock_t startTime, endTime;
    long int timeDuration = 0;
    time_t curr_time;
    
      // Check if we have pipes in the command
    token = strtok(input, "|");


    if (token)
    {
        int pipe_cnt = 0;
        

        for (int i = 0; i < MAX_PIPE_COMMAND; i++)
            pipe_commands[i][0] ='\0';

        while (token != NULL)     
        {
            strcpy(pipe_commands[pipe_cnt], token);
            token = strtok(NULL, "|");
            pipe_cnt++;
        }
    
        int rc;
        int fd[2];
        // file descriptor of previous pipe command
        int last_pipe_fd = STDIN_FILENO;


        startTime = clock();

        for (int i = 0; i < pipe_cnt; i++) 
        {
            char *cmd_args[MAX_ARG_LENGTH];
            int argument_counter = 0;
            clock_t startTime, endTime;
            
            time_t curr_time;

            token1 = strtok(pipe_commands[i], " \n");
            
            while (token1 != NULL) 
            {  
                cmd_args[argument_counter] = token1;
                token1 = strtok(NULL, " ");
                argument_counter++;            
            }

            // Asssign the last argument to NULL before passing it to execvp
            cmd_args[argument_counter] = NULL;


            if (pipe(fd) == -1) 
            {
                printf("pipe error");
                exit(1);
            }

            rc = fork();

            if (rc < 0) 
            {
                printf("fork failed\n");
                exit(1);
            }

            time(&curr_time);

            if (cmd_history_details_ctr < CNT_HISTORY_DETAILS)
            {
                cmd_history_details[cmd_history_details_ctr].pid = rc;
                strcpy(cmd_history_details[cmd_history_details_ctr].currentTime, ctime(&curr_time));
                //printf("\n Printing time for pipe %d iteration, cmd counter %d %s\n", i, cmd_history_details_ctr, cmd_history_details[cmd_history_details_ctr].currentTime);
                
            }

            // Child process
            if (rc == 0) {

                // Redirect ouput to std input of previous pipe for second pipe command onwards
                if (i > 0) 
                {
                    dup2(last_pipe_fd, STDIN_FILENO);
                    close(last_pipe_fd);
                }

                // Redirect output to std output of last command 
                if (i < pipe_cnt - 1)
                {
                    dup2(fd[1], STDOUT_FILENO);
                    close(fd[1]);
                }
                // Execute the command
                //startTime = clock();        
                execvp(cmd_args[0], cmd_args);
                //endTime = clock();
                //timeDuration = timeDuration + ((double)(endTime - startTime) / CLOCKS_PER_SEC);

            } 
            else 
            {
                // For Child processes
                wait(NULL);

                // Closing the previous pipe's read
                if (i > 0) 
                    close(last_pipe_fd);
                

                // Storing current pipe's read for the next iteration of pip command
                last_pipe_fd = fd[0];

                
                close(fd[1]);

            }
        }
        endTime = clock();
        timeDuration = (double)(endTime - startTime) / CLOCKS_PER_SEC;
        if (cmd_history_details_ctr < CNT_HISTORY_DETAILS)
        {
            cmd_history_details[cmd_history_details_ctr].timeDuration = timeDuration;
            //printf("Duration: %ld\n", timeDuration);
        }
    }
    cmd_history_details_ctr++;
  //  return 1;
}

// & case is missing here
void execute(char* cmd, char* args[],int cnt,int bkg)
{
    clock_t startTime, endTime;
    time_t curr_time;
    long int timeDuration;

    int rc = fork();

    
    time(&curr_time);

    if (cmd_history_details_ctr < CNT_HISTORY_DETAILS)
    {
        cmd_history_details[cmd_history_details_ctr].pid = rc;
        strcpy(cmd_history_details[cmd_history_details_ctr].currentTime, ctime(&curr_time));
    }

    if (rc < 0)
    {
        fprintf(stderr,"fork failed\n");
        exit(1);
    }
    else if (rc == 0)
    {
        // child process 
        //startTime = clock();
        //time(&curr_time);

        startTime = clock();
        if(execvp(cmd, args)==-1)
            printf("if this is printing then its an error\n");
        endTime = clock();

        timeDuration = (double)(endTime - startTime) / CLOCKS_PER_SEC;

        if (cmd_history_details_ctr < CNT_HISTORY_DETAILS)
        {
            cmd_history_details[cmd_history_details_ctr].timeDuration = timeDuration;
        //  cmd_history_details_ctr++;
        }
    }
    else
    {               
        int rc_wait;
        if(!bkg)
            // wait for child process
            rc_wait = wait(NULL);
        else
        {
            printf("bkgnd process is working: %d\n",rc);
        }
        //printf("I am parent of %d (rc_wait: %d) (pid:%d)\n",rc, rc_wait, (int)getpid());
        
    }

    cmd_history_details_ctr++;
}

void displayHistory(char lines[][MAX_CMD_LENGTH], int lctr)
{
    if (lctr > 0)
    {
        for (int i = 0; i < lctr; i++)
        {
            printf("Command: %s Child pid: %d CurrentTime: %s, Duration: %f\n", lines[i], cmd_history_details[i].pid, cmd_history_details[i].currentTime, cmd_history_details[i].timeDuration);

        }
    }
    else 
    {
        printf("No command was there before\n");
    }
}

int main()
{
    //Reading the input line 
    char inputLine[MAX_CMD_LENGTH];
    memset(inputLine,'\0',MAX_CMD_LENGTH);
    char command[MAX_CMD_LENGTH];
    memset(command,'\0',MAX_CMD_LENGTH);
    char *arguement[MAX_ARG_LENGTH];
    int lctr = 0;
    int bPipe = 0; // Test whether it's a pipe or not
    
    

    char copy_input [MAX_HISTORY_LENGTH][MAX_CMD_LENGTH];
    for (int i = 0; i < MAX_HISTORY_LENGTH; i++)
        memset(copy_input[i],'\0',MAX_CMD_LENGTH);
    

    while(1)
    {
        bPipe = 0;

        printf("iiitd@possum:~$ ");
        if(fgets(inputLine,sizeof(inputLine)-1,stdin)!=NULL)
        {
            
            //printf("Entered line: %s\n",inputLine);
            // to remove the trailing new line character 
            inputLine[strcspn(inputLine,"\n")]='\0';
        }
        else 
        {
           printf("Error reading the line\n");
           return 0;
        }
        // need to compare the string cases from the input
        if (strcmp(inputLine,"exit")==0)
        {
           break;           
        }

        if(strcmp(inputLine,"history")==0)
        {
            // here to show the history buffer or history commands through a function 
            if (lctr > 0)
               displayHistory(copy_input, lctr);
            continue;

        }
        strcpy(copy_input[lctr++],inputLine);
        if(check_for_pipe(inputLine))
        {
            execute_pipe(inputLine);
            bPipe = 1;
        }

            // else{printf("error in executing pipe\n");}
        

        // If it's not a pipe function
        if (bPipe == 0)
        {
            int cnt;
            int bkg = 0;
            int len = strlen(inputLine);
            char* a = "&";
            if (inputLine[len-1]=='&')
            {
                inputLine[strcspn(inputLine,a)]='\0';
                bkg=1;
            }

            if (finding_cmd_arg(inputLine,command,arguement,cnt))
            execute(command, arguement,cnt,bkg);
        }
    }
    
    return 0;
}