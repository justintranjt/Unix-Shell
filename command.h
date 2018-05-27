/*--------------------------------------------------------------------*/
/* Assignment 7                                                       */
/* command.h                                                          */
/* Author: Justin Tran                                                */
/*--------------------------------------------------------------------*/

#include "dynarray.h"

#ifndef COMMAND_INCLUDED
#define COMMAND_INCLUDED

/* A Command consists of name and arg tokens stored in a DynArray_T
   along with stdin and stdout flie redirection strings */
typedef struct Command *Command_T;

/*--------------------------------------------------------------------*/

/* Create and return a command containing a DynArray oNewCommandTokens
   consisting of command name and args as well as names of files 
   specified by pcNewStdin and pcNewStdout */

Command_T Command_newCommand(DynArray_T oNewCommandTokens, 
   char* pcNewStdin, char* pcNewStdout);

/*--------------------------------------------------------------------*/

/* Write all command tokens in oCommand to stdout. First specify a 
   command then its type; then write the token name or redirect. */

void Command_writeCommand(Command_T oCommand);

/*--------------------------------------------------------------------*/

/* Free all of the tokens in oCommand and free the command itself. */

void Command_freeCommand(Command_T oCommand);

/*--------------------------------------------------------------------*/

/* Return the field oCommandTokens from oCommand containing name and 
   args */

DynArray_T Command_getCommandTokens(Command_T oCommand);

/*--------------------------------------------------------------------*/

/* Return the field pcStdinRedirect from oCommand containing the name of 
   the file specified by stdin */

char* Command_getStdin(Command_T oCommand);

/*--------------------------------------------------------------------*/

/* Return the field pcStdoutRedirect from oCommand containing the name of 
   the file specified by stdout */

char* Command_getStdout(Command_T oCommand);

#endif
