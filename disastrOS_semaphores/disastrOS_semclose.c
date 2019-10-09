#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semClose(){
  int fd = running->syscall_args[0];

  //Fetches the descriptor of semaphore.
  SemDescriptor* d = SemDescriptorList_byFd(&running->sem_descriptors, fd);
  
  //Checks if descriptor actually exists.
  if(d == 0){
   printf("ERROR - unable to find semdescriptor of the semaphore with fd = %d.\n", fd);
   running->syscall_retvalue = DSOS_ESEMCLOSE;
   return;
  }
  
  //Gets the semaphore.
  Semaphore* s = d->semaphore;
  if(s == NULL){
   printf("ERROR - unable to find semaphore with fd = %d from its descriptor.\n", fd);
   running->syscall_retvalue = DSOS_ESEMCLOSE;
   return;
  }

  //Gets the pointer to descriptor of semaphore.
  SemDescriptorPtr* dptr = d->ptr;
  if(dptr == NULL){
   printf("ERROR - unable to find pointer to descriptor of semaphore with fd = %d from its descriptor.\n", fd);
   running->syscall_retvalue = DSOS_ESEMCLOSE;
   return;
  }

  //Unlinks the descriptor from the list of descriptors in the running pcb.
  List_detach(&running->sem_descriptors, (ListItem *) d);

  //Unlinks the pointer to descriptor from the list of pointers in the semaphore.
  List_detach(&s->descriptors, (ListItem *) dptr);
  
  //Releases the memory where descriptor is located.
  SemDescriptor_free(d);

  //Releases the memory where pointer to descriptor is located.
  SemDescriptorPtr_free(dptr);

  //Checks if the process is the last one linked to semaphore, releases the semaphore memory if so.
  if(s->descriptors.size == 0 && s->waiting_descriptors.size==0){
   List_detach(&semaphores_list, (ListItem*)s);
   Semaphore_free(s);
   printf("SUCCESS - Semaphore with fd=%d closed.\n",fd);
  }
  running->syscall_retvalue = 0;
  return;
}
