// File: c:/bsd/rigel/sort/C7/ComparisonCountingp.c
// Date: Sat Oct 28 11:26:16 2023
// (C) OntoOO/ Dennis de Champeaux

/*
How to use this driver:
Compile with: 
     gcc -O2 ComparisonCountingp.c  <==
Execute with: ./a.exe

The main function contains lines with the calls of the different 
algorithms.
Use siz to set the array size to be used.
The procedure fillarray allows to generate arrays of different sizes 
with different content.  
Validation functions are at the end.  They compare a version against  
the trusted version cut2 
*/


// Counting timings but NOT comparisons.


#include <stdio.h>
#include <stddef.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
// #include <strings.h>
#include <sys/time.h>
#include <math.h>


// Example of objects that can be used to populate an array to be sorted:
  // To obtain the int field from X: ((struct intval *) X)->val
  // To obtain the float field from X: ((struct intval *) X)->valf
typedef struct intval {
  int val;
  float valf;
} *Data;

// /* 
// These are included by the includes below
// #include "Isort.c"
#include "Isort.h"
#include "Hsort.h"
#include "Dsort.h"  // dflgm member
#include "D3sort.h" 

#include "C2sort.c" // support module
#include "C2LR.c" // support module when #threads = 1
#include "CD4.c" // support module when #threads = 1
#include "C4.c" // support module when #threads = 1
#include "C7.c" // support module when #threads = 1
/*
#include "C4.h"   // cut4 member
#include "CN4.h"  // ct4n
#include "CD4.h"  // cut4d
#include "C7.h"   // cut7
*/
// */
#include "Qstack.c"
#include "ParStuff.c"

#include "ParC2.c" 
#include "C2LRp.c" 
#include "ParD4.c" 
#include "CD4p.c" 
#include "Par4.c" 
#include "C4p.c" 
#include "ParC7.c"
#include "C7xp.c" 

void validateAlg();
void validateAlgP2();
void validateAlgPD4();
void validateAlgP4();
void validateAlgP7();

// double cnt = 0; // comparison counter
long long cnt = 0;
// Here an exmple comparison function for these objects:
// **** NOTE, again **** 
//      All the examples below use ::::
//          the intval objects, and 
//          the compareXY comparison function
int compareXY (const void *a, const void *b)
{
  extern long long cnt;
  // cnt++; // threads compete for access causing slowdown and undercount
  return ((struct intval *)a)->val - ((struct intval *)b)->val;
}

void cc();

int main (int argc, char *argv[]) {
  printf("Running ComparisonCountingp ...\n");
     // 1 pivot
  // cc("cut2   ", cut2, compareXY, 1, 0);
     // 3 pivot
  // cc("mpq    ", part3, compareXY, 1, 0);
  // cc("cut4   ", cut4, compareXY, 1, 0);
  // cc("c7     ", cut7, compareXY, 1, 0);
     // multi threaded versions; last argument is the # threads
  int nt = 2; // thread count; if =1 then sequential version
  cc("c2p    ", c2p, compareXY, 1, nt);
  cc("c4p    ", c4p, compareXY, 1, nt);
  cc("c7p    ", c7p, compareXY, 1, nt);
  cc("cd4p    ", cd4p, compareXY, 1, nt);

     // Misc
  // testAlg();
  // validateAlgP2();
  // validateAlgPD4();
  // validateAlgP4();
  // validateAlgP7();
  return 0;
} // end main

void *myMalloc(char* location, int size) {
  void *p = malloc(size);
  if ( 0 == p ) {
    fprintf(stderr, "malloc fails for: %s\n", location);
    exit(1);
  }
  return p;
} // end myMalloc

// fillarray assigns random values to the int-field of our objects
void fillarray(void **A, int lng, int startv) {
  // printf("fillarray startv %i\n", startv);
  int range = 32*1024*1024; // restrict the largest number
  // int range = 128; // limited # of different elements
  int i;
  struct intval *pi;

  srand(startv);
  for ( i = 0; i < lng; i++) {
    pi = (struct intval *)A[i];
    pi->val = rand()%range; 
    // pi->val = 0; 
    // pi->val = i; 
    // pi->val = -i; 
  }
  // */
  /*
  // permutation of [0, lng) to avoid duplicates
  srand(startv);
  for ( i = 0; i < lng; i++) {
    pi = (struct intval *)A[i];
    pi->val = i; 
  }
  int j;
  for ( i = lng-1; 0<i ; i--) {
    j = rand()%i;
    iswap(i, j, A);
  }
  */
} // end of fillarray

long long comparisons; double clocktime; 

void countcomparisons(int siz2, void (*alg1)(), 
		      int (*compar )(), int seed, int bool, int numThreads) {
  extern long long cnt,  comparisons;
  extern double clocktime;
  double algTime, T;
  int siz = siz2;
  printf(" on size: %d \n", siz);
  // construct array
  struct intval *pi;
  void **A = myMalloc("timeTest 1", sizeof(pi) * siz);
  int i;
  for (i = 0; i < siz; i++) {
    pi = myMalloc("timeTest 2", sizeof (struct intval));
    A[i] = pi;
  };

  int reps = 10;
  // int reps = 1;
  // warm up the process
  for (i = 0; i < reps; i++) fillarray(A, siz, seed); 
  // measure the array fill time
  // int TFill = clock();
    
      struct timeval tim;
      gettimeofday(&tim, NULL);
      double TFILL=tim.tv_sec+(tim.tv_usec/1000000.0);
    
  for (i = 0; i < reps; i++) fillarray(A, siz, seed+i);
      // here alternative ways to fill the array
      // int k;
      // for ( k = 0; k < siz; k++ ) A[k] = 0;
      // for ( k = 0; k < siz; k++ ) A[k] = k%5;
      // for ( k = 0; k < siz; k++ ) A[k] = siz-k;
      // TFill = clock() - TFill;
       gettimeofday(&tim, NULL);
       TFILL=tim.tv_sec+(tim.tv_usec/1000000.0) - TFILL;
 // now we know how much time it takes to fill the array
 // measure the time to fill & sort the array
 // T = clock();
       gettimeofday(&tim, NULL);
       T=tim.tv_sec+(tim.tv_usec/1000000.0);

  cnt = 0;

  for (i = siz; i < siz + reps; i++) { 
    fillarray(A, siz, i);
      // for ( k = 0; k < siz; k++ ) A[k] = 0;
      // for ( k = 0; k < siz; k++ ) A[k] = k%5;
      // for ( k = 0; k < siz; k++ ) A[k] = siz-k;
    if ( bool ) {
      (*alg1)(A, 0, siz-1, compar, numThreads);
    }
    else
      (*alg1)(A, siz, compar);
  }
       // ... and subtract the fill time to obtain the sort time
       // algTime = clock() - T - TFill;
       gettimeofday(&tim, NULL);
       algTime=tim.tv_sec+(tim.tv_usec/1000000.0) - T - TFILL;
  //printf("algTime: %f \n", algTime);
  clocktime += algTime/reps;
  cnt = cnt/reps;
  comparisons += cnt;

  // free array
  for (i = 0; i < siz; i++) {
    free(A[i]); 
  };
  free(A);

} // end countcomparisons

int siz = 1024*1024*16;
// int siz = 1024*1024 *4;
// int siz = 1024*1024;
// int siz = 1024*32;
// int siz = 1024*5;
// int siz = 1024;

void cc(char* label, void (*alg1)(), int (*compar )(), 
	int bool, int numThreads) {
  printf("cc %s ", label);
  int size = siz;
  // int repeat = 0;
  //  printf("size %i min # comparisons: %g\n", 
  //	 size, 1.44*size*log(size));
  int repeat = 4;
  int reps = 1;
  int i;
  while ( repeat < 5) {
    // double nln = size * log(size) / log(2.0);
    // printf("cc %s size %d min # comparisons: %12.0f\n", label, size, nln);
    //    printf("cc %s #threads %i\n", label, numThreads);
    comparisons = 0;
    clocktime = 0;
    for ( i = 0; i < reps; i++)
      countcomparisons(size, alg1, compar, i, bool, numThreads);
    comparisons = comparisons / reps; clocktime = clocktime / reps;

    // printf("size %i comparisons %g clocktime %i clock2 %8.2f\n",
    // printf("size %i comparisons %lld clocktime %i clock2 %8.2f\n",
    //         size, comparisons, clocktime, 1000000 * clocktime/ nln);
    printf("size %i comparisons %lld clocktime average %f\n",
	   size, comparisons, clocktime);
    size *= 2; repeat++;
  }
  
} // end cc


// ************************************************************************// ************************************************************************

// Infrastructure for testing non-uniform distributions

// fill with random numbers according to percentage, otherwise 0
void fillRandom(void **A, int lng, int startv, int percentage) {
  if ( percentage < 0 || 100 < percentage ) percentage = 50;
  int i, v, r, v2; 
  srand(startv);
  struct intval *pi;
  for ( i = 0; i < lng; i++) {
    pi = (struct intval *)A[i];
    if ( 0 == percentage ) pi->val = 0; else {
      v = rand();
      v2 = (v < 0) ? -v : v;
      r = v2%100;
      pi->val = (r <= percentage) ? v : 0; 
    }
  }
} // end fillRandom

// fill with random numbers according to percentage, otherwise with i
void fillRandom2(void **A, int lng, int startv, int percentage) {
  if ( percentage < 0 || 100 < percentage ) percentage = 50;
  int i, v, r, v2; 
  srand(startv);
  struct intval *pi;
  for ( i = 0; i < lng; i++) {
    pi = (struct intval *)A[i];
    // if ( 0 == percentage ) pi->val = i; else {
    if ( 0 == percentage ) pi->val = -i; else {
      v = rand();
      v2 = (v < 0) ? -v : v;
      r = v2%100;
      // pi->val = (r <= percentage) ? v : i; 
      pi->val = (r <= percentage) ? v : -i; 
    }
  }
} // end fillRandom2

void testNonRandom(int p, void **A, int siz, int seedLimit, 
		   void (*alg1)(), void (*alg2)(), 
		   int (*compare1)(), int (*compare2)()  ) {
  // A = A2;
  int z, alg1Time, alg2Time, T, seed, cnt1, cnt2;
  // for (z = 0; z < 3; z++) { // repeat to check stability
  for (z = 0; z < 1; z++) { // repeat to check stability
    alg1Time = 0; alg2Time = 0; cnt1 = 0; cnt2 = 0;
    int TFill = clock();
    for (seed = 0; seed < seedLimit; seed++) 
      fillarray(A, siz, seed);
    TFill = clock() - TFill;
    T = clock();
    for (seed = 0; seed < seedLimit; seed++) { 
      cnt = 0;
      // fillRandom(A, siz, seed, p); // zeros + random
      fillRandom2(A, siz, seed, p); // sorted + random
      // (*alg1)(A, siz, compare1); 
      (*alg1)(A, 0, siz-1, compare1); 
      cnt1 = cnt1 + cnt;
    }
    alg1Time = clock() - T - TFill;
    T = clock(); cnt = 0;
    for (seed = 0; seed < seedLimit; seed++) { 
      cnt = 0;
      // fillRandom(A, siz, seed, p); // zeros + random
      fillRandom2(A, siz, seed, p); // sorted + random
      // (*alg2)(A, siz, compare2);
      (*alg2)(A, 0, siz-1, compare2); 
      cnt2 = cnt2 + cnt;
    }
    alg2Time = clock() - T - TFill;
    printf("%s %d %s %i", "siz: ", siz, " p: ", p);
    printf(" %s %d %s", "alg1Time: ", alg1Time, " ");
    printf("%s %d %s", "alg2Time: ", alg2Time, " ");
    printf("cnt1: %i cnt2: %i ", (cnt1/seedLimit), (cnt2/seedLimit) );
    float frac = 0;
    if ( alg1Time != 0 ) frac = alg2Time / ( 1.0 * alg1Time );
    printf("%s %f %s", "frac: ", frac, "\n");
  }
} // end testNonRandom

void compareZeros00(char *label, int siz, int seedLimit, 
		   void (*alg1)(), void (*alg2)(),
		   int (*compare1)(), int (*compare2)() ) {
  printf("%s on size: %d seedLimit: %d\n", label, siz, seedLimit);
  int seed;
  // int limit = 1024 * 1024 * 16 + 1;
  int limit = 1024 * 1024 + 1;
  while (siz <= limit) {
    printf("%s %d %s %d %s", "siz: ", siz, " seedLimit: ", seedLimit, "\n");
    struct intval *pi;
    void **A = myMalloc("compareAlgorithms0 1", sizeof(pi) * siz);
    // construct array
    int i;
    for (i = 0; i < siz; i++) {
      pi = myMalloc("compareAlgorithms0 2", sizeof (struct intval));
      A[i] = pi;
    };  
    // warm up the process
    for (seed = 0; seed < seedLimit; seed++) 
      fillarray(A, siz, seed);
    // int p;
    // for (p = 0; p < 101; p = p + 20 )  // percentages of random elements
    //   testNonRandom(p, A, siz, seedLimit, alg1, alg2, compare1, compare2);
    testNonRandom(0, A, siz, seedLimit, alg1, alg2, compare1, compare2);
    testNonRandom(20, A, siz, seedLimit, alg1, alg2, compare1, compare2);
    testNonRandom(40, A, siz, seedLimit, alg1, alg2, compare1, compare2);
    // testNonRandom(100, A, siz, seedLimit, alg1, alg2, compare1, compare2);

    // free array
    for (i = 0; i < siz; i++) {
      free(A[i]);
    };
    free(A);
    siz = siz * 2;
    seedLimit = seedLimit / 2;
  }
} // end compareZeros00

// Check that adjacent objects in an array are ordered.
// If not, it reports an error 
void check(void **A, int N, int M) {
  int i; int cnt = 0;
  void *x, *y;
  struct intval *pi;
  for (i = N + 1; i <= M; i++) {
    x = A[i-1]; y = A[i];
    if ( compareXY(y, x) < 0 ) {
      pi = (struct intval *) y;
      printf("%s %d %s %d %s", "Error at: ", i, 
	     " A[i]: ", pi->val, "\n");
      cnt++;
    }
  }
  printf("check # errors: %d\n", cnt);
} // end check


void testAlg1(char* label, int siz, void (*alg1)(),
		     int (*compare1)()) {
  printf("%s on size: %d\n", label, siz);
  // create array
  struct intval *pi;
  void **A = myMalloc("testAlgorithm0 1", sizeof(pi) * siz);
  int i;
  for (i = 0; i < siz; i++) {
    pi = myMalloc("testAlgorithm0 2", sizeof (struct intval));
    A[i] = pi;
  };
  // fill its content
  fillarray(A, siz, 0);
  // sort it
  (*alg1)(A, 0, siz-1, compare1);
  // and check it
  check(A, 0, siz-1);
  // free array
    for (i = 0; i < siz; i++) {
      free(A[i]);
    };
    free(A);
} // end testAlg0

void testAlg2(char* label, int siz, void (*alg1)()) {
  testAlg1(label, siz, alg1, compareXY);
} // end testAlgorithm00

void testAlg3(char* label, void (*alg1)() ) {
  // testAlg2(label, 1024*1024*16, alg1);
  // testAlg2(label, 1024 * 1024 *2, alg1); 
  testAlg2(label, 16, alg1);
} // end testAlgorithm000

void testAlg() {
  // testAlg3("Test cut4 ...", cut4);
  // testAlg3("Test cut7 ...", cut7);
}

// validateAlgorithm0 is used to check algorithm alg2 against a
// trusted algorithm alg1.
// The check consists of making sure that starting from identical
// inputs they produce identical outputs
void validateAlgorithm0(char* label, int siz, void (*alg1)(), void (*alg2)() ) {
  printf("%s on size: %d\n", label, siz);
  // create the input for alg1 ...
  struct intval *pi;
  void **A = myMalloc("validateAlgorithm0 1", sizeof(pi) * siz);
  int i;
  for (i = 0; i < siz; i++) {
    pi = myMalloc("validateAlgorithm0 2", sizeof (struct intval));
    A[i] = pi;
  };
  fillarray(A, siz, 0);
  // ... sort it
  (*alg1)(A, 0, siz-1, compareXY);
  // (*alg1)(A, 0, siz, compareXY); // special for quickSortD

  // create the input for alg2 ...
  void **B = myMalloc("validateAlgorithm0 3", sizeof(pi) * siz);
  // struct intval *pi;
  for (i = 0; i < siz; i++) {
    pi =  myMalloc("validateAlgorithm0 4", sizeof (struct intval));
    B[i] = pi;
  };
  fillarray(B, siz, 0);
  // ... sort it
  (*alg2)(B, 0, siz-1, compareXY);
 
  // check that the two outputs are the same
  int foundError = 0;
  for (i = 0; i < siz; i++)
    // if ( A[i] != B[i] ) {
    if ( compareXY(A[i], B[i]) != 0 ) {
      printf("validate error i: %d\n", i);
      foundError = 1;
    }
  if ( !foundError ) 
    printf("NO error found ...\n");
  // free array
    for (i = 0; i < siz; i++) {
      free(A[i]); 
    };
    free(A);
    for (i = 0; i < siz; i++) {
      free(B[i]); 
    };
    free(B);
} // end validateAlgorithm0

// Like validateAlgorithm0 but with fixed array size
void validateAlgorithm(char* label, void (*alg1)(), void (*alg2)() ) {
  // validateAlgorithm0(label, 1024 * 1024 * 16, alg1, alg2);
  // validateAlgorithm0(label, 1024*1024*2, alg1, alg2);
  // validateAlgorithm0(label, 1025, alg1, alg2);
validateAlgorithm0(label, 30, alg1, alg2);
} // end validateAlgorithm

// Example:: replace XYZ by what you want to validate
void validateAlg() {
  void cut2(), c2p();
  validateAlgorithm("Running validate c2p ...", cut2, c2p);
  // validateAlgorithm("Running validate c2p ...", cut2, cut2);
  // validateAlgorithm("Running validate cut7 ...", quicksort0, cut7);
} // end validateAlg()

void validateAlgorithmP0(char* label, int siz, void (*alg1)(), void (*alg2)(),
			 int numTreads) {
  printf("%s on size: %d\n", label, siz);
  // create the input for alg1 ...
  struct intval *pi;
  void **A = myMalloc("validateAlgorithm0 1", sizeof(pi) * siz);
  int i;
  for (i = 0; i < siz; i++) {
    pi = myMalloc("validateAlgorithm0 2", sizeof (struct intval));
    A[i] = pi;
  };
  fillarray(A, siz, 0);
  // ... sort it
  (*alg1)(A, 0, siz-1, compareXY);
  // (*alg1)(A, 0, siz, compareXY); // special for quickSortD

  // create the input for alg2 ...
  void **B = myMalloc("validateAlgorithm0 3", sizeof(pi) * siz);
  // struct intval *pi;
  for (i = 0; i < siz; i++) {
    pi =  myMalloc("validateAlgorithm0 4", sizeof (struct intval));
    B[i] = pi;
  };
  fillarray(B, siz, 0);
  // ... sort it
  (*alg2)(B, 0, siz-1, compareXY, numTreads);
 
  // check that the two outputs are the same
  int foundError = 0;
  for (i = 0; i < siz; i++)
    // if ( A[i] != B[i] ) {
    if ( compareXY(A[i], B[i]) != 0 ) {
      printf("validate error i: %d\n", i);
      // exit(0);
      foundError = 1;
    }
  if ( !foundError ) 
    printf("NO error found ...\n");
  // free array
    for (i = 0; i < siz; i++) {
      free(A[i]); 
    };
    free(A);
    for (i = 0; i < siz; i++) {
      free(B[i]); 
    };
    free(B);
} // end validateAlgorithmP0


void validateAlgorithmP(char* label, void (*alg1)(), void (*alg2)(),
			int numThreads) {
  validateAlgorithmP0(label, 1024 * 1024 * 16, alg1, alg2, numThreads);
  // validateAlgorithmP0(label, 1024*1024*2, alg1, alg2, numThreads);
  // validateAlgorithmP0(label, 1024*1024, alg1, alg2, numThreads); // -
  // validateAlgorithmP0(label, 700*1024, alg1, alg2, numThreads); // -
  // validateAlgorithmP0(label, 650*1024, alg1, alg2, numThreads); // -
  // validateAlgorithmP0(label, 649*1024, alg1, alg2, numThreads); // +
  // validateAlgorithmP0(label, 10*1025, alg1, alg2, numThreads);
  // validateAlgorithmP0(label, 700, alg1, alg2, numThreads);
  // validateAlgorithmP0(label, 500, alg1, alg2, numThreads);
  // validateAlgorithmP0(label, 260, alg1, alg2, numThreads);
  // validateAlgorithmP0(label, 240, alg1, alg2, numThreads);
  // validateAlgorithmP0(label, 120, alg1, alg2, numThreads);

} // end validateAlgorithm

void validateAlgP2() {
  void cut2(), c2p();
  validateAlgorithmP("Running validate c2p ...", cut2, c2p, 3);
} // end validateAlgP2()

void validateAlgPD4() {
  void cut2(), cd4p();
  validateAlgorithmP("Running validate cd4p ...", cut2, cd4p, 2);
} // end validateAlgP2()

void validateAlgP4() {
  void cut2(), c4p();
  validateAlgorithmP("Running validate c4p ...", cut2, c4p, 3);
} // end validateAlgP2()

void validateAlgP7() {
  void cut2(), c7p();
  validateAlgorithmP("Running validate c7p ...", cut2, c7p, 3);
} // end validateAlgP2()
