#include "types.h"
#include "stat.h"
#include "user.h"
#include "x86.h"

struct balance {
    char name[32];
    int amount;
};

struct thread_spinlock {
  uint locked;       // Is the lock held?
};

struct thread_mutex {
  uint locked;
};

void thread_spin_init(struct thread_spinlock *lk)
{
  lk->locked = 0;
}

void thread_spin_lock(struct thread_spinlock *lk)
{
  while(xchg(&lk->locked, 1) != 0)
    ;

  __sync_synchronize();
}

void thread_spin_unlock(struct thread_spinlock *lk)
{
  __sync_synchronize();

  asm volatile("movl $0, %0" : "+m" (lk->locked) : );
}

void thread_mutex_init(struct thread_mutex *mutex)
{
  mutex->locked = 0;
}

void thread_mutex_lock(struct thread_mutex *mutex)
{
  while(xchg(&mutex->locked, 1) != 0)
    sleep(1);

  __sync_synchronize();
}

void thread_mutex_unlock(struct thread_mutex *mutex)
{
  __sync_synchronize();

  asm volatile("movl $0, %0" : "+m" (mutex->locked) : );
}

struct thread_spinlock lock;
//struct thread_mutex mutex;

volatile int total_balance = 0;

volatile unsigned int delay (unsigned int d) {
   unsigned int i; 
   for (i = 0; i < d; i++) {
       __asm volatile( "nop" ::: );
   }

   return i;   
}

void do_work(void *arg){
    int i; 
    int old;
   
    struct balance *b = (struct balance*) arg; 
    printf(1, "Starting do_work: s:%s\n", b->name);

    for (i = 0; i < b->amount; i++) { 
         thread_spin_lock(&lock);
         //thread_mutex_lock(&mutex);
         old = total_balance;
         delay(100000);
         total_balance = old + 1;
         thread_spin_unlock(&lock);
         //thread_mutex_unlock(&mutex);
    }
  
    printf(1, "Done s: %s, total_balance = %d\n", b->name, total_balance);

    thread_exit();
    return;
}

int main(int argc, char *argv[]) {

  struct balance b1 = {"b1", 3200};
  struct balance b2 = {"b2", 2800};
 
  void *s1, *s2;
  int t1, t2, r1, r2;

  s1 = malloc(4096);
  s2 = malloc(4096);

  thread_spin_init(&lock);
  //thread_mutex_init(&mutex);

  t1 = thread_create(do_work, (void*)&b1, s1);
  t2 = thread_create(do_work, (void*)&b2, s2); 

  r1 = thread_join();
  r2 = thread_join();
  
  printf(1, "Threads finished: (%d):%d, (%d):%d, shared balance:%d\n", 
      t1, r1, t2, r2, total_balance);

  exit();
}
