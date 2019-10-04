#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semOpen(){
  int id = running->syscall_args[0];
  if(SemaphoreList_byId(&semaphores_list, id) == 0){
   printf("ERROR - Semaphore with id = %d already exists.", id);
   return;
  }
  int semaphoreCount = running->syscall_args[1];
  if(semaphoreCount < 0){
   printf("ERROR - Unable to create semaphore with id = %d: counter is less than 0.", id);
   return; 
  }
  Semaphore* s = Semaphore_alloc(id, semaphoreCount);
  if(s == NULL){
   printf("ERROR - unable to allocate semaphore with id = %d: unknown error.", id);
   return;
  }
  SemDescriptor* d = SemDescriptor_alloc(running->last_sem_fd, s, running);
  printf("ERROR - unable to allocate descriptor of semaphore with id = %d: unknown error.", id);
   return;
  }
  SemDescriptorPtr* sem_descriptorPtr = SemDescriptorPtr_alloc(d);
  printf("ERROR - unable to allocate pointer to descriptor of semaphore with id = %d: unknown error.", id);
   return;
  }
  List_insert(&running->sem_descriptors, running->sem_descriptors.last, (ListItem *) d);
  running->last_sem_fd++;

  d->ptr = sem_descriptorPtr;
  List_insert(&s->descriptors, s->descriptors.last, (ListItem *) sem_descriptorPtr);
  running->syscall_retvalue = d->fd;
  return;
}


