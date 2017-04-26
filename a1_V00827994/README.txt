Author: Shreyas Devalapurkar
Student Number: V00827994
Course: CSC 360
Instructor: Dr. Kui Wu
Term: Fall 2016

Assignment #1 README FILE

————————————————————
Running the program
————————————————————

In order to get the program running, either:

1) run the makefile by typing:
make -f makefile.txt

OR

2) assuming that already on the linux server:
- open a terminal window
- navigate to the directory where the files submitted are located using cd and ls
- once in the correct directory, type gcc -o PMan PMan.c in order to compile the PMan.c file
- also type gcc -o inf inf.c in order to compile the inf.c file
- to run PMan, type ./PMan
- the program is now running

——————————————————————
Command Line Arguments
——————————————————————
After the makefile has been run or the above steps have been followed, and the program is now running, typing in command line arguments.

Example: bg ./inf hey 8

This example uses the inf.c file that was provided to run a background process that prints the word hey every 8 seconds.

From here:
 
- we can test and type bglist to see that this process is listed as a running background process. 

- the process can be killed by typing bgkill pid and then bglist to check that the process is gone from the list.

- bgstop pid, bgstart pid, and pstat pid can also be checked to see if they return the correct results/values.

——————————————————————
Decisions Made
——————————————————————

I chose to use an array implementation over a linked list because I was more comfortable with dealing with arrays. My implementation allows for a large number of processes to run simultaneously in the background. However, there is a limit. Since, scalability was not mentioned as a criteria, I decided that this approach would be best for this task.

I chose to use execvp() instead of system because it was easier to pass in the parameters needed and also we learnt it in class so it was easier than learning how to use something completely new. I read about system() and how it works in order to better understand how execution of processes and commands works.
