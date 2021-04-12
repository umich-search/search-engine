#include <iostream>
#include <../utility/include/mString.h>
#include <fstream>

enum TokenType
    {
    TokenInvalid = -1, TokenEOF = 0,
    TokenNotBuiltin,
    TokenPlus, TokenNOT, TokenMinus, TokenDoubleQuote,
    TokenLeftParenthesis, TokenRightParenthesis,
    TokenLeftBracket, TokenRightBracket,
    TokenAnd, TokenOr, TokenOR, TokenVerticalBar,
    TokenColon, TokenEqual, TokenMultiply, TokenDivide
    };

class Token
    {
    public:
        virtual TokenType GetTokenType( ) const;
        virtual char *TokenString( ) const
           {
           return nullptr;
           }
    private:
        TokenType token;
    };

class NotBuiltin: Token
    {
    public:
        NotBuiltin(char *string);
        TokenType GetTokenType( ) const
           {
           return TokenNotBuiltin;
           }
        char *TokenString( ) const
           {
           return string;
           }
    private:
        char *string;
    };

class Builtin: Token
    {
    public:
        Builtin(TokenType token);
        TokenType GetTokenType( ) const
           {
           return token;
           }
    private:
        TokenType token;
    };

class TokenStream
    {
    public:
        // Initially, CurrentToken will be TokenInvalid.
        Token *CurrentToken( );
        Token *TakeToken( );
        bool Match( TokenType t );
        TokenStream( );
        TokenStream( char *filename );
        TokenStream( std::ifstream &is );
    private:
        std::ifstream input;
        String currentTokenString;
        Token *currentToken;
    };

