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
  if(sem_ds==0){
    printf("internal_semPost error semaphore:%d\n",sem_fd);
    return;
  }

  Semaphore* sem= sem_ds->semaphore;
  if(sem->count<=-1){

  	List_insert(&ready_list, ready_list.last, (ListItem*)running);
    SemDescriptorPtr* sem_ptr =(SemDescriptorPtr*) List_detach(&sem->waiting_descriptors, (ListItem*)sem->waiting_descriptors.first);
    List_insert(&sem->descriptors, sem->descriptors.last, (ListItem*)sem_ptr);
    List_detach(&waiting_list, (ListItem*)sem_ptr->descriptor->pcb);

    running->status = Ready;
    running = sem_ptr->descriptor->pcb;
  }
  
  sem->count++;
  return;
}
