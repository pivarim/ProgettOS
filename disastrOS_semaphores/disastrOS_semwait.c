#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semWait(){
  int sem_fd = running->syscall_args[0];
  SemDescriptor* sem_ds=SemDescriptorList_byFd(&running->sem_descriptors,sem_fd);
  //error control for semaphore
  if(sem_ds==0){//
    printf("ERROR -semWait- unable to find semdescriptor of the semaphore :%d\n",sem_fd);
    running->syscall_retvalue = DSOS_ESEMWAIT;
    return;
  }

  Semaphore* sem= sem_ds->semaphore;

  SemDescriptorPtr* sem_ptr= sem_ds->ptr;

  if(sem->count<0){
    //process's descriptor is removed from descriptor list
    List_detach(&sem->descriptors, (ListItem*)sem_ptr);
    //process's descriptor is added in waiting descriptor list
  	List_insert(&sem->waiting_descriptors, sem->waiting_descriptors.last, (ListItem*)sem_ds->ptr);
  	//status of process is changed in waiting
  	running->status = Waiting;
    //process is added in the waiting processes list
  	List_insert(&waiting_list, waiting_list.last, (ListItem*) running);
    //first element of ready list is taken
  	PCB* pcb = (PCB*) List_detach(&ready_list, (ListItem*)ready_list.first);
    //the new running process is the last process taken
  	running = (PCB*)pcb;

  }
  //refresh the counter of the semaphore
  sem->count--;

  printf("SUCCESS - SemWait on semaphore with fd=%d.\n",sem_fd);
  //return value of the systemcall
  running->syscall_retvalue = 0;
  return;
}
