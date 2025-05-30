Snehil Jaiswal - finding_cmd_arg, displayHistory, main funcn.
Arnav Batra- execute_pipe, execute, main function.

1. finding_cmd_arg: It uses strtok to split the input string using spaces and newline characters as delimiters.
The first token is considered the command, which is stored in the command variable.
Subsequent tokens are considered arguments and are stored in the argument array.
It keeps track of the number of arguments using the counter variable

2. check_for_pipe: It uses strchr to search for the pipe character in the input line

3. check_for_bkgnd_process: It uses strchr to search for the ampersand character (&) in the input line.
If an ampersand character(&) is found, it returns 1 (indicating true), otherwise 0.

4. execute_pipe: It first splits the input line into individual pipe commands using strtok.
It then uses a loop to set up pipes, fork child processes, and execute each command in the sequence.
Each child process reads from the previous command's output (if not the first command) and writes to the next command's input (if not the last command).
The parent process waits for each child to finish execution.

5. execute : t forks a child process using fork() to execute the command.
In the child process, it uses execvp to replace the current process image with a new one specified by cmd and args.
If execvp fails, an error message is printed.
The parent process either waits for the child to finish (if not a background process) or continues immediately (if a background process).

6. displayHistory: It iterates through the command history and prints out details for each command

There is also some error handling in the main function.


An example of a sample run:

---------------------------------

iiitd@possum:~$ ls -l | grep test | wc -l
2
iiitd@possum:~$ ls -l | grep test
-rwxrwxr-x 1 vboxuser vboxuser 17280 Sep 10 22:02 test
-rw-rw-r-- 1 vboxuser vboxuser  9382 Sep 10 22:00 test.c
iiitd@possum:~$ pwd
/home/vboxuser/projects/os/assignment_2
iiitd@possum:~$ history
Command: ls -l | grep test | wc -l Child pid: 94740 CurrentTime: Sun Sep 10 22:02:57 2023
, Duration: 94387912885944
Command: ls -l | grep test Child pid: 94758 CurrentTime: Sun Sep 10 22:03:09 2023
, Duration: 116
Command: pwd Child pid: 94769 CurrentTime: Sun Sep 10 22:03:11 2023
, Duration: 97
iiitd@possum:~$ exit

----------------------------------


This terminal runs the following funcitons:
    ls
    ls /home
    echo you should be aware of the plagiarism policy
    wc -l fib.c
    wc -c fib.c
    grep printf helloworld.c
    ls -R
    ls -l
    ./fib 40
    ./helloworld
    sort fib.c
    uniq file.txt
    cat fib.c | wc -l
    cat helloworld.c | grep print | wc -l

