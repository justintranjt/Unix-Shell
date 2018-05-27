/*--------------------------------------------------------------------*/
/* Assignment 7                                                       */
/* synAnalyze.c                                                       */
/* Author: Justin Tran                                                */
/*--------------------------------------------------------------------*/
#include "dynarray.h"
#include "synAnalyze.h"
#include "command.h"
#include "token.h"
#include "program.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

Command_T SynAnalyze_parseCommand(DynArray_T oTokens)
{
   Command_T oCommand; /* New command to be returned */
   Token_T oToken; /* Current command in oTokens */
   Token_T oNextToken; /* Previous token in oTokens */
   size_t u; /* Index */
   size_t uLength; /* Number of tokens in oTokens */
   const char *pcPgmName; /* Program name for errors */
   int stdinCount; /* Number of stdin redirects */
   int stdoutCount; /* Number of stdout redirects */

   DynArray_T oCommandTokens; /* Arguments to be in new command */
   char* pcStdinRedirect; /* Stdin to be in new command */
   char* pcStdoutRedirect; /* Stdout to be in new command */

   assert(oTokens != NULL);
   
   /* Initialize oCommand fields */
   uLength = DynArray_getLength(oTokens);
   pcPgmName = Program_getPgmName();
   stdinCount = 0;
   stdoutCount = 0;
   oCommandTokens = DynArray_new(0);
   pcStdinRedirect = NULL;
   pcStdoutRedirect = NULL;

   /* Prevent processing of whitespace tokens */
   if (uLength == 0)
      return NULL;
   
   /* Check for initial errors. First token can't be special */
   oToken = DynArray_get(oTokens, 0);
   if (oToken == NULL)
      {perror(pcPgmName); exit(EXIT_FAILURE);}
   if (Token_getType(oToken) == TOKEN_SPECIAL)
   {
      DynArray_free(oCommandTokens);
      fprintf(stderr, "%s: missing command name\n", pcPgmName);
      return NULL;
   }

   /* Last token can't be special */
   oToken = DynArray_get(oTokens, uLength - 1);
   if (oToken == NULL)
      {perror(pcPgmName); exit(EXIT_FAILURE);}
   if (strcmp(Token_getValue(oToken), "<") == 0 && 
      Token_getType(oToken) == TOKEN_SPECIAL)
   {
      DynArray_free(oCommandTokens);
      fprintf(stderr, 
         "%s: standard input redirection without file name\n", 
         pcPgmName);
      return NULL;
   }
   else if (strcmp(Token_getValue(oToken), ">") == 0 && 
      Token_getType(oToken) == TOKEN_SPECIAL)
   {
      DynArray_free(oCommandTokens);
      fprintf(stderr, 
         "%s: standard output redirection without file name\n", 
         pcPgmName);
      return NULL;
   }

   /* Loop through tokens to create command */
   for (u = 0; u < uLength; u++)
   {
      oToken = DynArray_get(oTokens, u);

      /* Add ordinary name and args to DynArray */
      if (Token_getType(oToken) == TOKEN_ORDINARY)
         if (!DynArray_add(oCommandTokens, oToken))
            {perror(pcPgmName); exit(EXIT_FAILURE);}

      /* Set stdin redirect fields then check for consecutive special 
         tokens and add redirect files */
      if (u != uLength - 1)
      {
         oNextToken = DynArray_get(oTokens, u + 1);

         if (strcmp(Token_getValue(oToken), "<") == 0 && 
            Token_getType(oToken) == TOKEN_SPECIAL)
         {
            pcStdinRedirect = Token_getValue(oNextToken);
            stdinCount++;
            u++;
         }
         else if (strcmp(Token_getValue(oToken), ">") == 0 && 
            Token_getType(oToken) == TOKEN_SPECIAL)
         {
            pcStdoutRedirect = Token_getValue(oNextToken);
            stdoutCount++;
            u++;
         }

         if (strcmp(Token_getValue(oToken), "<") == 0 && 
             Token_getType(oNextToken) == TOKEN_SPECIAL)
         {
            DynArray_free(oCommandTokens);
            fprintf(stderr, 
               "%s: standard input redirection without file name\n", 
               pcPgmName);
            return NULL;
         }
         if (strcmp(Token_getValue(oToken), ">") == 0 && 
             Token_getType(oNextToken) == TOKEN_SPECIAL)
         {
            DynArray_free(oCommandTokens);
            fprintf(stderr, 
               "%s: standard output redirection without file name\n", 
               pcPgmName);
            return NULL;
         }
      }

      /* Check for multiple redirections */
      if (stdinCount > 1)
      {
         DynArray_free(oCommandTokens);
         fprintf(stderr, 
            "%s: multiple redirection of standard input\n", 
            pcPgmName);
         return NULL;
      }
      if (stdoutCount > 1)
      {
         DynArray_free(oCommandTokens);
         fprintf(stderr, 
            "%s: multiple redirection of standard output\n", 
            pcPgmName);
         return NULL;
      }
   }

   /* Create and return parsed command */
   oCommand = Command_newCommand(oCommandTokens, pcStdinRedirect, 
      pcStdoutRedirect);
   if (oCommand == NULL)
      {perror(pcPgmName); exit(EXIT_FAILURE);}
   return oCommand;
}
