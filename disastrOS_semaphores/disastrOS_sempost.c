#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semPost(){
  int sem_fd = running->syscall_args[0];
  SemDescriptors sem_ds=SemDescriptorsListByFd(running->sem_descriptors,sem_fd);

  Semaphore sem= sem_ds->Semaphore;
  if(sem->count<=-1){

  	List_insert(&ready_list, ready_list.last, (ListItem*)running);
    SemDescriptorPtr sem_ptr = List_detach(sem->waiting_descriptors, (ListItem*)sem->waiting_descriptors.first);
    List_insert(sem->descriptors, sem->descriptors.last, (ListItem*)sem_ptr);
    List_detach(waiting_list, (ListItem*)sem_ptr->descriptor->pcb);

    running->status = Ready;
    running = sem_ptr->descriptor->pcb;
  }
  
  sem->count++;
  return;
}
