#include <stdio.h>
#include "dynarray.h"
#include "command.h"

#ifndef SYNTAC_INCLUDED
#define SYNTAC_INCLUDED

/*--------------------------------------------------------------------*/

/* Syntactically analyze DynArray object containing oTokens to create 
   and return a command. If oTokens contains a syntactic error then 
   return NULL. Otherwise return a Command_T object containing the 
   tokens in pcLine. The caller owns the Command_T object and the
   tokens and strings that it contains. */

Command_T SynAnalyze_parseCommand(DynArray_T oTokens);

#endif
