#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

int no_of_queues, no_of_processes, priority_boost_time;
int global_clock = 0;
int running = 0;

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t main_m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;
pthread_cond_t main_c = PTHREAD_COND_INITIALIZER;
pthread_cond_t running_c = PTHREAD_COND_INITIALIZER;

struct process{
	int arrival_time;
    int total_cost;
    int amount_done;
    int queue_number;
    int number;
    int created;
    int quantum_used;
    int done;
};

struct queue{
	int time_quantum;
	// condition variable list or array
};

struct process processes[50];
struct queue queues[50];

void initialize_using_config(){
    FILE * inp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    inp = fopen(".config", "r");
    
    while ((read = getline(&line, &len, inp)) != -1) {
        if(strcmp(line,"NUMBER_OF_QUEUES\n")==0){
            getline(&line, &len, inp);
            no_of_queues = atoi(line);
            printf("no of queue : %d\n", no_of_queues);
        }
        if(strcmp(line,"TIME_SLICES\n")==0){
            int i=1;
            for( ; i <= no_of_queues ; i++){
                getline(&line, &len, inp);
                int slice=atoi(line);
                queues[i].time_quantum = slice;
                printf("time slices : %d\n", queues[i].time_quantum);
            }
        }
        if(strcmp(line,"NO_OF_PROCESSES\n")==0){
            getline(&line, &len, inp);
            no_of_processes = atoi(line);
            printf("number of processes %d\n", no_of_processes);
        }
        if(strcmp(line,"CORRESPONDING_QUEUE_NUMBERS\n")==0){
			int i;
			for( i = 1 ; i <= no_of_processes ; i++){
	        	getline(&line, &len, inp);
				int q = atoi(line);
        		processes[i].queue_number=q;
        		// add corresponding condition variable here
        	}
        }
        if(strcmp(line,"ARRIVAL_TIMES\n")==0){
            int i=1;
            for( ; i <= no_of_processes ; i++){
                getline(&line, &len, inp);
                int time = atoi(line);
                processes[i].amount_done=0;
                processes[i].arrival_time=time;
                // processes[i].queue_number=1;
                processes[i].number=i;
                processes[i].quantum_used=0;
                processes[i].done=0;
            }
        }
        if(strcmp(line,"CORRESPONDING_PROCESSING_TIMES\n")==0){
            int i=1;
            for( ; i <= no_of_processes ; i++){
                getline(&line, &len, inp);
                int time=atoi(line);
                processes[i].total_cost = time;
                printf("corresponding time : %d\n", time);
            }
        }
        if(strcmp(line,"PRIORITY_BOOST\n")==0){
            getline(&line, &len, inp);
            priority_boost_time = atoi(line);
            printf("%d\n", priority_boost_time );
        }
    }

    fclose(inp);
    if (line)
        free(line); 
}

int processes_complete(){
    int i=1;
    for( ; i<=no_of_processes ; i++){
        if(processes[i].amount_done != processes[i].total_cost)
            return 0;
    }
    return 1;
}

void boost_all(){
    int i=1;
    for( ; i<=no_of_processes ; i++)
        processes[i].queue_number = 1;
}

int find_proc(){
    int i=1;
    for( ; i<=no_of_processes ; i++)
        if(processes[i].arrival_time == global_clock && !processes[i].created)
            return i;
    return -1;
}

void * mul_thread(void *vargp){
    // printf("yes in here\n");
    pthread_mutex_lock(&m);
    running=1;
    int * x= (int *)vargp;
    printf("iteration number : %d %d %d\n", *x, global_clock, processes[*x].total_cost);

    while(processes[*x].amount_done != processes[*x].total_cost){
        // printf("reached here\n");
        while(processes[*x].quantum_used < queues[processes[*x].queue_number].time_quantum 
                && processes[*x].amount_done != processes[*x].total_cost){
            
            processes[*x].amount_done++;
            processes[*x].quantum_used++;
            global_clock++;
            printf("done 1 unit of process number : %d \n", *x);

            if(find_proc() != -1){
                pthread_cond_signal(&main_c);
                pthread_cond_wait(&running_c,&m);
            }
        }
        if(processes[*x].amount_done == processes[*x].total_cost){
            processes[*x].quantum_used=0;
            running=0;
            break;
        }
        
        if(processes[*x].queue_number != no_of_queues)
            processes[*x].queue_number++;

        processes[*x].quantum_used=0;
        running=0;
        pthread_cond_signal(&main_c);
        pthread_cond_wait(&c,&m);
    }
    pthread_cond_signal(&main_c);    
    pthread_mutex_unlock(&m);
    // pthread_exit(0);
}

int main()
{
    initialize_using_config();
    pthread_t tid;
    pthread_mutex_lock(&m);
    // printf("%d\n",processes_complete());
    while(!processes_complete()){       

        int procno=find_proc();
        // printf("%d\n",procno );
        while(procno != -1){
            printf("creating thread for %d\n",procno);
            processes[procno].created=1;
            // printf("yes executed till here\n");
            pthread_create(&tid, NULL, mul_thread, &processes[procno].number);
            // printf("process created\n");
            procno=find_proc();    
        }
        if(running)
            pthread_cond_signal(&running_c);
        else
            pthread_cond_signal(&c);
        
        pthread_cond_wait(&main_c,&m);
        printf("Now going to next iter\n");
        printf("Time elapsed : %d\n", global_clock);
        
    }
    pthread_mutex_unlock(&m);
    return 0;
}
