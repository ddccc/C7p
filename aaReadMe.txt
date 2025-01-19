File: c:/bsd/rigel/sort/C7p/aaReadMe.txt
Date: Sat Dec 14 07:03:07 2019

There is one driver;

-- ComparisonCountingp.c
This one deals with the parallel versions.
It reports real time timings

Instructions for how to run the drivers are in the top of this file.

To make a driver for sorting referenced items one needs:
- a comparison function
- for cut2lr2: the files Isort, Hsort, D3sort, Dsort, C2LR2
- the 3-pivot versions:
  for cut4: the cut2lr2 files & C4
  for cut4d: the cut2lr2 files & CD4
  for cut4n: the cut2lr2 files & CN4
  for cut7: the cut2lr2 files & C7
The file ComparisonCountingp.c does all this for all of them.

The .h files loads the corresponding file with:
   #include "XXX.c"
for convenience because recompilation is fast enough here.

