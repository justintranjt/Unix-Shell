/*--------------------------------------------------------------------*/
/* Assignment 7                                                       */
/* lexAnalyze.c                                                       */
/* Author: Justin Tran                                                */
/*--------------------------------------------------------------------*/
#include "dynarray.h"
#include "lexAnalyze.h"
#include "token.h"
#include "program.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

char *LexAnalyze_readLine(FILE *psFile)
{
   const size_t INITIAL_LINE_LENGTH = 2;
   const size_t GROWTH_FACTOR = 2;

   size_t uLineLength = 0;
   size_t uPhysLineLength = INITIAL_LINE_LENGTH;
   char *pcLine;
   int iChar;
   const char *pcPgmName;

   assert(psFile != NULL);

   /* If no lines remain, return NULL. */
   if (feof(psFile))
      return NULL;
   iChar = fgetc(psFile);
   if (iChar == EOF)
      return NULL;

   /* Get program name for error output*/
   pcPgmName = Program_getPgmName();

   /* Allocate memory for the string. */
   pcLine = (char*)malloc(uPhysLineLength);
   if (pcLine == NULL)
      {perror(pcPgmName); exit(EXIT_FAILURE);}

   /* Read characters into the string. */
   while ((iChar != '\n') && (iChar != EOF))
   {
      if (uLineLength == uPhysLineLength)
      {
         uPhysLineLength *= GROWTH_FACTOR;
         pcLine = (char*)realloc(pcLine, uPhysLineLength);
         if (pcLine == NULL)
            {perror(pcPgmName); exit(EXIT_FAILURE);}
      }
      pcLine[uLineLength] = (char)iChar;
      uLineLength++;
      iChar = fgetc(psFile);
   }

   /* Append a null character to the string. */
   if (uLineLength == uPhysLineLength)
   {
      uPhysLineLength++;
      pcLine = (char*)realloc(pcLine, uPhysLineLength);
      if (pcLine == NULL)
         {perror(pcPgmName); exit(EXIT_FAILURE);}
   }
   pcLine[uLineLength] = '\0';

   return pcLine;
}

/* Takes single char c as input. Return 1 if c is an angle bracket.
   Else return 0. */
static int LexAnalyze_isspecial(char c)
{
   return (c == '>' || c == '<');
}

/* Takes single char c as input. Return 1 if c is a quotation mark.
   Else return 0. */
static int LexAnalyze_isquote(char c)
{
   return (c == '\"');
}

/* Create a special token designated by char c. Add to the DynArray 
   called oTokens */
static void LexAnalyze_createSpecialToken(char c, DynArray_T oTokens)
{
   char pcBuffer[2];
   Token_T oToken;
   int iSuccessful;
   const char *pcPgmName;

   assert(oTokens != NULL);

   pcPgmName = Program_getPgmName();
   /* Insert special character followed by EOF */
   pcBuffer[0] = c;
   pcBuffer[1] = '\0';
   oToken = Token_newToken(TOKEN_SPECIAL, pcBuffer);
   iSuccessful = DynArray_add(oTokens, oToken);
   if (!iSuccessful)
      {perror(pcPgmName); exit(EXIT_FAILURE);}
}

/* Create an ordinary token designated by all chars in pcBuffer and 
   add to oTokens. Current position in pcBuffer given by uBufferIndex */
static void LexAnalyze_createOrdinaryToken(char *pcBuffer, DynArray_T 
   oTokens, size_t uBufferIndex)
{
   Token_T oToken;
   int iSuccessful;
   const char *pcPgmName;

   assert(pcBuffer != NULL);
   assert(oTokens != NULL);

   pcPgmName = Program_getPgmName();
   /* Insert EOF at end of current buffer position uBufferIndex */
   pcBuffer[uBufferIndex] = '\0';
   oToken = Token_newToken(TOKEN_ORDINARY, pcBuffer);
   iSuccessful = DynArray_add(oTokens, oToken);
   if (!iSuccessful)
      {perror(pcPgmName); exit(EXIT_FAILURE);}
}

DynArray_T LexAnalyze_lexLine(const char *pcLine)
{
   /* lexLine() uses a DFA approach.  It "reads" its characters from
      pcLine. The DFA has these three states: */
   enum LexState {STATE_START, STATE_ORDINARY, STATE_SPECIAL, 
      STATE_QUOTE_BEGIN, STATE_QUOTE_END};

   /* The current state of the DFA. */
   enum LexState eState = STATE_START;

   /* An index into pcLine. */
   size_t uLineIndex = 0;

   /* Pointer to a buffer in which the characters comprising each
      token are accumulated. */
   char *pcBuffer;

   /* An index into the buffer. */
   size_t uBufferIndex = 0;

   char c;
   DynArray_T oTokens;
   const char *pcPgmName;

   assert(pcLine != NULL);

   /* Get program name for error output*/
   pcPgmName = Program_getPgmName();

   /* Create an empty token DynArray object. */
   oTokens = DynArray_new(0);
   if (oTokens == NULL)
      {perror(pcPgmName); exit(EXIT_FAILURE);}

   /* Allocate memory for a buffer that is large enough to store the
      largest token that might appear within pcLine. */
   pcBuffer = (char*)malloc(strlen(pcLine) + 1);
   if (pcBuffer == NULL)
      {perror(pcPgmName); exit(EXIT_FAILURE);}

   for (;;)
   {
      /* "Read" the next character from pcLine. */
      c = pcLine[uLineIndex++];

      switch (eState)
      {
         /* Handle the START state. */
         case STATE_START:
            if (c == '\0')
            {
               free(pcBuffer);
               return oTokens;
            }
            else if (isspace(c))
               eState = STATE_START;
            else if (LexAnalyze_isspecial(c))
            {
               LexAnalyze_createSpecialToken(c, oTokens);
               uBufferIndex = 0;
               eState = STATE_SPECIAL;
            }
            else if (LexAnalyze_isquote(c))
               eState = STATE_QUOTE_BEGIN;
            else
            {
               pcBuffer[uBufferIndex++] = c;
               eState = STATE_ORDINARY;
            }
            break;

         /* Handle the STATE_ORDINARY state. */
         case STATE_ORDINARY:
            if (c == '\0')
            {
               LexAnalyze_createOrdinaryToken(pcBuffer, oTokens, uBufferIndex);
               uBufferIndex = 0;
               free(pcBuffer);
               return oTokens;
            }
            else if (isspace(c))
            {
               LexAnalyze_createOrdinaryToken(pcBuffer, oTokens, uBufferIndex);
               uBufferIndex = 0;
               eState = STATE_START;
            }
            else if (LexAnalyze_isspecial(c))
            {
               LexAnalyze_createOrdinaryToken(pcBuffer, oTokens, uBufferIndex);
               LexAnalyze_createSpecialToken(c, oTokens);
               uBufferIndex = 0;
               eState = STATE_SPECIAL;
            }
            else if (LexAnalyze_isquote(c))
               eState = STATE_QUOTE_BEGIN;
            else
            {
               pcBuffer[uBufferIndex++] = c;
               eState = STATE_ORDINARY;
            }
            break;

         /* Handle the STATE_SPECIAL state. */
         case STATE_SPECIAL:
            if (c == '\0')
            {
               uBufferIndex = 0;
               free(pcBuffer);
               return oTokens;
            }
            else if (isspace(c))
            {
               uBufferIndex = 0;
               eState = STATE_START;
            }
            else if (LexAnalyze_isspecial(c))
            {
               LexAnalyze_createSpecialToken(c, oTokens);
               uBufferIndex = 0;
               eState = STATE_SPECIAL;
            }
            else if (LexAnalyze_isquote(c))
               eState = STATE_QUOTE_BEGIN;
            else
            {
               pcBuffer[uBufferIndex++] = c;
               eState = STATE_ORDINARY;
            }
            break;

         /* Handle the STATE_QUOTE_BEGIN state. */
         case STATE_QUOTE_BEGIN:
            if (c == '\0')
            {
               /* Error from unclosed quote */
               fprintf(stderr, "%s: unmatched quote\n", pcPgmName);
               free(pcBuffer);
               Token_freeTokens(oTokens);
               DynArray_free(oTokens);
               return NULL;
            }
            else if (LexAnalyze_isquote(c))
               eState = STATE_QUOTE_END;
            else
            {
               pcBuffer[uBufferIndex++] = c;
               eState = STATE_QUOTE_BEGIN;
            }
            break;

         /* Handle the STATE_QUOTE_END state. */
         case STATE_QUOTE_END:
            if (c == '\0')
            {
               LexAnalyze_createOrdinaryToken(pcBuffer, oTokens, uBufferIndex);
               uBufferIndex = 0;
               free(pcBuffer);
               return oTokens;
            }
            else if (isspace(c))
            {
               LexAnalyze_createOrdinaryToken(pcBuffer, oTokens, uBufferIndex);
               uBufferIndex = 0;
               eState = STATE_START;
            }
            else if (LexAnalyze_isspecial(c))
            {
               LexAnalyze_createOrdinaryToken(pcBuffer, oTokens, uBufferIndex);
               LexAnalyze_createSpecialToken(c, oTokens);
               uBufferIndex = 0;
               eState = STATE_SPECIAL;
            }
            else if (LexAnalyze_isquote(c))
               eState = STATE_QUOTE_BEGIN;
            else
            {
               pcBuffer[uBufferIndex++] = c;
               eState = STATE_ORDINARY;
            }
            break;

         default:
            assert(0);
      }
   }
}
