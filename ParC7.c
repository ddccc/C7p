/*
Copyright (c) 2019, Dennis de Champeaux.  All rights reserved.

The copyright holders hereby grant to any person obtaining a copy of
this software (the "Software") and/or its associated documentation
files (the Documentation), the irrevocable (except in the case of
breach of this license) no-cost, royalty free, rights to use the
Software for non-commercial evaluation/test purposes only, while
subject to the following conditions:

. Redistributions of the Software in source code must retain the above
copyright notice, this list of conditions and the following
disclaimers.

. Redistributions of the Software in binary form must reproduce the
above copyright notice, this list of conditions and the following
disclaimers in the documentation and/or other materials provided with
the distribution.

. Redistributions of the Documentation must retain the above copyright
notice, this list of conditions and the following disclaimers.

The name of the copyright holder, may not be used to endorse or
promote products derived from this Software or the Documentation
without specific prior written permission of the copyright holder.
 
THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS", WITHOUT WARRANTY
OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE CONTRIBUTORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
IN CONNECTION WITH THE SOFTWARE OR DOCUMENTATION OR THE USE OF OR
OTHER DEALINGS WITH THE SOFTWARE OR DOCUMENTATION.
*/

// File: c:/bsd/rigel/sort/C7/ParC7.c
// Date: Tue Oct 15 17:16:33 2019
/* This file has the source of the algorithms that make up ParC7
   headed by c7p
*/
/* compile with: (or use the -O3 param)
   gcc -c ParC7.c
   gcc -c ParC7.c
*/

/*  // include if used in a different context
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
*/

#define errexit(code,str)                          \
  fprintf(stderr,"%s: %s\n",(str),strerror(code)); \
  exit(1);


/* // include in a different context
#include "Hsort.c"
#include "Dsort.c"
#include "Isort.c"
// 
#include "Qusort.c"
#include "C2sort.c" 
*/

struct stack *ll;
struct task *newTask();
void addTaskSynchronized();
//#include "Cut7p.c"

void cut7pc();
void *sortThread7(void *AAA) { // AAA-argument is NOT used
  // int taskCnt = 0;
  //  printf("Thread number: %ld #sleepers %d\n", 
  //         pthread_self(), sleepingThreads);
  struct task *t = NULL;
  for (;;) {
    pthread_mutex_lock( &condition_mutex2 );
       sleepingThreads++;
       while ( NULL == ( t = pop(ll) ) && 
	       sleepingThreads < NUMTHREADS ) {
	 pthread_cond_wait( &condition_cond2, &condition_mutex2 );
       }
       if ( NULL != t ) sleepingThreads--;
    pthread_mutex_unlock( &condition_mutex2 );
    if ( NULL == t ) { 
      pthread_mutex_lock( &condition_mutex2 );
      pthread_cond_signal( &condition_cond2 );
      pthread_mutex_unlock( &condition_mutex2 );
      break;
    }
    void **A = getA(t);
    int n = getN(t);
    int m = getM(t);
    int depthLimit = getDL(t);
    int (*compare)() = getXY(t);
    free(t);
    // taskCnt++;
    cut7pc(A, n, m, depthLimit, compare);
  }

  //  printf("Exit of Thread number: %ld taskCnt: %d\n", pthread_self(), taskCnt);
  return NULL;
} // end sortThread

#define iswap(p, q, A) { void *t3t = A[p]; A[p] = A[q]; A[q] = t3t; }


int cut2SLimit = 2000;
void c7pc();
void c7p(void **A,  int lo, int hi,
	int (*compareXY ) (const void *, const void * ),
	int numberOfThreads) {
  // printf("c7p size %i nT %i\n", size, numberOfThreads);
  int L = hi - lo;
  if ( L <= cut2SLimit  || numberOfThreads <= 1 ) {
    // quicksort0(A, 0, size-1, compareXY);
    cut7(A, lo, hi, compareXY);
    return;
  }
  int depthLimit = 2.9 * floor(log(L));
  c7pc(A, lo, hi, depthLimit, compareXY, numberOfThreads);
}

void c7pc(void **A, int lo, int hi, int depthLimit,
	  int (*compareXY) (const void *, const void * ),
	  int numberOfThreads) {

  int L = hi-lo+1;
  // printf("c7pc %i #T %i \n ", L, numberOfThreads);
  // Proceed !

  sleepingThreads = 0;
  NUMTHREADS = numberOfThreads;
  pthread_t thread_id[NUMTHREADS];
  ll = newStack();
  // Try doing the first partition in parallel with two threads
  int N = lo; int M = hi; 
  
  // Check for duplicates
  int sixth = L / 6;
  int e1 = N  + sixth;
  int e5 = M - sixth;
  int e3 = N + L/2; // The midpoint
  // int e3 = (N+M) / 2; // The midpoint
  int e4 = e3 + sixth;
  int e2 = e3 - sixth;
  
  // Sort these elements using a 5-element sorting network
  void *ae1 = A[e1], *ae2 = A[e2], *ae3 = A[e3], *ae4 = A[e4], *ae5 = A[e5];
  void *t;
  // if (ae1 > ae2) { t = ae1; ae1 = ae2; ae2 = t; }
  if ( 0 < compareXY(ae1, ae2) ) { t = ae1; ae1 = ae2; ae2 = t; } // 1-2
  if ( 0 < compareXY(ae4, ae5) ) { t = ae4; ae4 = ae5; ae5 = t; } // 4-5
  if ( 0 < compareXY(ae1, ae3) ) { t = ae1; ae1 = ae3; ae3 = t; } // 1-3
  if ( 0 < compareXY(ae2, ae3) ) { t = ae2; ae2 = ae3; ae3 = t; } // 2-3
  if ( 0 < compareXY(ae1, ae4) ) { t = ae1; ae1 = ae4; ae4 = t; } // 1-4
  if ( 0 < compareXY(ae3, ae4) ) { t = ae3; ae3 = ae4; ae4 = t; } // 3-4
  if ( 0 < compareXY(ae2, ae5) ) { t = ae2; ae2 = ae5; ae5 = t; } // 2-5
  if ( 0 < compareXY(ae2, ae3) ) { t = ae2; ae2 = ae3; ae3 = t; } // 2-3
  if ( 0 < compareXY(ae4, ae5) ) { t = ae4; ae4 = ae5; ae5 = t; } // 4-5
  // ... and reassign
  A[e1] = ae1; A[e2] = ae2; A[e3] = ae3; A[e4] = ae4; A[e5] = ae5;

  // Fix end points
  if ( compareXY(ae1, A[N]) < 0 ) iswap(N, e1, A);
  if ( compareXY(A[M], ae5) < 0 ) iswap(M, e5, A);
  
  void *T = ae3; // pivot

  // check Left label invariant
  // if ( T <= A[N] || A[M] < T ) {
  if ( compareXY(T, A[N]) <= 0 || compareXY(A[M], T) < 0 ) {
    // cannot do first parallel partition
    struct task *t = newTask(A, lo, hi, depthLimit, compareXY);
    addTaskSynchronized(ll, t);
  } else {
    /*
      |------------------------|------------------------|
      N                        e3                       M
      A[N] < T             A[e3] = T                 T<=A[M]
    */
    struct task *t1 = newTask(A, N, e3, 0, compareXY);
    struct task *t2 = newTask(A, e3, M, 0, compareXY);
    int errcode;
    if ( (errcode=pthread_create(&thread_id[1], NULL, 
				 partitionThreadLeft, (void*) t1) )) {
      errexit(errcode,"ParC7/C7p()/pthread_create");
    }
    if ( (errcode=pthread_create(&thread_id[2], NULL, 
				 partitionThreadRight, (void*) t2)) ) {
      errexit(errcode,"ParC7/C7p()/pthread_create");
    }
    if ( (errcode=pthread_join(thread_id[1], NULL) )) {
      errexit(errcode,"ParC7/C7p()/pthread_join");
    }
    int i1 = getN(t1); free(t1);
    if ( (errcode=pthread_join(thread_id[2], NULL) )) {
      errexit(errcode,"ParC7/C7p()/pthread_join");
    }
    int i2 = getN(t2); free(t2);
    /*      LL            LR           RL           RR
       |--------------]----------][-----------]-----------------|
       N     <        i1   >=    e3    <      i2      >=        M
       i2 = e3 -> RL is empty
    */
    int k; int m3;
    if ( e3 == i2 ) { m3 = i1; } else {
      int middle2 = e3+1;
      // int b = middle - i1; int c = i2 - middle2;
      int b = e3 - i1; int c = i2 - middle2;
      // swap the two middle segments
      if ( b <= c ) {
	// printf("b <= c\n");
	for ( k = 0; k < b; k++ ) // iswap(e3-k, i2-k, A);
	  { int xx = e3-k, yy = i2-k; iswap(xx, yy, A); } 
	m3 = i2 - b;
      }
      else {
	// printf("c < b\n");
	for ( k = 0; k < c+1; k++ ) // iswap(middle2+k, i1+1+k, A);
	  { int xx = middle2+k, yy = i1+1+k; iswap(xx, yy, A); } 
	m3 = i1 + c+1;
      }
    }
    /*
      |------------------------][----------------------------|
      N           <           m3           >=                M
     */
    t1 = newTask(A, N, m3, depthLimit, compareXY);
    addTaskSynchronized(ll, t1);
    t1 = newTask(A, m3+1, M, depthLimit, compareXY);
    addTaskSynchronized(ll, t1);
  }
  // printf("Entering sortArray\n");
  int i;
  int errcode;
  for ( i = 0; i < NUMTHREADS; i++ ) {
    if ( errcode=pthread_create(&thread_id[i], NULL, 
				sortThread7, (void*) A) ) {
      errexit(errcode,"ParC7/C7p()/pthread_create");
    }
  }

  for ( i = 0; i < NUMTHREADS; i++ ) {
    if ( errcode=pthread_join(thread_id[i], NULL) ) {
      errexit(errcode,"ParC7/C7p/pthread_join");
    }
  }
  free(ll);
  // printf("Exiting ParC7\n");

} // end c7pc

