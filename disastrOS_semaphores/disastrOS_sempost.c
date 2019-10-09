#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semPost(){
  int sem_fd = running->syscall_args[0];

  SemDescriptor* sem_ds=SemDescriptorList_byFd(&running->sem_descriptors,sem_fd);
  // error control for the semaphore
  if(sem_ds==0){
    printf("ERROR -semPost- unable to find semdescriptor of the semaphore :%d\n",sem_fd);
    running->syscall_retvalue = DSOS_ESEMPOST;
    return;
  }

  Semaphore* sem= sem_ds->semaphore;

  if(sem->count<=-1){//if the count is below -1 i have to take and start the next process
  	//this process is added to the tail of the ready list
    List_insert(&ready_list, ready_list.last, (ListItem*)running);
    //first descriptor of the semaphore in the waiting list is removed from the list and it is stored
    SemDescriptorPtr* sem_ptr =(SemDescriptorPtr*) List_detach(&sem->waiting_descriptors, (ListItem*)sem->waiting_descriptors.first);
    //the descriptor is inserted in the descriptor list of semaphore
    List_insert(&sem->descriptors, sem->descriptors.last, (ListItem*)sem_ptr);
    //the process in removed from the waiting list
    List_detach(&waiting_list, (ListItem*)sem_ptr->descriptor->pcb);
    //the status of the running process is canged to ready
    running->status = Ready;
    //the next running process is the process removed from the top of waiting list
    running = sem_ptr->descriptor->pcb;
  }
  //refresh the counter of the semaphore
  sem->count++;
  printf("SUCCESS - SemPost on semaphore with id=%d.\n",sem_fd);
  //return value of the systemcall
  running->syscall_retvalue = 0;
  return;
}
