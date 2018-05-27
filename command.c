#include "dynarray.h"
#include "command.h"
#include "program.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* A command consists of name and arg token in a DynArray as well as 
   file redirect strings */
struct Command
{
   /* Command name and Command arguments */
   DynArray_T oCommandTokens;

   /* Stdin redirection string */
   char* pcStdinRedirect;

   /* Stdout redirection string */
   char* pcStdoutRedirect;
};

Command_T Command_newCommand(DynArray_T oNewCommandTokens, 
   char* pcNewStdin, char* pcNewStdout)
{
   Command_T oCommand;
   const char *pcPgmName;

   assert(oNewCommandTokens != NULL);

   pcPgmName = Program_getPgmName();

   /* Allocate space for new command and set oCommandTokens field */
   oCommand = (Command_T)malloc(sizeof(struct Command));
   if (oCommand == NULL)
      {perror(pcPgmName); exit(EXIT_FAILURE);}
   oCommand->oCommandTokens = oNewCommandTokens;

   /* Set redirect string fields */
   oCommand->pcStdinRedirect = pcNewStdin;
   oCommand->pcStdoutRedirect = pcNewStdout;

   return oCommand;
}

void Command_writeCommand(Command_T oCommand)
{
   size_t u;
   size_t uLength;
   Token_T oToken;

   assert(oCommand != NULL);

   uLength = DynArray_getLength(oCommand->oCommandTokens);
   assert(uLength != 0);

   /* First command is always the name token */
   oToken = DynArray_get(oCommand->oCommandTokens, 0);
   assert(oToken != NULL);
   printf("Command name: %s\n", Token_getValue(oToken));

   /* Additional commands are args */
   for (u = 1; u < uLength; u++)
   {
      oToken = DynArray_get(oCommand->oCommandTokens, u);
      assert(oToken != NULL);
      printf("Command arg: %s\n", Token_getValue(oToken));
   }

   /* Name of file to which stdin/stdout is redirected to */
   if (oCommand->pcStdinRedirect != NULL)
      printf("Command stdin: %s\n", oCommand->pcStdinRedirect);
   if (oCommand->pcStdoutRedirect != NULL)
      printf("Command stdout: %s\n", oCommand->pcStdoutRedirect);
}

void Command_freeCommand(Command_T oCommand)
{
   assert (oCommand != NULL);

   DynArray_free(oCommand->oCommandTokens);
   free(oCommand);
}

DynArray_T Command_getCommandTokens(Command_T oCommand)
{
   return oCommand->oCommandTokens;
}

char* Command_getStdin(Command_T oCommand)
{
   return oCommand->pcStdinRedirect;
}

char* Command_getStdout(Command_T oCommand)
{
   return oCommand->pcStdoutRedirect;
}
