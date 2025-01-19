// File: c:/bsd/rigel/sort/CD4.c
// Date: Wed May 12 14:03:17 2021 Sat Jul 23 09:51:37 2022
// (C) OntoOO/ Dennis de Champeaux


#define iswap(p, q, A) { void *t3t = A[p]; A[p] = A[q]; A[q] = t3t; }

// static const int cut4dLimit = 1600; 

void cut4dc();
void cut4dpc();
// cut4d is doing 4-partitioning using 3 pivots
void cut4dp(void **A, int N, int M, int (*compareXY)()) {
  // printf("cut4d %d %d \n", N, M);
  int L = M - N; 
  if ( L <= 0 ) return;
  int depthLimit = 1 + 2.9 * floor(log(L));
  cut4dpc(A, N, M, depthLimit, compareXY);
} // end cut4dp

void cut4dpc(void **A, int N, int M, int depthLimit, int (*compareXY)()) {
  int L;
 Start:
  // printf("cut4dpc %d %d \n", N, M);
  L = M - N +1; 

  if ( depthLimit <= 0 ) {
    heapc(A, N, M, compareXY);
    return;
  }
  depthLimit--;

  if ( L < cut4dLimit ) { 
    // cut2lrc(A, N, M, depthLimit, compareXY);
    // cut2k2c(A, N, M, depthLimit, compareXY);
    // cut2Nk1nc(A, N, M, depthLimit, compareXY);
    // cut2lrc(A, N, M, depthLimit, compareXY);
    cut4dc(A, N, M, depthLimit, compareXY);
    return;
  }

  int k, N1, M1; // for sampling
  int maxlx, middlex, minrx;  
  void *maxl, *middle, *minr;   // pivots for left/ middle / right regions
  int i, j, lw, up, z; // indices
  i = N; j = M;
  z = middlex = N + (L>>1); // N + L/2/

  // int probeLng = sqrt(L/5.8); 
    int probeLng = sqrt(L/3.0); 
    int halfSegmentLng = probeLng >> 1; // probeLng/2;
    int quartSegmentLng = probeLng >> 2; // probeLng/4;
    N1 = middlex - halfSegmentLng; //  N + (L>>1) - halfSegmentLng;
    M1 = N1 + probeLng - 1;
    maxlx = N1 + quartSegmentLng;
    minrx = M1 - quartSegmentLng;
    int offset = L/probeLng;  

    // assemble the mini array [N1, M1]
    for (k = 0; k < probeLng; k++) // iswap(N1 + k, N + k * offset, A);
    { int xx = N1 + k, yy = N + k * offset; iswap(xx, yy, A); }
    // sort this mini array to obtain good pivots
    // cut2lrc(A, N1, M1, depthLimit, compareXY);
    cut4dc(A, N1, M1, depthLimit, compareXY);
    // dflgm3(A, N1, M1, depthLimit, compareXY);
    lw = maxlx; up = minrx;

  // pivots
  maxl = A[maxlx]; middle = A[z]; minr = A[minrx];

  // check that segments can be properly initialized
  if ( compareXY(maxl, middle) == 0 || 
       compareXY(middle, minr) == 0 || 
       compareXY(minr, A[M1]) == 0 ||
       compareXY(maxl, A[N1]) == 0 ) {
    // no good pivots available, thus escape
    dflgm(A, N, M, middlex, cut4dpc, depthLimit, compareXY);
    return;
  }

    // Swap these two segments to the corners
    for ( k = N1; k <= maxlx; k++ ) {
      iswap(k, i, A); i++;
    }
    i--;
    for ( k = M1; minrx <= k; k--) {
      iswap(k, j, A); j--;
    }
    j++;

  // get moving

  void *x, *y; // values  
  /* The last element in x must be insert somewhere. The hole
     location is used for this task */
  // int hole = N; 
  x = A[++i]; // x is the first element to be inserted somewhere
  A[i] = A[N]; 

  // Here the general layout
   /*   L             ML         MR             R
    |-----]------+[---------]---------]+-----[-----|
    N     i      lw         z         up     j     M
   */

    /* ***********
       It is actually possible that ML contains an element equal to maxl 
       and similarly an element in MR equal to minr.
       ***********
    */ 
   /*
       There are invariants to be maintained (which are >essential< 
     for machine assisted correctness proofs):
     maxl < middle < minr
     If there are two gaps:
       N < x <= i --> A[x] <= maxl
       lw < x <= z --> maxl < A[x] <= middle
       z < x < up --> middle < A[x] < minr
       j <= x <= M --> minr <= A[x]
     If the left gap has closed:
       N < x < i --> A[x] <= maxl
       i <= x <= z --> maxl < A[x] <= middle
       z <  x < up --> middle < A[x] < minr
       j <= x <= M --> minr <= A[x]
     If the right gap has closed:
       N < x <= i --> A[x] <= maxl
       lw < x <= z --> maxl < A[x] <= middle
       z < x <= j --> middle < A[x] < minr
       j < x <= M --> minr <= A[x]
  */


  /* We employ again whack-a-mole. We know in which partition element x 
     should be.  Find a close, undecided position where x should go.  
     Store its content in y.  Drop x (simplified).  Set x to y and repeat. 
   */
  // Ready to roll ... 

  while (1) {
   /*   L             ML         MR             R
    o-----]------+[---------]---------]+-----[-----|
    N     i      lw         z         up     j     M
   */

    if (compareXY(x, middle) <= 0) { // x -> left
      if ( i == lw ) goto leftClosed;
      if (compareXY(x, maxl) <= 0) { // x -> L
	y = A[++i]; A[i] = x;
      } else { // x -> ML
	y = A[lw]; A[lw--] = x;
      }
    } else { // x -> right
      if ( up == j ) goto rightClosed;
      if (compareXY(x, minr) < 0) { // x -> MR
	y = A[up]; A[up++] = x;
      } else {  // x -> R
	y = A[--j]; A[j] = x;
      }
    }
    x = y;
  }

 leftClosed:
   /*   L             ML         MR             R
    o-----------][---------]---------]+-----[-----|
    N           i          z         up     j     M
   */
  while ( up < j ) {
    if ( compareXY(x, middle) <= 0 ) { // x -> left
      y = A[up];
      if ( compareXY(x, maxl) <= 0 ) { // x -> L
	A[up] = A[++z]; up++;
	A[z] = A[++i]; A[i] = x;
      } else { // x -> ML
	A[up] = A[++z]; up++; A[z] = x;
      }
    } else { // x -> right
      if ( compareXY(minr, x) <= 0 ) { // x -> R
	y = A[--j]; A[j] = x;
      } else { // x -> MR
	y = A[up]; A[up++] = x;
      }
    }
    x = y;
  }
  goto finalize;

 rightClosed:
   /*   L             ML         MR             R
    o-----]------+[---------]---------][-------------|
    N     i      lw         z          j             M
   */
  while ( i < lw ) {
    if ( compareXY(x, middle) <= 0 ) { // x -> left
      if ( compareXY(x, maxl) <= 0 ) { // x -> L
	y = A[++i]; A[i] = x;
      } else { // x -> ML
	y = A[lw]; A[lw--] = x;
      }
    } else { // x -> right
      y = A[lw]; A[lw--] = A[z];
      if ( compareXY(x, minr) < 0 ) { // x -> MR
	A[z--] = x;
      } else { // x -> R
	A[z] = A[--j]; z--; A[j] = x;
      }
    }
    x = y;
  }
  // fall through

 finalize: // insert last x

   /*   L             ML         MR             R
    o-----------][---------]---------][-------------|
    N           i          z          j             M
   */
   if ( compareXY(x, middle) <= 0 ) { // x -> left
      if ( compareXY(x, maxl) <= 0 ) { // x -> L
	A[N] = x;
      } else { // x -> ML
	A[N] = A[i]; A[i--] = x;
      }
   } else { // x -> right
     A[N] = A[i]; A[i--] = A[z];
     if ( compareXY(x, minr) < 0 ) { // x -> MR
       A[z--] = x;
     } else { // x -> R
       A[z] = A[--j]; z--; A[j] = x;
     } 
   }

   /*   L             ML         MR             R
    |-----------][---------]---------][-------------|
    N           i          z          j             M
   */

   /* // for checking:
	for (k = N; k <= i; k++)
	  if ( compareXY(maxl, A[k]) < 0 ) {
	    printf("Error L k %i N %i i %i z %i j %i M %i\n", k,N,i,z,j,M);
	    exit(0);
	  }
	for (k = i+1; k <= z; k++) 
	  if ( compareXY(A[k], maxl) < 0 || compareXY(middle, A[k]) < 0 ) {
	    printf("Error ML k %i N %i i %i z %i j %i M %i\n", k,N,i,z,j,M);
	    exit(0);
	  }
	for (k = z+1; k < j; k++) 
	  if ( compareXY(A[k], middle) < 0 || compareXY(minr, A[k]) <= 0 ) {
	    printf("Error MR k %i N %i i %i z %i j %i M %i\n", k,N,i,z,j,M);
	    exit(0);
	  }
	for (k = j; k <= M; k++)
	  if ( compareXY(A[k], minr) < 0 ) {
	    printf("Error R k %i N %i i %i z %i j %i M %i\n", k,N,i,z,j,M);
	    exit(0);
	  }
	// */	

   // Tail iteration::: sequential:
   /*
	if ( z-N < M-z ) {
	  cut4dc(A, N, i, depthLimit, compareXY);
	  cut4dc(A, i+1, z, depthLimit, compareXY);
	  if ( j-z < M-j ) {
	    cut4dc(A, z+1, j-1, depthLimit, compareXY);
	    // cut4dc(A, j, M, depthLimit, compareXY);
	    // return;
	    N = j;
	    goto Start;
	  }
	  cut4dc(A, j, M, depthLimit, compareXY);
	  // cut4dc(A, z+1, j-1, depthLimit, compareXY);
	  // return;
	  N = z+1; M = j-1;
	  goto Start;
	}
	// M-z <= z-N
	cut4dc(A, z+1, j-1, depthLimit, compareXY);
	cut4dc(A, j, M, depthLimit, compareXY);
	if ( i-N < z-i ) {
	  cut4dc(A, N, i, depthLimit, compareXY);
	  // cut4dc(A, i+1, z, depthLimit, compareXY);
	  // return;
	  N = i+1; M = z;
	  goto Start;
	}
	cut4dc(A, i+1, z, depthLimit, compareXY);
	// cut4dc(A, N, i, depthLimit, compareXY);
	M = i;
	goto Start;\
   */
 // Tail iteration::: parallel
   /*   L             ML         MR             R
    |-----------][---------]---------][-------------|
    N           i          z          j             M
   */

   if ( z-N < M-z ) {
     if ( j-z < M-j ) {
       // push cut4dc(A, J, M, depthLimit, compareXY);
       addTaskSynchronized(ll, newTask(A, j, M, depthLimit, compareXY));
       // push cut4dc(A, z+1, j-1, depthLimit, compareXY);
       addTaskSynchronized(ll, newTask(A, z+1, j-1, depthLimit, compareXY));
     } else {
       // push cut4dc(A, z+1, j-1, depthLimit, compareXY);
       addTaskSynchronized(ll, newTask(A, z+1, j-1, depthLimit, compareXY));
      // push cut4dc(A, J, M, depthLimit, compareXY);
       addTaskSynchronized(ll, newTask(A, j, M, depthLimit, compareXY));
     }
     // push  cut4dc(A, i+1, z, depthLimit, compareXY);
     addTaskSynchronized(ll, newTask(A, i+1, z, depthLimit, compareXY));
     // do  cut4dc(A, N, i, depthLimit, compareXY);
     M = i; goto Start;
   } else {
     if  ( i-N < z-i ) {
       // push cut4dc(A, i+1, z, depthLimit, compareXY);
       addTaskSynchronized(ll, newTask(A, i+1, z, depthLimit, compareXY));
       //  push cut4dc(A, N, i, depthLimit, compareXY);
       addTaskSynchronized(ll, newTask(A, N, i, depthLimit, compareXY));
     } else {
       // push cut4dc(A, N, i, depthLimit, compareXY);
       addTaskSynchronized(ll, newTask(A, N, i, depthLimit, compareXY));
      // push cut4dc(A, i+1, z, depthLimit, compareXY);
       addTaskSynchronized(ll, newTask(A, i+1, z, depthLimit, compareXY));
     }
     // push cut4dc(A, j, M, depthLimit, compareXY);
     addTaskSynchronized(ll, newTask(A, j, M, depthLimit, compareXY));
     N = z+1; M = j-1; goto Start;
   }

} // end cut4dpc

#undef iswap


