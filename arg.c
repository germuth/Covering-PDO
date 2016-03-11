/*   arg.c
**
**   This file contains functions for parsing the arguments from the
**   command line.
**
*/


#include <stdio.h>
#include <string.h>
#include "cover.h"
#include "arg.h"
#include "tables.h"


struct varstruct {
  char *varname;
  char *format;
  void *varaddr;
} vars[] =
{
  {"k", "%d", &k},
  {"t", "%d", &t},
  {"m", "%d", &m},
  {"v", "%d", &v},
  {"l", "%d", &coverNumber}, /* lambda is coverNumber in the program! */
  {"b", "%d", &b},
  {"TestCount", "%d", &testCount},
  {"TC", "%d", &testCount},
  {"CoolingFactor", "%f", &coolFact},
  {"CF", "%f", &coolFact},
  {"InitProb", "%f", &initProb},
  {"IP", "%f", &initProb},
  {"frozen", "%d", &frozen},
  {"RestrNeighbor", "%d", &restrictedNeighbors},
  {"RN", "%d", &restrictedNeighbors},
  {"InitTemp", "%f", &initialT},
  {"IT", "%f", &initialT},
  {"L", "%d", &L},
  {"LFact", "%f", &LFact},
  {"LF", "%f", &LFact},
  {"EndLimit", "%d", &endLimit},
  {"EL", "%d", &endLimit},
  {"local", "%d", &localOpt},
  {"exhaustive", "%d", &exhaust},
  {"pdo", "%d", &pdoFlag},
  {"randomStart", "%d", &randomStartFlag},
  {"greedyStart", "%d", &greedyStartFlag},
  {"apprexp", "%d", &apprexp},
  {"OntheFly", "%d", &onTheFly},
  {"OF", "%d", &onTheFly},
  {"Pack", "%d", &pack},
  {"P", "%d", &pack},
  {"SolX", "%d", &solX},
  {"SX", "%d", &solX},
  {"verbose", "%d", &verbose},
  {"MemoryLimit", "%lu", &memoryLimit},
  {"ML", "%lu", &memoryLimit},
  {"check", "%d", &check},
  {"SearchB", "%d", &searchB},
  {"SB", "%d", &searchB},
  {"SBFact", "%f", &SBFact},
  {"SBF", "%f", &SBFact},
  {"PRNGseed", "%u", &PRNGseed},
  {"PRNG", "%u", &PRNGseed},
};

#define OPT_COUNT (sizeof(vars) / sizeof(struct varstruct))


static void errmsg(char *msg)
{
  int i;
  static char format[40];

  fprintf(stderr, "Invalid option: %s\nValid options are:\n\n"
	  "       variable   format   default\n"
	  "       --------   ------   -------\n", msg);
  for(i = 0; i < OPT_COUNT; i++) {
    fprintf(stderr, "%15s   %6s   ", vars[i].varname, vars[i].format);
    strcpy(format, vars[i].format);
    strcat(format, "\n");
    if(strchr(vars[i].format, 'd'))
      fprintf(stderr, format, *((int *)vars[i].varaddr));
    else if(strchr(vars[i].format, 'f'))
      fprintf(stderr, "%4.2f\n", *((float *)vars[i].varaddr));
    else if(strchr(vars[i].format, 's'))
      fprintf(stderr, format, (char *)vars[i].varaddr);
    else if(strstr(vars[i].format, "lu"))
      fprintf(stderr, format, *((long unsigned *)vars[i].varaddr));
    /* add any types you wish */
  }
  coverError(SEE_ABOVE_ERROR);
}


#define TMP_BUF_SIZE 50

void parseArguments(int argc, char **argv)
{
  int i, j, eq;
  char name[TMP_BUF_SIZE];
  char value[TMP_BUF_SIZE];
  char *tmp;
  int found;
  int m_defined = 0, t_defined = 0;

  for(i = 1; i < argc; i++) {
    if((tmp = strchr(argv[i], '=')) == NULL)
      errmsg(argv[i]);
    eq = tmp - argv[i] + 1;
    strncpy(name, argv[i], min(eq - 1, TMP_BUF_SIZE - 1));
    name[min(eq - 1, TMP_BUF_SIZE - 1)] = '\0';
    strncpy(value, argv[i] + eq, min(strlen(argv[i]) - eq, TMP_BUF_SIZE - 1));
    value[min(strlen(argv[i]) - eq, TMP_BUF_SIZE - 1)] = '\0';
    found = 0;
    for(j = 0; j < OPT_COUNT; j++)
      if(!strcmp(vars[j].varname, name)) {
	found++;
	if(sscanf(value, vars[j].format, vars[j].varaddr) != 1)
	  errmsg(argv[i]);
	if(!strcmp(name, "IT") || !strcmp(name, "InitTemp"))
	  Tset = 1;
	if(!strcmp(name, "L"))
	  Lset = 1;
	if(!strcmp(name, "m") && !t_defined) { /* default: t=m if either one */
	  t = m;                               /* is not given */
	  m_defined = 1;
	}
	if(!strcmp(name, "t") && !m_defined) {
	  m = t;
	  t_defined = 1;
	}
	if(!strcmp(name, "PRNGseed") || !strcmp(name, "PRNG"))
	  setPRNGseed(PRNGseed);
	break;
      }
    if(!found)
      errmsg(argv[i]);
  }
}
