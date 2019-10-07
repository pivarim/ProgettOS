#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semClose(){
  int id = running->syscall_args[0];
  SemDescriptor* d = SemDescriptorList_byFd(&running->sem_descriptors, id);
  if(d == NULL){
   printf("ERROR - unable to find semdescriptor of the semaphore with id = %d.\n", id);
   running->syscall_retvalue = DSOS_ESEMCLOSE;
   return;
  }
  Semaphore* s = d->semaphore;
  if(s == NULL){
   printf("ERROR - unable to find semaphore with id = %d from its descriptor.\n", id);
   running->syscall_retvalue = DSOS_ESEMCLOSE;
   return;
  }
  SemDescriptorPtr* dptr = d->ptr;
  if(dptr == NULL){
   printf("ERROR - unable to find pointer to descriptor of semaphore with id = %d from its descriptor.\n", id);
   running->syscall_retvalue = DSOS_ESEMCLOSE;
   return;
  }
  List_detach(&running->sem_descriptors, (ListItem *) d);
  List_detach(&s->descriptors, (ListItem *) dptr);

  SemDescriptor_free(d);
  SemDescriptorPtr_free(dptr);
  if(s->descriptors.size == 0 && s->waiting_descriptors.size==0){
   List_detach(&semaphores_list, (ListItem*)s);
   Semaphore_free(s);
   printf("SUCCESS - Semaphore with id=%d closed.\n",id);
  }
  running->syscall_retvalue = 0;
  return;
}
