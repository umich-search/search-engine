#include <iostream>
#include <../utility/include/mString.h>
#include <Tokens.h>

NotBuiltin::NotBuiltin(char *string) : string(string)
   {
   }

Builtin::Builtin(TokenType token) : token(token)
   {

   }

Token* TokenStream::CurrentToken( )
   {
   return currentToken;
   }

Token* TokenStream::TakeToken()
   {

   }


bool TokenStream::Match( TokenType t )
   {
   return ( currentToken->GetTokenType() == t );
   }

TokenStream::TokenStream(std::ifstream &is)
   {
   is >> currentTokenString;
   }

TokenStream::TokenStream(char *filename)
   {
   std::ifstream docs;
   docs.open(filename);
   docs >> currentTokenString;
   currentToken = new Builtin(TokenInvalid);
   }

