#include "dynarray.h"

#ifndef TOKEN_INCLUDED
#define TOKEN_INCLUDED

/* A Token object can be either ordinary or special. */
enum TokenType {TOKEN_ORDINARY, TOKEN_SPECIAL};

/* A Token is either ordinary or special, expressed as a string. */
typedef struct Token *Token_T;

/*--------------------------------------------------------------------*/

/* Create and return a token whose type is eTokenType and whose
   value consists of string pcValue.  The caller owns the token. */

Token_T Token_newToken(enum TokenType eTokenType, char *pcValue);

/*--------------------------------------------------------------------*/

/* Write all tokens in oTokens to stdout.  First write the token
   title; then write the token name and type */

void Token_writeTokens(DynArray_T oTokens);

/*--------------------------------------------------------------------*/

/* Free all of the tokens in oTokens. */

void Token_freeTokens(DynArray_T oTokens);

/*--------------------------------------------------------------------*/

/* Returns the TokenType of oToken */

enum TokenType Token_getType(Token_T oToken);

/*--------------------------------------------------------------------*/

/* Returns the string value of oToken */

char* Token_getValue(Token_T oToken);

#endif
