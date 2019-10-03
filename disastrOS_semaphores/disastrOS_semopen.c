#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semOpen(){
  int id = running->syscall_args[0];
  if(SemaphoreList_byId(semaphores_list, id) == 0){
   return;
  }
  int semaphoreCount = running->syscall_args[1];
  if(semaphoreCount < 1){
   return; 
  }
  Semaphore* s = Semaphore_alloc(id, count);
  if(s == NULL){
   return;
  }
  Descriptor* d = Descriptor_alloc(running->last_sem_fd, s, running);
  List_insert(running->sem_descriptors, sem_descriptors.last, d);
  
  
}


