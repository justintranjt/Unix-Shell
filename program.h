#ifndef PROGRAM_INCLUDED
#define PROGRAM_INCLUDED

/*--------------------------------------------------------------------*/

/* Initialize program pcPgmTitle so error messages can contain program 
   name */

void Program_initPgm(const char *pcPgmTitle);

/*--------------------------------------------------------------------*/

/* Return name of program for error output */

const char *Program_getPgmName(void);

#endif
