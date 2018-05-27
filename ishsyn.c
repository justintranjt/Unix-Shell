#include "dynarray.h"
#include "lexAnalyze.h"
#include "synAnalyze.h"
#include "command.h"
#include "token.h"
#include "program.h"
#include <stdlib.h>

/* Syntactic analyzer client. argc contains the number of arguments in 
   the command line and array argv contains all command line inputs. 
   Returns 0 if arguments successfully broken into tokens and recognized
   as a command. Returns 1 if error. */
int main(int argc, char *argv[])
{
   char *pcLine;
   DynArray_T oTokens;
   Command_T oCommand;
   int iRet;
   const char *pcPgmName;

   /* Current program name. Used for error output */
   Program_initPgm(argv[0]);
   pcPgmName = Program_getPgmName();

   printf("%% ");

   /* Continue reading from stdin until input is NULL */
   while ((pcLine = LexAnalyze_readLine(stdin)) != NULL)
   {
      printf("%s\n", pcLine);

      /* Flush stdout buffer manually */
      iRet = fflush(stdout);
      if (iRet == EOF)
         {perror(pcPgmName); exit(EXIT_FAILURE);}

      /* Analyze input arguments and display */
      oTokens = LexAnalyze_lexLine(pcLine);
      if (oTokens != NULL)
      {
         /* Pass token array to syntactic analyzer to parse command */
         oCommand = SynAnalyze_parseCommand(oTokens);
         if (oCommand != NULL)
         {
            /* Write argument as command then free command */
            Command_writeCommand(oCommand);
            Command_freeCommand(oCommand);
         }
         /* Free token array */
         Token_freeTokens(oTokens);
         DynArray_free(oTokens);
      }
      free(pcLine);
      printf("%% ");
   }

   printf("\n");
   return 0;
}
