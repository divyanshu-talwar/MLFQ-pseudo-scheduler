# MLFQ-pseudo-scheduler

Task: You will be implementing MLFQ scheduler. The program will involve multithreading. The scheduler will take config parameters in a config file and set up the scheduler. The config parameters will be:

No. of queues. (should work for around 5 queues)

Time slice in each of the queues.

Time period after which the priority will be boosted (i.e. all are moved to the top queue).

No. of processes and time they will take to complete, and their order and time of arrival. (should work for 20+ processes)

The scheduler will run in the main thread and new jobs will be created as other threads. Use of pThread library, with locks etc., will be required to implement it. As an output, you will show the output of each queue, and turnaround time of each process. 

This is the first part of this assignment, in the second part more features will be added. However the deadline will remain same, so it is advised that you start working on it immediately. 



Marks: 10.

Files should be named as:

ROLLNO_A4.c or ROLLNo_A4_X.c

X is 1,2,3... (same as in previous assignments).