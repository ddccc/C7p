// File: c:/bsd/rigel/sort/C7p/ParStuff.c
// Date: Mon Oct 16 16:40:46 2023
// (C) Dennis de Champeaux/ OntoOO

/*
  Infrastructure for the parallel versions.
  Also for doing the first partition in parallel.
 */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

static int sleepingThreads = 0;
static int NUMTHREADS;
struct stack *ll;
struct task * newTask();
void addTaskSynchronized();

// #include "C2psort.c" // +++ replace with 1-pivot version

pthread_mutex_t condition_mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_cond2  = PTHREAD_COND_INITIALIZER;

void addTaskSynchronized(struct stack *ll, struct task *t) {
  // printf("Thread#: %ld ", pthread_self());
  // printf("addTask N: %d M: %d sleepingThreads: %d\n", 
  //	 getN(t), getM(t), sleepingThreads);

  pthread_mutex_lock( &condition_mutex2 );
  push(ll, t);
  pthread_cond_signal( &condition_cond2 );
  pthread_mutex_unlock( &condition_mutex2 );
} // end addTaskSynchronized

#define iswap(p, q, A) { void *t3t = A[p]; A[p] = A[q]; A[q] = t3t; }
  /*
    |------------------------|
    N                        M 
    A[N] < T             A[M] = T 
   */
int partitionLeft(void **A, int N, int M, int (*compareXY)()) { 
  int i = N; int j = M; void *T = A[M];
 again:
  while ( compareXY(A[++i], T) < 0 ); // T <= A[i]
  while ( compareXY(T, A[--j]) <= 0 ); // A[j] < T
  if ( i < j ) {
      iswap(i, j, A);
      goto again;
  }
  return j;
} // end partitionLeft

void *partitionThreadLeft(void *ptr) {
  struct task *tx = ( struct task * ) ptr;
  void **A = getA(tx);
  int n = getN(tx);
  int m = getM(tx);
  // int T = getDL(tx);
  int (*compare)() = getXY(tx);
  int ix = partitionLeft(A, n, m, compare);
  setN(tx, ix);
} // end partitionThreadLeft
  /*
    |------------------------|
    N                        M 
    A[N] = T             T <= A[M] 
    Do NOT change A[N]; do allow that no elements < T
   */
int partitionRight(void **A, int N, int M, int (*compareXY)()) { 
  int i = N; int j = M; void *T = A[N]; 
  while ( compareXY(A[++i], T) < 0 ); // T <= A[i]
  while ( i <= j && compareXY(T, A[--j]) <= 0 ); // j<i or A[j] < T
  if ( j < i ) { // j = N -> no elements < T
    return j;
  }
  // i < j
  iswap(i, j, A);
  // both sides not empty
 again:
  while ( compareXY(A[++i], T) < 0 ); // T <= A[i]
  while ( compareXY(T, A[--j]) <= 0 ); // A[j] < T
  if ( i < j ) {
      iswap(i, j, A);
      goto again;
  }
  return j;
} // end partitionRight

void *partitionThreadRight(void *ptr) {
  struct task *tx = ( struct task * ) ptr;
  void **A = getA(tx);
  int n = getN(tx);
  int m = getM(tx);
  // int T = getDL(tx);
  int (*compare)() = getXY(tx);
  int ix = partitionRight(A, n, m, compare);
  setN(tx, ix);
} // end partitionThreadRight

#undef iswap
