#include <stdio.h>
#include <unistd.h>
#include <poll.h>

#include "disastrOS.h"

int sem1,sem2;




void  sem_testing(){
	printf("I'm the testing function for semaphore and I'm starting right now\n");

	//test for open close wait and post on a semaphore
	printf("-TEST1-\nOpening Semaphore, expected no error\n");
 	sem1= disastrOS_semOpen(0,1);
 	printf("-TEST1-END-\n");
 	printf("-TEST2-\nDecrementing counter of Semaphore(semwait), expected no error\n");
 	disastrOS_semWait(sem1);
 	printf("-TEST2-END-\n");
  	printf("-TEST3-\nIncrementing counter of Semaphore(sempost), expected no error\n");
  	disastrOS_semPost(sem1);
  	printf("-TEST3-END-\n");
  	printf("-TEST4-\nClosing Semaphore, expected no error\n");
  	disastrOS_semClose(sem1);
  	printf("-TEST4-END-\n");

  	printf("-TEST5-\nOpening Semaphore, then opening another semaphore with the same id, expected no error\n");
  	sem1= disastrOS_semOpen(0,1);
  	sem2= disastrOS_semOpen(0,1);
  	printf("-TEST5-END-\n");

  	printf("-TEST6-\n Closing a Semaphore that has never been opened\nExpected error: unable to find semdescriptor of the semaphore\n");
  	disastrOS_semClose(sem2);
  	printf("-TEST6-END-\n");

  	printf("-TEST7-\n SemPost on a Semaphore that has never been opened\nExpected error: unable to find semdescriptor of the semaphore\n");
  	disastrOS_semPost(sem2);
  	printf("-TEST7-END-\n");

  	printf("-TEST8-\n SemWait on a Semaphore that has never been opened\nExpected error: unable to find semdescriptor of the semaphore\n");
  	disastrOS_semWait(sem2);
  	printf("-TEST8-END-\n");
	
	printf("Shutdown!\n");
 	disastrOS_shutdown();
}


int main(int argc, char** argv){
  char* logfilename=0;
  if (argc>1) {
    logfilename=argv[1];
  }
  printf("start\n");
  disastrOS_start(sem_testing, 0, logfilename);
  return 0;
}

