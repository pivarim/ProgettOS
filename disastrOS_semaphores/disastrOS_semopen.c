#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semOpen(){
  
  int semaphoreCount;
  if(semaphoreCount < 1){
   return; 
  }
  int id;
  if(SemaphoreList_byId(semaphosres_list, id) == 0){
    return;
  }
  Semaphore s = Semaphore_alloc(id, count);
  if(){
  
  }
  
  
}


