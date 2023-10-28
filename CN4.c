// File: c:/bsd/rigel/sort/CN4.c
// Date: Wed Jul 14 13:43:11 2022   Wed Jul 27 21:23:27 2022
// (C) Dennis de Champeaux/ OntoOO

// Author: Nigel Horspool implementing ideas of Dennis de Champeaux
// Date: 2021-03-29

#include <math.h>
#include "C2LR2.h"

// swap two elements of an array given pointers to the two elements, p and q
#define PSWAP(p, q) { void *t2 = *(void**)p; *(void**)p = *(void**)q; *(void**)q = t2; }

#define CUT4NMIN 1500    // minimum size array for 3 pivots to be used

void cut4np(void **A, void **hip, int depthLimit, int (*compareXY)());


void cut4n(void **A, int lo, int hi, int (*compareXY)()) {
    int size = hi-lo+1;
    int depthLimit = 1 + 2.5 * floor(log(size));
    cut4np(A+lo, A+hi, depthLimit, compareXY);
}


// cut4nc is used only if dflgm is invoked
void cut4nc(void **A, int N, int M, int depthLimit, int (*compareXY)()) {
    cut4np(A+N, A+M, depthLimit, compareXY);
}


// A[0] is the first element in a subarray to be sorted
// hip references the last element in subarray to be sorted
// void cut4np(void **A, void **hip, int depthLimit, COMPAREFN compareXY) {
void cut4np(void **A, void **hip, int depthLimit, int (*compareXY)()) {
  int size;

  while (1) {
        // while( (size = hip - A + 1) >= CUT4NMIN ) {
        if (depthLimit-- <= 0) {
            heapc(A, 0, size-1, compareXY);
            return;
        }
	size = hip - A + 1;
	if ( size < CUT4NMIN ) {
	  // cut2(A, 0, hip-A, compareXY);
	  // cut2lr(A, 0, hip-A, compareXY);
	  // cut2lrc(A, 0, hip-A, depthLimit, compareXY);
	  // cut2k2c(A, 0, hip-A, depthLimit, compareXY);
	  // cut2Nk1nc(A, 0, hip-A, depthLimit, compareXY);
	  cut2lr2c(A, 0, hip-A, depthLimit, compareXY);
	  return;
	}
        void **zp = A + (size>>1); // zp points to array midpoint

        // int numberSamples = (int)sqrt(size/5.8);
	int numberSamples = (int)sqrt(size/3.0);
        int offset = size / numberSamples;
        void **SampleStart = zp - numberSamples/2;
        void **SampleEnd = SampleStart + numberSamples;

        // assemble the mini array [SampleStart, SampleEnd)
        void **yyp = A;
        void **xxp;
        for( xxp = SampleStart; xxp < SampleEnd; xxp++) {
            PSWAP(xxp, yyp);
            yyp += offset;
        }
        // sort this mini array to obtain good pivots
        cut4np(SampleStart, SampleEnd-1, depthLimit, compareXY);

        // the three pivots and their positions
        void **pivot1xp = zp - numberSamples/4;
        void **pivot2xp = zp;
        void **pivot3xp = zp + numberSamples/4;
        void *pivot1 = *pivot1xp;
        void *pivot2 = *pivot2xp;
        void *pivot3 = *pivot3xp;

        // check that all four segments will be non-empty
        if (compareXY(pivot2, pivot1)  == 0 ||
            compareXY(pivot2, pivot3) == 0 ||
            compareXY(*SampleStart, pivot1) == 0 ||
            compareXY(*(SampleEnd-1), pivot3) == 0 ) {
            // an initial partition will be empty due to duplicate values,
            // so use Dutch Flag which will handle the duplicates well
            dflgm(A, 0, hip-A, zp-A, cut4nc, depthLimit, compareXY);
            return;
        }

        void **ip = A-1;        // points to last element in LEFT partition
        void **jp = hip+1;      // points to first element in RIGHT partition
        void **qp = pivot1xp;   // points to last element in left unexplored region
        void **rp = pivot3xp;   // points to first element in right unexplored region

        // Initialize the LEFT partition
        void **kkp;
        for ( kkp = SampleStart; kkp <= pivot1xp; kkp++ ) {
            ip++;
            PSWAP(kkp, ip);
        }

        // Initialize the RIGHT partition
        for ( kkp = SampleEnd-1; kkp >= pivot3xp; kkp--) {
            jp--;
            PSWAP(kkp, jp);
        }

        void *x, *y; // element holders

        /* The last element in x must be inserted somewhere. The hole
           location is used for this task */
        void **holep = A;
        x = *++ip; // x is the first element to be inserted somewhere
        *ip = *holep;

          /* We employ again whack-a-mole. We discover in which partition element x
             should be.  Find a close, undecided position where x should go.
             Exchange contents of that position with x.
           */


    //  Partition scheme while there are two unexplored regions
    //
    //       LEFT               MID-LEFT        MID-RIGHT              RIGHT
    //    ____________________________________________________________________
    //   | |        |       |               |                |       |        |
    //   |*| <= p1  |  ???  | >= p1 & <= p2 |  >= p2 & <= p3 |  ???  |  >= p3 |
    //   |_|________|_______|_______________|________________|_______|________|
    //   A        ip      qp              zp                  rp      jp    hip
    //
    //   * indicates the "hole" (one element) which will be filled at the end
    //   A    references the first element in the subarray being sorted
    //   ip   references the last element in LEFT partition
    //   qp   references the last element in the left unexplored region
    //   zp   references the last element in the MID-LEFT partition
    //   rp   references the first element in the right unexplored region
    //   jp   references the first element in RIGHT partition
    //   hip  references the last element in the subarray
    // where p1, p2, p3 are the three pivots.

        for( ; ; ) {
            if (compareXY(x, pivot2) <= 0) {
                if (ip == qp) break;
                if (compareXY(x, pivot1) <= 0) { // Add x to LEFT
                    y = *++ip;
                    *ip = x;
                } else { // Add x to MID-LEFT
                    y = *qp;
                    *qp-- = x;
                }
            } else {
                if (rp == jp) break;
                if (compareXY(x, pivot3) < 0) {  // Add x to MID-RIGHT
                	y = *rp;
                	*rp++ = x;
                } else {  // Add x to RIGHT
                    y = *--jp;
                    *jp = x;
                }
            }
            x = y;
        }


        //  Partition scheme when the left gap has been closed (qp is now unused)
        //
        //       LEFT       MID-LEFT        MID-RIGHT              RIGHT
        //    ____________________________________________________________
        //   | |        |               |                |       |        |
        //   |*| <= p1  | >= p1 & <= p2 |  >= p2 & <= p3 |  ???  |  >= p3 |
        //   |_|________|_______________|________________|_______|________|
        //   from     ip              zp                  rp      jp       to

        while(rp < jp) {
            if (compareXY(x, pivot2) <= 0) {
                if (compareXY(x, pivot1) <= 0) { // Add x to LEFT
                    y = *++ip;
                    *ip = x;
                    x = *++zp;
                    *zp = y;
                    y = *rp;
                    *rp++ = x;
                    x = y;
                } else {    // Add x to MID-LEFT
                    y = *++zp;
                    *zp = x;
                    x = *rp;
                    *rp++ = y;
                }
            } else {
                if (compareXY(x, pivot3) < 0) {  // Add x to MID-RIGHT
                    y = *rp;
                    *rp++ = x;
                } else {    // Add x to RIGHT
                    y = *--jp;
                    *jp = x;
                }
                x = y;
            }
        }

        //  Partition scheme when the right gap has been closed (rp is now unused)
        //
        //       LEFT               MID-LEFT        MID-RIGHT       RIGHT
        //    ____________________________________________________________
        //   | |        |       |               |                |        |
        //   |*| <= p1  |  ???  | >= p1 & <= p2 |  >= p2 & <= p3 |  >= p3 |
        //   |_|________|_______|_______________|________________|________|
        //   A        ip      qp              zp                  jp    hip

        while(ip < qp) {
            if (compareXY(x, pivot2) <= 0) {
                if (compareXY(x, pivot1) <= 0) { // Add x to LEFT
                    y = *++ip;
                    *ip = x;
                } else {    // Add x to MID-LEFT
                    y = *qp;
                    *qp-- = x;
                }
                x = y;
            } else {
                if (compareXY(x, pivot3) < 0) {  // Add x to MID-RIGHT
                    y = *zp;
                    *zp-- = x;
                    x = *qp;
                    *qp-- = y;
                } else {    // Add x to RIGHT
                    y = *--jp;
                    *jp = x;
                    x = *zp;
                    *zp-- = y;
                    y = *qp;
                    *qp-- = x;
                    x = y;
                }
            }
        }

    	// +++++++++++++++both gaps closed+++++++++++++++++++

        //  Partition scheme when the both gaps have been closed
        //
        //       LEFT       MID-LEFT        MID-RIGHT       RIGHT
        //    ____________________________________________________
        //   | |        |               |                |        |
        //   |*| <= p1  | >= p1 & <= p2 |  >= p2 & <= p3 |  >= p3 |
        //   |_|________|_______________|________________|________|
        //   A        ip              zp                  jp    hip

        // place the last element, which is in x
        // we may have to bump elements down until we get one
        // which belongs in the LEFT partition

        if (compareXY(x, pivot2) <= 0) {
            if (compareXY(x, pivot1) <= 0) {   // x belongs in LEFT
                *holep = x;
            } else { // x belongs in MID-LEFT
                y = *ip;
                *ip-- = x;
                *holep = y;
            }
        } else {
            if (compareXY(x, pivot3) < 0) {
                // x belongs in MID-RIGHT
                y = *zp;
                *zp-- = x;
                x = *ip;
                *ip-- = y;
                *holep = x;
            } else {
                // x belongs in RIGHT
                y = *--jp;
                *jp = x;
                x = *zp;
                *zp-- = y;
                y = *ip;
                *ip-- = x;
                *holep = y;

            }
        }


    //  Final picture
    //
    //     LEFT       MID-LEFT        MID-RIGHT       RIGHT
    //    __________________________________________________
    //   |        |               |                |        |
    //   | <= p1  | >= p1 & <= p2 |  >= p2 & <= p3 |  >= p3 |
    //   |________|_______________|________________|________|
    //   A      ip              zp                  jp    hip


        // More or less sort from smallest partition to largest
    	if ( zp-A < hip-zp ) {
            cut4np(A, ip, depthLimit, compareXY);
            cut4np(ip+1, zp, depthLimit, compareXY);
            if ( jp-zp < hip-jp ) {
                cut4np(zp+1, jp-1, depthLimit, compareXY);
                A = jp;
            } else {
                cut4np(jp, hip, depthLimit, compareXY);
                A = zp+1; hip = jp-1;
            }
    	} else {
        	cut4np(zp+1, jp-1, depthLimit, compareXY);
        	cut4np(jp, hip, depthLimit, compareXY);
        	if ( ip-A < zp-ip ) {
                cut4np(A, ip, depthLimit, compareXY);
                A = ip+1; hip = zp;
        	} else {
            	cut4np(ip+1, zp, depthLimit, compareXY);
            	hip = ip;
            }
        }
  }
  /*
    // Come here when fewer than CUT4NMIN elements in the subarray
    // cut2lr(A, 0, hip-A, compareXY);
    cut2lrc(A, 0, hip-A, depthLimit, compareXY);
    // cut2(A, 0, hip-A, compareXY);
    */
} // end cut4np

#undef PSWAP


