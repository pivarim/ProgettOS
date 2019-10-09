#include <stdio.h>
#include <unistd.h>
#include <poll.h>

#include "disastrOS.h"

#define BUFFER_LENGTH 50
#define ITERATION_COUNT 10
#define THREADS_NUM 10

int sem_fill, sem_empty, sem_consumer, sem_producer;
int buf[BUFFER_LENGTH];
int write_index = 0;
int read_index = 0;

unsigned long int shared_resource;

int producer(){
    disastrOS_semWait(sem_empty);
    disastrOS_semWait(sem_producer);
    
    int ret = shared_resource;
    buf[write_index] = shared_resource;
    write_index = (write_index+1) % BUFFER_LENGTH;
    shared_resource++;

    disastrOS_semPost(sem_producer);
    disastrOS_semPost(sem_fill);
    
    return ret;
}

int consumer(){
    int x;
    
    disastrOS_semWait(sem_fill);
    disastrOS_semWait(sem_consumer);
    
    x = buf[read_index];
    read_index = (read_index+1) % BUFFER_LENGTH;
    
    disastrOS_semPost(sem_consumer);
    disastrOS_semPost(sem_empty);
    
    return x;
}

// we need this to handle the sleep state
void sleeperFunction(void* args){
  printf("Hello, I am the sleeper, and I sleep %d\n",disastrOS_getpid());
  while(1) {
    getc(stdin);
    disastrOS_printStatus();
  }
}



void childFunction(void* args){
  printf("Hello, I am the child function %d\n",disastrOS_getpid());
  printf("I will test the semaphore before terminating\n");

  sem_fill = disastrOS_semOpen(1, 0);  
  sem_empty = disastrOS_semOpen(2, BUFFER_LENGTH); 
  sem_consumer = disastrOS_semOpen(3, 1);  
  sem_producer = disastrOS_semOpen(4,1); 

  //disastrOS_printStatus();
  for(int i=0; i < ITERATION_COUNT; i++){
    if (disastrOS_getpid() % 2 == 0){
     int sv = producer();
     printf("Child %d: write in buffer value: %d\n", disastrOS_getpid(), sv);
     }
    else {
     int ret = consumer();
     printf("Child %d: read from buffer value:%d\n", disastrOS_getpid(), ret);
    }
  }

  printf("Child %d is terminating\n", disastrOS_getpid());

  disastrOS_semClose(sem_fill);
  disastrOS_semClose(sem_empty);
  disastrOS_semClose(sem_consumer);
  disastrOS_semClose(sem_producer);

  disastrOS_exit(disastrOS_getpid()+1);
}


void initFunction(void* args) {
  disastrOS_printStatus();
  printf("Hello, I am init and I just started\n");

  shared_resource=1;
  
  printf("I feel like to spawn %d nice threads\n",THREADS_NUM);
  int alive_children=0;
  for (int i=0; i<THREADS_NUM; ++i) {
    disastrOS_spawn(childFunction, 0);
    alive_children++;
  }

  disastrOS_printStatus();
  int retval;
  int pid;

  while(alive_children>0 && (pid=disastrOS_wait(0, &retval))>=0){ 
    disastrOS_printStatus();
    printf("initFunction, child: %d terminated, retval:%d, alive: %d \n",
	   pid, retval, alive_children);
    alive_children--;
  }

  printf("shutdown!");
  disastrOS_shutdown();
}



int main(int argc, char** argv){
  char* logfilename=0;
  if (argc>1) {
    logfilename=argv[1];
  }
  // we create the init process processes
  // the first is in the running variable
  // the others are in the ready queue
  printf("the function pointer is: %p", childFunction);
  // spawn an init process
  printf("start\n");
  disastrOS_start(initFunction, 0, logfilename);
  return 0;
}
