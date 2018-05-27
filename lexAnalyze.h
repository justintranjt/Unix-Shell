/*--------------------------------------------------------------------*/
/* Assignment 7                                                       */
/* lexAnalyze.h                                                       */
/* Author: Justin Tran                                                */
/*--------------------------------------------------------------------*/

#include <stdio.h>
#include "dynarray.h"

#ifndef LEXICAL_INCLUDED
#define LEXICAL_INCLUDED

/*--------------------------------------------------------------------*/

/* If no lines remain in psFile, then return NULL. Otherwise read a line
   of psFile and return it as a string. The string does not contain a
   terminating newline character. The caller owns the string. */

char* LexAnalyze_readLine(FILE *psFile);

/*--------------------------------------------------------------------*/

/* Lexically analyze string pcLine.  If pcLine contains a lexical
   error, then return NULL.  Otherwise return a DynArray object
   containing the tokens in pcLine.  The caller owns the DynArray
   object and the tokens that it contains. */

DynArray_T LexAnalyze_lexLine(const char *pcLine);

#endif
