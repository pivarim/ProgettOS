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
  if(sem_ds==0){
    printf("internal_semWait error, semaphore:%d\n",sem_fd);
    running->syscall_retvalue = DSOS_ESEMWAIT;
    return;
  }

  Semaphore* sem= sem_ds->semaphore;

  SemDescriptorPtr* sem_ptr= sem_ds->ptr;

  if(sem->count<0){

    List_detach(&sem->descriptors, (ListItem*)sem_ptr);

  	List_insert(&sem->waiting_descriptors, sem->waiting_descriptors.last, (ListItem*)sem_ds->ptr);
  	
  	running->status = Waiting;

  	List_insert(&waiting_list, waiting_list.last, (ListItem*) running);

  	PCB* pcb = (PCB*) List_detach(&ready_list, (ListItem*)ready_list.first);

  	running = (PCB*)pcb;

  }
  sem->count--;
  running->syscall_retvalue = 0;//return value of the systemcall
  return;
}
