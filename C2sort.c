// c:/bsd/rigel/sort/C2sort.c
// Date: Fri Jan 31 13:32:12 2014, 2017 Sun Mar 03 16:14:28 2019, Sat Jul 23 13:05:48 2022
// Mon Jan 04 10:43:49 2021, Sun May 30 14:48:25 2021 Thu Jul 14 16:05:59 2022
// (C) OntoOO/ Dennis de Champeaux

#define iswap(p, q, A) { void *t3t = A[p]; A[p] = A[q]; A[q] = t3t; }

// This version combines isort + dflgm + ( pivot sample + (fast loops | dflgm ) )

static const int dflgmLimit2 = 250;
// static const int iLimit2 = 9;

void cut2c(); // is called also
// cut2 is used as a best in class quicksort implementation 
// with a defense against quadratic behavior due to duplicates
// cut2 is a support function to call up the workhorse cut2c
void cut2(void **A, int lo, int hi, int (*compare)()) { 
  // printf("cut2 %d %d %d\n", lo, hi, hi-lo);
  int L = hi - lo;
  if ( L <= 0 ) return;
  int depthLimit = 2.9 * floor(log(L));
  cut2c(A, lo, hi, depthLimit, compare);
} // end cut2
/*
// calculate the median of 3
static int medq2(void **A, int a, int b, int c,
	int (*compareXY ) (const void *, const void * ) ) {
  return
    compareXY( A[a], A[b] ) < 0 ?
    ( compareXY( A[b], A[c] ) < 0 ? b : compareXY( A[a], A[c] ) < 0 ? c : a)
    : compareXY( A[b], A[c] ) > 0 ? b : compareXY( A[a], A[c] ) > 0 ? c : a;
} // end medq2
*/
void cut2c(void **A, int lo, int hi, int depthLimit, 
		 int (*compareXY)(const void*, const void*)) {
  // printf("Enter cut2c lo: %d hi: %d %d\n", lo, hi, depthLimit);
  // printf(" gap %d \n", hi-lo);

  while ( lo < hi ) {
    // printf("cut2c lo: %d hi %d  L %i\n", lo, hi, hi-lo);
    int L = hi - lo;
    if ( depthLimit <= 0 ) {
      heapc(A, lo, hi, compareXY);
      return;
    }
    depthLimit--;

    if ( L <= dflgmLimit2 ) {
      dflgm3(A, lo, hi, depthLimit, compareXY);
      return;
    }

    register void *T; // pivot
    register int I = lo, J = hi; // indices
    int middlex = lo + (L>>1); // lo + L/2
    void *middle;
    int k, lo1, hi1; // for sampling
    int probeLng = sqrt(L/7.0); if ( probeLng < 9 ) probeLng = 9;
    // int probeLng = 9;
    int halfSegmentLng = probeLng >> 1; // probeLng/2;
    lo1 = middlex - halfSegmentLng; //  lo + (L>>1) - halfSegmentLng;
    hi1 = lo1 + probeLng - 1;
    int offset = L/probeLng;  

    // assemble the mini array [lo1, hi1]
    for (k = 0; k < probeLng; k++) // iswap(lo1 + k, lo + k * offset, A);
      { int xx = lo1 + k, yy = lo + k * offset; iswap(xx, yy, A); }
    // sort this mini array to obtain good pivots
    // quicksort0c(A, lo1, hi1, depthLimit, compareXY);
    cut2c(A, lo1, hi1, depthLimit, compareXY);
    T = middle = A[middlex];
    if ( compareXY(A[hi1], middle) <= 0 ) {
      // give up because cannot find a good pivot
      // dflgm is a dutch flag type of algorithm
      dflgm(A, lo, hi, middlex, cut2c, depthLimit, compareXY);
      return;
    }
    for ( k = lo1; k <= middlex; k++ ) {
      iswap(k, I, A); I++;
    }
    I--;
    for ( k = hi1; middlex < k; k--) {
      iswap(k, J, A); J--;
    }
    J++;
 
    // get the ball rolling::
    register void *AI, *AJ; // array values
    // The left segment has elements <= T
    // The right segment has elements >= T
  Left:
    while ( compareXY(A[++I], T) <= 0 ); 
    AI = A[I];
    while ( compareXY(T, A[--J]) < 0 ); 
    AJ = A[J];
    if ( I < J ) { // swap
      A[I] = AJ; A[J] = AI;
      goto Left;
    }
    // Tail iteration
    if ( (I - lo) < (hi - J) ) { // smallest one first
      cut2c(A, lo, J, depthLimit, compareXY);
      lo = I; 
    } else {
      cut2c(A, I, hi, depthLimit, compareXY);
      hi = J;
    }
  } // end while
} // (*  OF cut2; *) the brackets remind that this was once, 1985, Pascal code

#undef iswap
