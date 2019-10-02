#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semWait(){
  int sem_fd = running->syscall_args[0];
  SemDescriptors sem_ds=SemDescriptorsListByFd(running->sem_descriptors,sem_fd);

  Semaphore sem= sem_ds->Semaphore;

  SemDescriptorPtr sem_ptr= sem_ds->ptr;

  if(sem->count<0){

  	List_insert(sem->waiting_descriptors, sem->waiting_descriptors.last, (ListItem*)sem_ds->ptr);
  	
  	running->status = Waiting;

  	List_insert(waiting_list, waiting_list.last, (ListItem*) running);

  	PCB* pcb = List_detach(&ready_list, (ListItem*)ready_list.first);

  	running = (PCB*)pcb;

  }

  return;
}
