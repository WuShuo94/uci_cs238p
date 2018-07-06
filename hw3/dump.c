#include "types.h"
#include "stat.h"
#include "user.h"
#include "syscall.h"

#define PGSIZE 4096

  /* Fork a new process to play with */
  /* We don't have a good way to list all pids in the system
     so forking a new process works for testing */ 

void tdump() 
{
  int pid = fork();
  int size = PGSIZE+4;
  int addr = 0x10; 
  char *buffer = malloc(size);

  if (pid == 0) {
    /* child spins and yields */
    while (1) {
       sleep(5);
    }
  }

  if(dump(pid, (char*)addr, buffer, size) < 0) {
    printf(1, "size is out of range!\n");
  }

  int i = 0;
  for(; i < (size/16)*16; ) {
    printf(1, "%x: %x  %x  %x  %x\n", addr + i, *((int*)buffer+i/4), *((int*)buffer+i/4+1), *((int*)buffer+i/4+2), *((int*)buffer+i/4+3));
    i += 16;
  }
  if(i < size) {
    printf(1, "%x: ", addr + i);
    for(; i < size; ) {
      printf(1,"%x  ", *((int*)buffer+i/4));
      i += 4;
    }
    printf(1,"\n");
  }
  /* parent dumps memory of the child */
}

int main(int argc, char *argv[])
{
  tdump();
  exit();
}



// To add a system call that can be called in xv6's shell:

// proc.c add the real implementation of your method here
// sysproc.c add system call wrapper to transfer arguments for system call
// syscall.h define the position of the system call vector that connect to your implementation
// user.h define the function that can be called through the shell
// syscall.c external define the function that connect the shell and the kernel, use the position defined in syscall.h to add the function to the system call vector
// usys.S use the macro to define connect the call of user to the system call function
// defs.h add a forward declaration for your new system call

