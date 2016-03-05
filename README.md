# ProblemDependantOptimization

This program is an altered form of Cover by Kari J. Nurmela and Patric R. J. \"Osterg\aard. It has been edited to replace simulated annealing with problem dependant optimization.

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
