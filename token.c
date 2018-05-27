/*--------------------------------------------------------------------*/
/* Assignment 7                                                       */
/* token.c                                                            */
/* Author: Justin Tran                                                */
/*--------------------------------------------------------------------*/
#include "dynarray.h"
#include "token.h"
#include "program.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* A token consists of a type and value */
struct Token
{
   /* The type of the token. */
   enum TokenType eType;

   /* The string which is the token's value. */
   char *pcValue;
};

Token_T Token_newToken(enum TokenType eTokenType, char *pcValue)
{
   struct Token *psToken;
   const char *pcPgmName;

   assert(pcValue != NULL);
   pcPgmName = Program_getPgmName();

   psToken = (struct Token*)malloc(sizeof(struct Token));
   if (psToken == NULL)
      {perror(pcPgmName); exit(EXIT_FAILURE);}
   psToken->eType = eTokenType;
   psToken->pcValue = (char*)malloc(strlen(pcValue) + 1);
   if (psToken->pcValue == NULL)
      {perror(pcPgmName); exit(EXIT_FAILURE);}
   strcpy(psToken->pcValue, pcValue);

   return psToken;
}

void Token_writeTokens(DynArray_T oTokens)
{
   size_t u;
   size_t uLength;
   struct Token *psToken;

   assert(oTokens != NULL);

   uLength = DynArray_getLength(oTokens);

   for (u = 0; u < uLength; u++)
   {
      psToken = DynArray_get(oTokens, u);
      assert(psToken != NULL);

      /* Write token with appropriate type */
      if (psToken->eType == TOKEN_SPECIAL)
         printf("Token: %s (special)\n", psToken->pcValue);
      else if (psToken->eType == TOKEN_ORDINARY)
         printf("Token: %s (ordinary)\n", psToken->pcValue);
   }
}

void Token_freeTokens(DynArray_T oTokens)
{
   size_t u;
   size_t uLength;
   struct Token *psToken;

   assert(oTokens != NULL);

   uLength = DynArray_getLength(oTokens);

   for (u = 0; u < uLength; u++)
   {
      psToken = DynArray_get(oTokens, u);
      assert(psToken != NULL);

      free(psToken->pcValue);
      free(psToken);
   }
}

enum TokenType Token_getType(Token_T oToken)
{
   return (oToken->eType);
}

char* Token_getValue(Token_T oToken)
{
   return (oToken->pcValue);
}
