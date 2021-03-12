# Searching for Designs, Generalized Coverings and Packings with Problem Dependent Optimization (PDO)
Iliya Bluskov and Aaron Germuth

December 20th, 2016

This program searches for generalized coverings and packings with a fixed block size. This application is a fork of Nurmela and Ostergard's COVER. It uses Problem Dependent Optimization (PDO) as a search strategy as opposed to simulated annealing (as in the original COVER). Below you can find some notes on executing the program (cover.exe). 

The current version of the program works best on Windows. In order to use it on UNIX environments, a couple of headers need to be changed in cover.h. The old UNIX headers for random number generation are still there, but commented out, and labelled as such. This program is executed by opening a command prompt in the executable’s directory, and running it (by typing ./cover). This will begin the search. It can also be ran by clicking on any .bat file in the executable’s directory containing a line with ./cover and parameters as described below.

## New World Records

This was used to break some world records at the time, see https://www.researchgate.net/profile/Iliya-Bluskov/publication/323854750_New_Constant_Weight_Codes_and_Packing_Numbers/links/5bea1194299bf1124fce1e81/New-Constant-Weight-Codes-and-Packing-Numbers.pdf

TODO - add other world records

## Parameters:

The application can take parameters at run time in the same way as the original COVER. Parameters are specified after the name like:

	>> ./cover v=17 k=5 t=3 b=68
	
You can specify as many parameters as you like. Each specification begins with the name of the parameter (for example, v or k above), then an equals sign, and then its value. Many of the parameters are Boolean in nature and therefore can only be set to 0 or 1. Most of the other options are integer valued, with some floating point parameters. The large majority of parameters have exact backwards compatibility with Nurmela and Ostergard's COVER, but now work with PDO instead. There are also some added parameters. By default, on finding a solution, two output files are produced (.res and .log). The .res file contains all the blocks of the found solution (conveniently, in the same format which the program may take as input with startFromFile=1, see below). The .log file contains information on the values of all parameters used. The files are named in the following format “(v,k,m,t,lambda) - b.xxx”. Make sure you type the parameters correctly; otherwise the program may not start, or it may start but produce a non-desired result. Below is a dictionary of most of the currently accepted parameters:

### v,k,t,m,l

These variables are the parameters of the design. The default value for l is 1, and if only one of the values m and t are given it is assumed that m=t. The design is on v points, has blocks of size k, and every set of size m has intersection t with at least l blocks. (In lottery terms, the guarantee is t if m, and l t-wins are guaranteed if m of the drawn numbers are within the set of the v numbers of the system)


### b

 This variable is the number of k-sets (blocks) in the design.

### EndLimit

Sometimes you may want to stop the heuristic before cost zero is reached. The value of EndLimit is the cost of a desired solution. Abbreviation EL. For example, if you are looking for an (11,6,5) covering in 99 blocks, and the heuristics does not find one, then you might restart it and specify EL=2. The heuristic will stop at a solution with 99 blocks, but two 5-sets will not be covered by these blocks. 

### OntheFly

Set this variable to 1, if you want the program to not allocate all the tables and to compute some of the needed data on-the-fly. Abbreviation OF.

### SolX

 Set this variable to 1, if you want the solutions to be printed as
 `--X-XX' instead of `2 4 5'. Abbreviation SX.


### Pack

 You can search for packing designs by setting this variable to 1. Abbreviation P.

### pdoK

A floating point parameter of the PDO search algorithm. Each value of the jump down function is normally set to pdoK * neighbourhoodLength.   pdoK has a default value of 10. This parameter is not of much significance; it only affects how fast the heuristic runs at the beginning. Generally, we do not have much of an idea what the true values of the jump down function are, so we set them to some initial value, same for all levels. Later the heuristic will be adjusting the values of the jump down function to ones closer to the true values. Recommended (universal) value is 5.

### pdoJ

A floating point parameter of the PDO search algorithm. Once at a solution of cost c, we use the jump down function to assess how many times we should try to find a cost decreasing solution (accepting cost preserving solutions in the process) before accepting a cost increasing one. The number of times we try is pdoJ * jumpDownFunction(c). The default value is 3. This parameter is important; low values of pdoJ can send the heuristic to higher levels and keep it there, thereby affecting the convergence, because the jump down function will not be adjusted properly. Higher than the default value are suggested for longer runs, such as pdoJ=5, or even pdoJ=6. Values of pdoJ higher than 6 will likely have negligible effect on the convergence of the heuristic, but will further slow it down. Recommended (universal) value is 5. 

### pdoPrint

This can be set to four levels (0-3) to control how the program prints progress updates. At 0, the program prints nothing while searching. At 1, the program prints the bottom 10 levels (0-9), the number of visits for each level, and the current values of the jump down function for each level. At 2, the information is similar, but it is about the 10 lowest levels which have been visited (note that these levels might not be represented by consecutive integers). At 3 the information is about the ten levels starting from the level in which the heuristic currently is. The default value is 2. An output window of height 5 lines gives the typical PDO output for options 1-3.

### pdoPrintFreq

This controls how often to display progress updates. When set to 1, the displayed information is changed every time a value of the jump down function is updated. The problem with this setting is that the massive amount of output leads to performance issues. Therefore, the default value is 50. Recommended value is higher, say 500, unless you want to monitor the performance closer, or the parameters are big.

### pdoMaxJDF

This controls how high the jump down function values can be. The default is 5 million (which is more than enough for all feasible searches). We need this bound since the data types for integers have limited space available (we can store numbers up to 2,147,483,647, so pdoMaxJDF cannot be higher).

### startFromFile

This allows you to start looking for an improvement from a given solution. It reads the solution from a text file called startFile.txt. See the Optimization heading below. 

### greedyStart

This uses a kind of greedy approach when first constructing a solution. Normally, the heuristic starts with b random blocks. If  greedyStart=1, then each of the b starting blocks covers at least one m-set not covered by any of the other blocks. Default is 0. Generally, this parameter does not essentially affect the convergence.
bSearch, bFinal

These parameters allow the program to automatically start a new search for a covering of size b-1 if one of size b has been found (or a packing of size b+1 if one of size b has been found). The heuristic starts by looking for a solution with b blocks. If such is found, and bSearch is not 0, the heuristic will automatically search for a solution of size b-1 (covering) or b+1 (packing). If bSearch=1, it will start looking for a new solution from random. If bSearch=2, it will use the old solution and try to improve it. bFinal is the final value of b you are interested in (the smallest size covering, or the largest size packing). bFinal < b for coverings, and bFinal > b for packings.  bSearch=0 is the default (no automatic restart for improvement(s)). Naturally, if bSearch is not 0, then bFinal should be specified. Note that enabling bSearch might be incompatible with startFromFile or greedyStart.

### MemoryLimit

This allows you to manually set a limit for how much memory the program may use (see Memory Allocation below). 

### check

When check=1 the program checks the solution for correctness upon completion, by just verifying that the object indeed meets the definition (i.e. the definition of packing or covering). If you trust the program, or if you do want to check the solution with a different program, then use check=0; the verifying will be omitted. The default value is 1.

### Optimization:

It is possible to use this program as an optimizer, or verifier (see check). You may take an existing solution and have the program start from it with the option (startFromFile=1). Make sure your solution file is called “startFile.txt”, and it is located in the same directory as the binary executable. Also, its blocks must have all their points in the range [0,v-1]. Finally, the points of each block must be separated by space(s), and each block must start on a new line. Here is an example:

	“startFile.txt” for (5,3,2,2,1) covering in 4 blocks:

   0 1 2 
   0 2 3 
   0 2 4 
   1 3 4 




## Memory Allocations:

Cover normally (when set to (the default) OnTheFly=0) pre-computes the entire table of neighbourhood and covering data, in order to make the search more efficient. This might require a lot of memory, so you might receive an error for trying to use too much. The datatypes of many of the parameters can be increased in cover.h at compile time. There are also run-time options associated with memory (such as OnTheFly, MemoryLimit). As of now, the program may output:

### "Binomial coefficient overflow"
Overflow of binCoefType when computing (v choose k)
    "Internal overflow"
Overflow of an internal type. 
    "Invalid parameters"
If one of the following is true (t > k || t > m || k > v || m > v || b <= 0 || t <= 0)
    "Parameter v too large, not enough space reserved"
The maxV parameter must be reduced. (Declared in cover.h)
    "Memory allocation error"
Operating System does not have enough memory for the amount requested. Remove other programs running, or decrease parameters. 
    "Parameter b is larger than maxkSetCount"
Too many blocks in the design. Either decrease b, or increase the size of coveredType
    "Space demands exceed the limit given by MemoryLimit option"
MemoryLimit is a parameter you can set at runtime, it must be increased
    "RankType is too small to contain the binomial coefficients needed."
Overflow of rankType when computing the rank of a block

## Compilation:
This program is compiled for Windows. It is done using TDM-GCC v5.1.0-3 (http://tdm-gcc.tdragon.net/). After installation this provides a directory C:/TDM-GCC-64 with all of the contents. The make utility (which invokes gcc and compiles the program) is located in C:/TDM-GCC-64/bin/mingw32-make. Note you can copy this and rename it if you like. If the entire C:/TDM-GCC-64/bin folder is added as an environment variable in windows, then these programs can be ran from any directory. In order to add the environment variable you can use the following steps:
My Computer -> Properties (use right click menu)
System Properties Window -> Advanced Tab
Select Environment Variables
Highlight the Path variable and click Edit
You can append a directory to this variable (do not delete pre-existing directories). For example it may look like this when you start:
	C:\Program Files; C:\Winnt; C:\Winnt\System32
You would change it to read:
	C:\Program Files; C:\Winnt; C:\Winnt\System32; C:\TDM-GCC-64\bin

You should now be able to invoke  mingw32-make  and compile the program.

## Notes:

The description of the original program COVER is in the report Constructing Covering Designs by Simulated Annealing by Kari J. Nurmela and Patric R. J. Ostergard. Helsinki University of Technology, Digital Systems Laboratory, Series B: Technical Reports, No. 10, January 1993, ISSN 0783-540X, ISBN 951-22-1382-6. A postscript file of this report can be found at

http://www.tcs.hut.fi/old/papers/B10.ps

The description of PDO is in the article Problem dependent optimization (PDO), by Iliya Bluskov, in Journal of Combinatorial Optimization, April 2016, Volume 31, Issue 3, pp 1335-1344; online at 

https://www.researchgate.net/publication/270516923_Problem_dependent_optimization_PDO 

The description of the PDO used in this fork is slightly different than the description in the abovementioned article, namely, in formula (1) in the article, the ceiling function in the right hand side of the formula (1) is removed and the ratio is rewritten as 
f(c(B))+(counter-f(c(B)))/(jdc(c(B))+1) 
to improve performance and avoid multiplication of large numbers (suggestion by Jan de Heer).

Any result obtained by this program should mention PDO-COVER by Bluskov-Germuth-Nurmela-Ostergard. If you are a developer and make improvements or functional changes to this software, please notify one or both authors: Bluskov (at lotbook@telus.net); Germuth (at mailto:germoose0@gmail.com)

See the original readme below:

cover --- a program for searching general covering and packing designs
======================================================================

Authors: Kari J. Nurmela and Patric R. J. \"Osterg\aard.
Version 1.0b.

This program can be freely used whenever the following stipulations
are followed:

1) This program may be used for research only. No commercial use is
   allowed.

2) Whenever the program has been used to obtain results that are
   published, a reference should be made to the report
   `Constructing Covering Designs by Simulated Annealing' by
   Kari J. Nurmela and Patric R. J. \"Osterg\aa rd. (\LaTeX)
   Helsinki University of Technology, Digital Systems Laboratory, 
   Series B: Technical Reports, No. 10, January 1993, ISSN 0783-540X,
   ISBN 951-22-1382-6.

3) Bugs in the program should be reported to Kari.Nurmela@hut.fi or 
   Patric.Ostergard@hut.fi. We also appreciate if improvements and
   new features are sent to the same e-mail addresses.


What is `cover'
--------------

The program `cover' is a program that can be used when searching for
general covering or packing designs. `Cover' searches the designs
using simulated annealing. An introduction to plus a compact
bibliography on covering and packing designs is contained in the
technical report mentioned above. It also contains some documentation
of the program.


Compiling the program
---------------------

The program is written in ANSI C and it should be easily portable to
various environments. A makefile is supplied for automated
compilation. A couple of things should be noted, however.

The CPU-time the program has used is recorded by system call
`getrusage()'. You may have to change this call in `cover.c', if you
want to compile the program in a non-UNIX environment.

The program uses functions `srandom()' and `random()' as the random
number generator. If your system does not have these functions, then
you have to rewrite the macros `randomize', `rnd', `setPRNGSeed' and
`random01' in `cover.h'. If your system offers several different
random number generators, use the best.

The most important data types are defined in `cover.h'. You may wish
to change some of them.

The program modules are:

 cover.c      The main program.
 bincoef.c    Functions needed to calculate and store binomial coefficients.
 tables.c     The functions in this file allocate, compute and
              deallocate the dynamically allocated data tables.
 setoper.c    The functions for manipulation of subsets.
 anneal.c     The simulated annealing algorithm.
 solcheck.c   This file contains several functions useful in debugging
              purposes.
 exp.c        Approximate exponentiation.
 arg.c        Command line option parser.


Using the program
-----------------

The program is run with the command `cover [options]', where options
are of the format `variable=value' The valid variables and their
meanings are

k,t,m,v,l

 These variables are the parameters of the design. The default value for
 l is 1, and if only one of the values m and t are given it is assumed
 that m==t.

b

 This variable is the number of k-sets (blocks) in the design.

TestCount

 This variable determines, how many annealing runs for a given b are
 accomplished. Variable TestCount can be abbreviated TC. Default 1.

CoolingFactor

 This variable is the value for the coefficient in the exponential
 cooling schedule. Abbreviation CF. Default 0.99.

InitProb

 This variable is the value of initial probability of acceptance of
 a cost increasing move in the initial solution. Abbreviation IP.
 Default 0.5.

frozen

 This variable denotes the required number of non-cost-decreasing
 successive temperatures to indicate that the process if frozen. 
 Default 10.

RestrNeighbor

 Set the value of this variable to 1, if you want to use the
 restricted next solution selection mechanism instead of the full
 neighborhood. Abbreviation RN.

InitTemp

 If you want to give the initial temperature directly instead of
 giving InitProb, then use this variable. Abbreviation IT.

L

 If you want to give different value for the permutation count per
 temperature in the simulated annealing algorithm, then use this
 variable. See also LFact below.

LFact

 This is the primary way of giving the permutation count L. If you do
 not give the value of $L$ directly (via the L parameter), the value
 of L is calculated by multiplying the neighborhood size by LFact. The
 default value for LFact is 1. Abbreviation LF.

EndLimit

 Sometimes you may want to stop the annealing even before all sets are
 covered. You can give an limit for an acceptable solution with this 
 variable. Abbreviation EL.

local

 Set this variable to 1, if you want to perform a local optimization
 process instead of simulated annealing.

apprexp

 If you want to use the approximate exponentiation, then set this
 variable to 1.

OntheFly

 Set this variable to 1, if you want the program not to allocate all
 the tables and to compute some of the needed data on-the-fly.
 Abbreviation OF.

Pack

 You can search for packing desings by setting this variable to 1.
 Abbreviation P.

log

 In addition to the output produced to stdout, the program
 records some information to a log file. You can specify the name of
 the log file by the variable log. The default is `cover.log'.

result

 If the program finds a design satisfying the EndLimit requirement, it
 stores the solution in a result file. You can give the result file a
 name with this variable. Default `cover.res'. The file is overwritten
 every time a solution is found, so it will contain only the last
 solution found.

SolX

 Set this variable to 1, if you want the solutions be printed as
 `--X-XX' instead of `2 4 5'. Abbreviation SX.

verbose

 The value of this variable (0, 1 or 2) determines the amount of data
 presented to the user. Default 2.


Example
-------

The command 
   
   cover t=2 k=3 m=2 v=7 b=7 TC=2

tries to find covering design 1-(7,2,3,2) with 7 k-sets (which is also
the value of C(7,2,3,2).


Additional options
------------------

There are some options available in this version of `cover' that are
not mentioned in the report. These are:

  SearchB, SBFact If you set the variable SearchB to 1, the program
                  assumes that there exists a covering design for the given
                  value of b. The program then tries to find a design
                  with as small a value of b as possible. It decreases
                  the given b by multiplying it by SBFact until no
                  design with that value of b is found. Then the
                  program uses kind of a binary search to find the
                  smallest value of b, with which it can find a
                  covering design. Note that these parameters do not
                  currently work with packing designs.

  check           If this variable is set to a nonzero value, the
                  program checks the cost of each final solution and
                  compares it to the cost calculated by summing the
                  cost changes. Useful in debugging when experimenting
                  with different cost functions. The default value of
                  this variable is 1, set it to 0 to prevent cost
                  value checking.

  MemoryLimit     You can set this variable to the amount of bytes
                  that the program is allowed to allocate. This
                  variable is useful, if you run many searches in
                  the background and do not want the program to
                  allocate so much memory that it would slow down the
                  foreground processes. This variable defaults to 0,
                  which means that no upper limit for the memory
                  amount is given.

  PRNGseed        You can give the seed directly to the pseudo-random
                  number generator, this is useful in debugging. If
                  this variable is not given, then the seed is taken
                  from the system clock in the beginning of the
                  program.

Files
-----

README        This file
cover.tar.Z   Compressed tar archive which contains the program files.



Compiling the program
---------------------

Most of the data types are defined in `cover.h'. You may wish to
change them according to the designs that you are to search and
according to the data type sizes of your c-compiler and computer.


Revision history
----------------

Changes from version 1.0 to 1.0a:

  - A bug that forced m==t was fixed in the command line option
    parser.

Changes from 1.0a to 1.0b:

  - `PRNGseed' variable added.

  - the default value of variable `check' changed from 0 to 1.

Miscellaneous notes
-------------------

This version of `cover' is for searching covering and/or packing
designs. If you want to accomplish some performance comparisons or try
to find some minimal/maximal designs automatically, you may wish to
modify the `main()' in cover.c.
