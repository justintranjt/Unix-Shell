#define _GNU_SOURCE
#include "dynarray.h"
#include "lexAnalyze.h"
#include "synAnalyze.h"
#include "command.h"
#include "token.h"
#include "program.h"
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* Execute interal command SETENV  with arguments in oCommand 
   built into shell */
static void ish_execSetenv(Command_T oCommand)
{
   Token_T oEnv;
   Token_T oEnvValue;
   size_t uLength;
   const char *pcPgmName;

   uLength = DynArray_getLength(Command_getCommandTokens(oCommand));
   pcPgmName = Program_getPgmName();

   /* Only one argument, needs more to know env. vars to set */
   if (uLength == 1)
      fprintf(stderr, "%s: missing variable\n", pcPgmName);
   /* Two arguments, set env. var. value to empty string */
   else if (uLength == 2)
   {
      oEnv = DynArray_get(Command_getCommandTokens(oCommand), 1);
      setenv(Token_getValue(oEnv), "", 1);
   }
   /* Three arguments, set env. var. value to specified value*/
   else if (uLength == 3)
   {
      oEnv = DynArray_get(Command_getCommandTokens(oCommand), 1);
      oEnvValue = DynArray_get(Command_getCommandTokens(oCommand), 2);
      setenv(Token_getValue(oEnv), Token_getValue(oEnvValue), 1);
   }
   /* More than 2 args, can't set a specified env. var. */
   else if (uLength > 3)
      fprintf(stderr, "%s: too many arguments\n", pcPgmName);
}

/* Execute interal command UNSETENV with arguments in oCommand 
   built into shell */
static void ish_execUnsetenv(Command_T oCommand)
{
   Token_T oEnv;
   size_t uLength;
   const char *pcPgmName;

   uLength = DynArray_getLength(Command_getCommandTokens(oCommand));
   pcPgmName = Program_getPgmName();

   /* Only one argument, needs more to know env. vars to unset */
   if (uLength == 1)
      fprintf(stderr, "%s: missing variable\n", pcPgmName);
   /* More than one argument, can't unset multiple env. vars */
   else if (uLength > 2)
      fprintf(stderr, "%s: too many arguments\n", pcPgmName);
   /* Unset the specified environment variable */
   else if (uLength == 2)
   {
      oEnv = DynArray_get(Command_getCommandTokens(oCommand), 1);
      unsetenv(Token_getValue(oEnv));
   }
}

/* Execute interal command CHDIR with arguments in oCommand 
   built into shell */
static void ish_execChdir(Command_T oCommand)
{
   Token_T oDirectory;
   size_t uLength;
   int iRet;
   const char *pcPgmName;

   uLength = DynArray_getLength(Command_getCommandTokens(oCommand));
   pcPgmName = Program_getPgmName();

   /* Only 1 argument, HOME env variable must be set */
   if (uLength == 1)
   {
      if (getenv("HOME") == NULL)
         fprintf(stderr, "%s: HOME not set\n", pcPgmName);
      else
      {
         iRet = chdir(getenv("HOME"));
         if (iRet == -1)
            fprintf(stderr, "%s: No such file or directory \n", 
               pcPgmName);
      }
   }

   /* 2 arguments, go to specified directory*/
   else if (uLength == 2)
   {
      oDirectory = DynArray_get(Command_getCommandTokens(oCommand), 1);
      iRet = chdir(Token_getValue(oDirectory));
      if (iRet == -1)
         fprintf(stderr, "%s: No such file or directory \n", pcPgmName);
   }

   /* Too many arguments */
   else if (uLength > 2)
      fprintf(stderr, "%s: too many arguments\n", pcPgmName);
}

/* Execute interal command EXIT with arguments in oCommand 
   built into shell. Frees oTokens and pcLine before exit */
static void ish_execExit(Command_T oCommand, DynArray_T oTokens, 
   char *pcLine)
{
   Command_freeCommand(oCommand);
   Token_freeTokens(oTokens);
   DynArray_free(oTokens);
   free(pcLine);
   exit(0);
}

/* Handle stdin and stdout redirect based on files specified by 
   oCommand. Adapted from precept code */
static void ish_redirectIO(Command_T oCommand)
{
   enum {PERMISSIONS = 0600}; /* Owner can read and write */

   const char* pcPgmName;
   int iFd; /* File descriptor flag */
   int iRet; /* Flag for performing operations on file */
   char* pcStdin; /* Name of file stdin is redirected to */
   char* pcStdout; /* Name of file stdout is redirected to */

   pcPgmName = Program_getPgmName();
   pcStdin = Command_getStdin(oCommand);
   pcStdout = Command_getStdout(oCommand);

   /* Redirect stdin to be from a specified file */
   if (pcStdin != NULL)
   {
      /* Open file for read only, error if it doesn't exist. Set 
         file descriptor */
      iFd = open(pcStdin, O_RDONLY);
      if (iFd == -1) {perror(pcPgmName); exit(EXIT_FAILURE);}

      /* Close stdin pointing to /dev/tty */
      iRet = close(0);
      if (iRet == -1) {perror(pcPgmName); exit(EXIT_FAILURE);}

      /* Duplicate file descriptor to be new source of stdin */
      iRet = dup(iFd);
      if (iRet == -1) {perror(pcPgmName); exit(EXIT_FAILURE);}

      /* Close original pointer to file */
      iRet = close(iFd);
      if (iRet == -1) {perror(pcPgmName); exit(EXIT_FAILURE);}
   }

   /* Redirect stdout to be from a specified file */
   if (pcStdout != NULL)
   {
      /* Create file for read and write. Set file descriptor to file */
      iFd = creat(pcStdout, PERMISSIONS);
      if (iFd == -1) {perror(pcPgmName); exit(EXIT_FAILURE);}

      /* Close stdout pointing to /dev/tty */
      iRet = close(1);
      if (iRet == -1) {perror(pcPgmName); exit(EXIT_FAILURE);}

      /* Duplicate file descriptor to be new source of stdout */
      iRet = dup(iFd);
      if (iRet == -1) {perror(pcPgmName); exit(EXIT_FAILURE);}

      /* Close original pointer to file */
      iRet = close(iFd);
      if (iRet == -1) {perror(pcPgmName); exit(EXIT_FAILURE);}
   }
}

/* Handles first SIGINT signal. Reset alarm for 3 seconds. Takes 
   number of signal that caused function to be called iSignal as input.
   Second SIGINT signal results in default SIGINT exit from shell */
static void ish_myINTHandler(int iSignal)
{
   enum{SLEEP_SECONDS = 3};
   const char *pcPgmName;
   void (*pfRet)(int); /* Signal handler */

   pcPgmName = Program_getPgmName();

   pfRet = signal(SIGINT, SIG_DFL);
   if (pfRet == SIG_ERR) {perror(pcPgmName); exit(EXIT_FAILURE);}

   alarm(SLEEP_SECONDS);
}

/* ALRM handler does not print default "Alarm clock" message. Takes 
   number of signal that caused function to be called iSignal as 
   input. */
static void ish_myALRMHandler(int iSignal)
{
   const char *pcPgmName;
   void (*pfRet)(int); /* Signal handler */

   pcPgmName = Program_getPgmName();

   pfRet = signal(SIGINT, ish_myINTHandler);
   if (pfRet == SIG_ERR) {perror(pcPgmName); exit(EXIT_FAILURE);}
}

/* Execute external command oCommand not built into shell */
static void ish_execExternCommand(Command_T oCommand)
{
   pid_t iPid; /* Process ID */
   size_t u; /* Index */
   size_t uLength; /* Length of oCommand tokens array */
   Token_T oToken; /* Token used to get value of args */
   char** apcArgv; /* Array of string args */
   const char* pcPgmName;
   void (*pfRet)(int); /* Signal handler */

   pcPgmName = Program_getPgmName();

   uLength = DynArray_getLength(Command_getCommandTokens(oCommand));
   apcArgv = malloc(sizeof(char*) * (uLength + 1));
   if (apcArgv == NULL)
      {perror(pcPgmName); exit(EXIT_FAILURE);}

   /* Parent process handling command, ignore SIGINT */
   pfRet = signal(SIGINT, SIG_IGN);
   if (pfRet == SIG_ERR) {perror(pcPgmName); exit(EXIT_FAILURE);}

   /* Populate argument array for child process to execute */
   for (u = 0; u < uLength; u++)
   {
      oToken = DynArray_get(Command_getCommandTokens(oCommand), u);
      apcArgv[u] = Token_getValue(oToken);
   }
   /* Null terminator for execvp */
   apcArgv[uLength] = NULL;

   iPid = fork();
   if (iPid == -1) {perror(pcPgmName); exit(EXIT_FAILURE);}

   /* Child process executes command with args */
   if (iPid == 0)
   {
      /* Default signal behavior with SIGINT in child */
      pfRet = signal(SIGINT, SIG_DFL);
      if (pfRet == SIG_ERR) {perror(pcPgmName); exit(EXIT_FAILURE);}

      /* Handle stdin and stdout redirection first*/
      ish_redirectIO(oCommand);

      /* Execute program */
      execvp(apcArgv[0], apcArgv);
      perror(pcPgmName);
      exit(EXIT_FAILURE);
   }

   /* Wait for child to exit */
   iPid = wait(NULL);
   if (iPid == -1) {perror(pcPgmName); exit(EXIT_FAILURE);}

   free(apcArgv);
}

/* Decide whether oCommand is executed externally or built into shell.
   In case of exit, oTokens and pcLine need to be freed from memory */
static void ish_execSelection(Command_T oCommand, DynArray_T oTokens, 
   char *pcLine)
{
   Token_T oToken;

   oToken = DynArray_get(Command_getCommandTokens(oCommand), 0);

   /* Determine command action */
   if (strcmp(Token_getValue(oToken), "setenv") == 0)
      ish_execSetenv(oCommand);

   else if (strcmp(Token_getValue(oToken), "unsetenv") == 0)
      ish_execUnsetenv(oCommand);

   else if (strcmp(Token_getValue(oToken), "cd") == 0)
      ish_execChdir(oCommand);

   else if (strcmp(Token_getValue(oToken), "exit") == 0)
      ish_execExit(oCommand, oTokens, pcLine);
   /* If no matching built in command, execute external command */
   else
      ish_execExternCommand(oCommand);
}

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
   void (*pfRet)(int); /* Signal handler */

   /* Current program name. Used for error output */
   Program_initPgm(argv[0]);
   pcPgmName = Program_getPgmName();

   printf("%% ");

   /* Install alarm handler for SIGALRM signals. */
   pfRet = signal(SIGALRM, ish_myALRMHandler);
   if (pfRet == SIG_ERR) {perror(pcPgmName); exit(EXIT_FAILURE);}

   /* Install ish_myINTHandler as handler for SIGINT signals while 
      parent not handling commands */
   pfRet = signal(SIGINT, ish_myINTHandler);
   if (pfRet == SIG_ERR) {perror(pcPgmName); exit(EXIT_FAILURE);}

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
            /* Execute commands in shell then free command */
            ish_execSelection(oCommand, oTokens, pcLine);

            /* Restore custom SIGINT handler. Parent is not handling
             command anymore */
            pfRet = signal(SIGALRM, ish_myALRMHandler);
            if (pfRet == SIG_ERR) 
               {perror(pcPgmName); exit(EXIT_FAILURE);}

            pfRet = signal(SIGINT, ish_myINTHandler);
            if (pfRet == SIG_ERR) 
               {perror(pcPgmName); exit(EXIT_FAILURE);}

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
