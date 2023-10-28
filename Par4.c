// File: c:/bsd/rigel/sort/C7p/Par4.c
// Date: Fri Oct 20 15:01:32 2023
// (C) OntoOO/ Dennis de Champeaux


/*
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
/* // include elsewhere for this setting
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

/*  // include elsewhere for this setting
#include "Isort.c"
#include "Hsort.c"
#include "Qusort.c"
#include "Dsort.c"
*/

// #include "C2sort.c" // cut2 member

struct stack *ll;
struct task * newTask();
void addTaskSynchronized();

// */ 
// threads execute sortThread
void cut4Pc(); 
void *sortThread4(void *AAA) { // AAA is not used
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
    cut4Pc(A, n, m, depthLimit, compare); 
  }
  //  printf("Exit of Thread number: %ld taskCnt: %d\n", pthread_self(), taskCnt);
  return NULL;
} // end sortThread4

#define iswap(p, q, A) { void *t3t = A[p]; A[p] = A[q]; A[q] = t3t; }


int cut4pLimit = 2000;
void c4pc();
void c4p(void **A, int lo, int hi,  
	 int (*compareXY) (const void *, const void * ),
	 int numberOfThreads ) {
	 // int numberOfThreads) {
  int L = hi - lo;
  if ( L <= cut4pLimit  || numberOfThreads <= 1 ) {
    // quicksort0(A, 0, size-1, compareXY);
    // cut2lr(A, lo, hi, compareXY);
    cut4(A, lo, hi, compareXY);
    return;
  }
  int depthLimit = 2.9 * floor(log(L));
  c4pc(A, lo, hi, depthLimit, compareXY, numberOfThreads);
}

void c4pc(void **A, int lo, int hi, int depthLimit,
	  int (*compareXY) (const void *, const void * ),
	  int numberOfThreads) {

  int size = hi-lo+1;
  // printf("c4pc %i #T %i \n ", size, numberOfThreads);
  // Proceed !

  sleepingThreads = 0;
  NUMTHREADS = numberOfThreads;
  pthread_t thread_id[NUMTHREADS];
  ll = newStack();
  //  int depthLimit = 2.9 * floor(log(size));

  // Try doing the first partition in parallel with two threads
  int L = hi-lo +1;

  // Check for duplicates
  int sixth = L / 6;
  int e1 = lo  + sixth;
  int e5 = hi - sixth;
  int e3 = lo + L/2; // The midpoint
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
  if ( compareXY(ae1, A[lo]) < 0 ) iswap(lo, e1, A);
  if ( compareXY(A[hi], ae5) < 0 ) iswap(hi, e5, A);
  
  void *T = ae3; // pivot

  // check Left label invariant
  // if ( T <= A[N] || A[M] < T ) {
  if ( compareXY(T, A[lo]) <= 0 || compareXY(A[hi], T) < 0 ) {
    // cannot do first parallel partition
    struct task *t = newTask(A, lo, hi, depthLimit, compareXY);
    addTaskSynchronized(ll, t);
  } else {
    /*
      |------------------------|------------------------|
      N                        e3                       M
      A[N] < T             A[e3] = T                 T<=A[M]
    */

    struct task *t1 = newTask(A, lo, e3, 0, compareXY);
    struct task *t2 = newTask(A, e3, hi, 0, compareXY);
    int errcode;
    if ( (errcode=pthread_create(&thread_id[1], NULL, 
				 partitionThreadLeft, (void*) t1) )) {
      errexit(errcode,"Par4/c4p()/pthread_create 1");
    }
    if ( (errcode=pthread_create(&thread_id[2], NULL, 
				 partitionThreadRight, (void*) t2)) ) {
      errexit(errcode,"Par4/c4p()/pthread_create 2");
    }

    if ( (errcode=pthread_join(thread_id[1], NULL) )) {
      errexit(errcode,"Par4/c4p()/pthread_join 1");
    }
    int i1 = getN(t1); free(t1);
    if ( (errcode=pthread_join(thread_id[2], NULL) )) { 
      errexit(errcode,"Par4/c4p()/pthread_join 2");
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

    t1 = newTask(A, lo, m3, depthLimit, compareXY);
    addTaskSynchronized(ll, t1);
    t1 = newTask(A, m3+1, hi, depthLimit, compareXY);
    addTaskSynchronized(ll, t1);
  }
  // printf("Entering sortArray D4\n");
  int i;
  int errcode;
  for ( i = 0; i < NUMTHREADS; i++ ) {
    if ( errcode=pthread_create(&thread_id[i], NULL, 
				sortThreadd4, (void *)A ) ) {
      errexit(errcode,"Par4/ c4p()/pthread_create");
    }
  }

  for ( i = 0; i < NUMTHREADS; i++ ) {
    if ( errcode=pthread_join(thread_id[i], NULL) ) {
      errexit(errcode,"Par4/ c4p()/pthread_join");
    }
  }
  free(ll);

} // end c4pc

#undef iswap
