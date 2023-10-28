// c:/bsd/rigel/sort/C2LR.c
// Date: Wed Jun 10 15:37:30 2020, Fri May 28 21:24:28 2021
// (C) OntoOO/ Dennis de Champeaux

// static const int cut2LRLimit =  250; 
#define cut2LRLimit 250
// static const int bufSize = 200;
#define bufSize 200
#define iswap(p, q, A) { void *t3t = A[p]; A[p] = A[q]; A[q] = t3t; }

static void cut2lrc(void **A, int lo, int hi, 
		    int depthLimit, int (*compareXY)());

void cut2lr(void **A, int lo, int hi, int (*compare)()) { 
  int L = hi - lo;
  int depthLimit = 2.9 * floor(log(L));
  if ( L < cut2LRLimit ) { 
    dflgm3(A, lo, hi, depthLimit, compare);
    // quicksortmc(A, lo, hi, depthLimit, compare);
    // cut2c(A, lo, hi, depthLimit, compare);
    return;
  }
  cut2lrc(A, lo, hi, depthLimit, compare);
} // end cut2lr


static void cut2lrc1(void **A, int lo, int hi, int bufl[], int bufr[],
	     int depthLimit, int (*compareXY)());
void cut2lrc(void **A, int lo, int hi, 
	       int depthLimit, int (*compareXY)()) { 
  int bufl[bufSize];
  int bufr[bufSize];
  cut2lrc1(A, lo, hi, bufl, bufr, depthLimit, compareXY);
}

void cut2lrc1(void **A, int lo, int hi, int bufl[], int bufr[],
	       int depthLimit, int (*compareXY)()) {  
 Start:;
  // printf("cut2lrc1 lo %d hi %d %d\n", lo, hi, hi-lo);

  int L = hi - lo;
  if ( L <= 0 ) return;

  if ( depthLimit <= 0 ) {
    heapc(A, lo, hi, compareXY);
    return;
  }
  depthLimit--;

  if ( L < cut2LRLimit ) { 
    dflgm3(A, lo, hi, depthLimit, compareXY);
    // quicksortmc(A, lo, hi, depthLimit, compareXY);
    // cut2c(A, lo, hi, depthLimit, compareXY);
    //cut2k2c(A, lo, hi, depthLimit, compareXY);
    return;
  }

  register void *T; // pivot
  register int I = lo, J = hi; // indices
  int middlex = lo + (L>>1); // lo + L/2
  void *middle;
  int k, lo1, hi1; // for sampling 
  // int probeLng = (int) sqrt(L/7.0);   // 820 9.75
  // int probeLng = (int) sqrt(L/6.0);   // 835 9.66
  int probeLng = (int) sqrt(L/6.0);   // 835 9.66
  if ( probeLng < 9 ) probeLng = 9;
    int halfSegmentLng = probeLng >> 1; // probeLng/2;
    lo1 = middlex - halfSegmentLng; //  lo + (L>>1) - halfSegmentLng;
    hi1 = lo1 + probeLng - 2;
    int offset = L/probeLng;  

    // assemble the mini array [lo1, hi1]
    // int xx = lo1;
    int yy = lo;
    for (k = lo1; k <= hi1; k++) 
      { iswap(k, yy, A); yy += offset; }

    // sort this mini array to obtain good pivots
    // quicksort0c(A, lo1, hi1, depthLimit, compareXY);
    // cut2c(A, lo1, hi1, depthLimit, compareXY);
    cut2lrc1(A, lo1, hi1, bufl, bufr, depthLimit, compareXY);

    T = middle = A[middlex]; // pivot
    if ( compareXY(A[hi1], middle) <= 0 ||
	 compareXY(A[lo1], middle) == 0 ) {
      // give up because cannot find a good pivot
      // dflgm is a dutch flag type of algorithm
      dflgm(A, lo, hi, middlex, cut2lrc, depthLimit, compareXY);
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
    // The left segment has elements <= T
    // The right segment has elements >= T 
    //    and at least one element > T
  int kl, kr, idxl, idxr; 
  int bufx = bufSize-1;

  /*
       |------]--------------------[------|
       lo     I                    J      hi
   */
Left:
  kl = kr = -1;
  while ( kl < bufx ) {
    while ( compareXY(A[++I], T) <= 0 );
    if ( J <= I ) { I = J-1; goto MopUpL; }
    bufl[++kl] = I;
  }
  /*
       |-------*--*---*-]----------[------|
       lo               I          J      hi
  */

  while ( kr < bufx ) {
    while ( compareXY(T, A[--J]) < 0 ); 
    if ( J <= I ) { J = I+1; goto MopUpR; }
    bufr[++kr] = J;
  }

  /*
    |-------*--*---*-]----[--*--*-*-------|
    lo               I    J               hi
  */
  // swap them
  while ( 0 <= kl ) {
    idxl = bufl[kl--];
    idxr = bufr[kr--];
    iswap(idxl, idxr, A); 
  }
  goto Left;

 MopUpR:
  // swap them
  while ( 0 <= kr ) {
    idxl = bufl[kl--];
    idxr = bufr[kr--];
    iswap(idxl, idxr, A); 
  }

MopUpL:
  /*
       |--------------*---*----*--[------|
       lo                         J      hi

   */
  // swap them
  while ( 0 <= kl ) {
    idxl = bufl[kl--];
    if ( J <= idxl ) continue;
    if ( idxl + 1 == J ) { J--; continue; }
    { int z = J-1; iswap(z, idxl, A); J = z; }
  }

    if ( (I - lo) < (hi - J) ) { // smallest one first
      cut2lrc1(A, lo, J-1, bufl, bufr, depthLimit, compareXY);
      lo = J; 
      goto Start;
    }
    cut2lrc1(A, J, hi, bufl, bufr, depthLimit, compareXY);
    hi = J-1;
    goto Start;
} // (*  OF cut2lrc1; *) the brackets remind that this was once, 1985, Pascal code

#undef cut2LRLimit
#undef iswap
#undef bufsize
