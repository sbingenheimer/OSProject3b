#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"
#include "mmu.h"
#include "stddef.h"



char*
strcpy(char *s, const char *t)
{
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
    ;
  return os;
}

int
strcmp(const char *p, const char *q)
{
  while(*p && *p == *q)
    p++, q++;
  return (uchar)*p - (uchar)*q;
}

uint
strlen(const char *s)
{
  int n;

  for(n = 0; s[n]; n++)
    ;
  return n;
}

void*
memset(void *dst, int c, uint n)
{
  stosb(dst, c, n);
  return dst;
}

char*
strchr(const char *s, char c)
{
  for(; *s; s++)
    if(*s == c)
      return (char*)s;
  return 0;
}

char*
gets(char *buf, int max)
{
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
    cc = read(0, &c, 1);
    if(cc < 1)
      break;
    buf[i++] = c;
    if(c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
  return buf;
}

int
stat(const char *n, struct stat *st)
{
  int fd;
  int r;

  fd = open(n, O_RDONLY);
  if(fd < 0)
    return -1;
  r = fstat(fd, st);
  close(fd);
  return r;
}

int
atoi(const char *s)
{
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
    n = n*10 + *s++ - '0';
  return n;
}

void*
memmove(void *vdst, const void *vsrc, int n)
{
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  while(n-- > 0)
    *dst++ = *src++;
  return vdst;
}

int thread_create(void (*start_routine)(void *, void *), void *arg1, void *arg2){
  //free space that will be check for page allignment and a stack pointer that will be 
  //set after page alignment is confirmed
  void * stackspace = malloc(PGSIZE);
  if (stackspace == NULL){
    return -1;
  }
  void * stack;

  //setting up pagealignment
  if (((uint)stackspace % PGSIZE) == 0){
    stack = stackspace;
  }else{
    stack = stackspace + (PGSIZE - ((uint)stackspace - PGSIZE));
  }

  //call clone
  int toret = clone(start_routine, arg1, arg2, stack);

  if (toret != -1){
    return toret;
  }else {
    return -1;
  }
}

int thread_join() {
  //create a stack variable which will be set in join
  void * stack;
  int toret = join(&stack);
  if (toret != -1){
    return toret;
  }
  else{
    return -1;
  }
}

// Lock functions as seen in spinlock.c
void lock_init(lock_t *lock){
  lock->locked = 0;
}

void lock_acquire(lock_t *lock){
 while(xchg(&lock->locked, 1) != 0)
    ;
}

void lock_release(lock_t *lock){
  xchg(&lock->locked, 0);
}