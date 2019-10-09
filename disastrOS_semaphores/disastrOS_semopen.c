#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semOpen(){
  int id = running->syscall_args[0];

  //Checks if semaphore already exists.
  if(SemaphoreList_byId(&semaphores_list, id) != 0){
   printf("ERROR - Semaphore with id = %d already exists.\n", id);
   running->syscall_retvalue = DSOS_ESEMOPEN;
   return;
  }
  
  int semaphoreCount = running->syscall_args[1];

  //Checks if counter is less than 0.
  if(semaphoreCount < 0){
   printf("ERROR - Unable to create semaphore with id = %d: counter is less than 0.\n", id);
   running->syscall_retvalue = DSOS_ESEMOPEN;
   return; 
  }

  //Allocates the semaphore.
  Semaphore* s = Semaphore_alloc(id, semaphoreCount);

  //Checks if semaphore has been allocated.
  if(s == NULL){
   printf("ERROR - Unable to allocate semaphore with id = %d: unknown error.\n", id);
   running->syscall_retvalue = DSOS_ESEMOPEN;
   return;
  }

  //Inserts the semaphore in the list of semaphores.
  List_insert(&semaphores_list, semaphores_list.last, (ListItem*) s);

  //Allocates descriptor of semaphore.
  SemDescriptor* d = SemDescriptor_alloc(running->last_sem_fd, s, running);
  
  //Checks if descriptor has been allocated.
  if(d == NULL){
   printf("ERROR - Unable to allocate descriptor of semaphore with id = %d: unknown error.\n", id);
   running->syscall_retvalue = DSOS_ESEMOPEN;
   return;
  }
  //Allocates pointer to descriptor of semaphore.
  SemDescriptorPtr* sem_descriptorPtr = SemDescriptorPtr_alloc(d);

  //Checks if pointer to descriptor has been allocated.
  if(sem_descriptorPtr == NULL){
   printf("ERROR - Unable to allocate pointer to descriptor of semaphore with id = %d: unknown error.\n", id);
   running->syscall_retvalue = DSOS_ESEMOPEN;
   return;
  }
  //Insterts descriptor in the list inside the running pcb.
  List_insert(&running->sem_descriptors, running->sem_descriptors.last, (ListItem *) d);

  //Increases the int of last semaphore inside the running pcb.
  running->last_sem_fd++;
  
  //Assings the pointer to the descriptor.
  d->ptr = sem_descriptorPtr;

  //Inserts the descriptor in the list inside the semaphore.
  List_insert(&s->descriptors, s->descriptors.last, (ListItem *) sem_descriptorPtr);

  printf("SUCCESS - Opened semaphore with id = %d.\n", id);
  running->syscall_retvalue = d->fd;
  return;
}


