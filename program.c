/*--------------------------------------------------------------------*/
/* Assignment 7                                                       */
/* program.c                                                          */
/* Author: Justin Tran                                                */
/*--------------------------------------------------------------------*/
#include <assert.h>
#include <stdlib.h>

/* The name of the executable binary file. */
static const char *pcPgmName;

void Program_initPgm(const char *pcPgmTitle)
{
   assert(pcPgmTitle != NULL);
   
   pcPgmName = pcPgmTitle;
}

const char *Program_getPgmName(void)
{
   return pcPgmName;
}