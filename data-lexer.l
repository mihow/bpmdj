%{
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include "data.h"
#define YYSTYPE Data
#include "data-syntax.h"
#include "symbol.h"
#include "string.h"
#include "numbers.h"

/**
 * When we define print_tokens, we print the one we are reading
 */



#ifdef PRINT_TOKENS
#define PRINT_TOKEN(T) printf(T"%s\n",yytext);
#else
#define PRINT_TOKEN(T)
#endif

static void read_string();
extern void yyerror(const char* text);
%}

%%

[\r\n(\r\n)\ \t\f] {}

"{"                { return BRACE_OPEN; }
"}"                { return BRACE_CLOSE; }
":"                { return COLON; }
"["                { return BRACKET_OPEN; }
"]"                { return BRACKET_CLOSE; }
"<"                { return DIMENSION_OPEN; }
">"                { return DIMENSION_CLOSE; }
"'"                { read_string(); return STRING; }

[_/a-zA-Z][\._/a-zA-Z0-9]*  { 
                            yylval = Symbol(yytext); 
                            PRINT_TOKEN("Symbol");
                            return SYMBOL;
                            }

[0-9]+g                     {
                            yylval = Float8(yytext); 
                            PRINT_TOKEN("Token f8"); 
                            return NUMBER;
                            }
[0-9]+\.[0-9]+g             {
                            yylval = Float8(yytext); 
                            PRINT_TOKEN("Token f8"); 
                            return NUMBER;
                            }
[+-][0-9]+g                 {
                            yylval = Float8(yytext); 
                            PRINT_TOKEN("Token f8"); 
                            return NUMBER;
                            }
[+-][0-9]+\.[0-9]+g         {
                            yylval = Float8(yytext); 
                            PRINT_TOKEN("Token f8"); 
                            return NUMBER;
                            }

[0-9]+f                     {
                            yylval = Float4(yytext); 
                            PRINT_TOKEN("Token f4"); 
                            return NUMBER;
                            }
[0-9]+\.[0-9]+f             {
                            yylval = Float4(yytext); 
                            PRINT_TOKEN("Token f4"); 
                            return NUMBER;
                            }
[+-][0-9]+f                 {
                            yylval = Float4(yytext); 
                            PRINT_TOKEN("Token f4"); 
                            return NUMBER;
                            }
[+-][0-9]+\.[0-9]+f         {
                            yylval = Float4(yytext); 
                            PRINT_TOKEN("Token f4"); 
                            return NUMBER;
                            }

[0-9]+      {
            yylval = Unsigned4(yytext); 
            PRINT_TOKEN("Token u4"); 
            return NUMBER;
            }
[0-9]+i     {
            yylval = Unsigned4(yytext); 
            PRINT_TOKEN("Token u4"); 
            return NUMBER;
            }
[+-][0-9]+  {
            yylval = Signed4(yytext); 
            PRINT_TOKEN("Token s4");
            return NUMBER;
            }
[+-][0-9]+i {
            yylval = Signed4(yytext); 
            PRINT_TOKEN("Token s4"); 
            return NUMBER;
            }
[0-9]+w     {
            yylval = Unsigned2(yytext);
            return NUMBER;
            }
[+-][0-9]+w {
            yylval = Signed2(yytext);
            return NUMBER;
            }
[0-9]+b     {
            yylval = Unsigned1(yytext);
            return NUMBER;
            }
[+-][0-9]+b {
            yylval = Signed1(yytext);
            return NUMBER;
            }
[0-9]+l     {
            yylval = Unsigned8(yytext);
            return NUMBER;
            }
[+-][0-9]+l {
            yylval = Signed8(yytext); 
            return NUMBER;
            }

%%

static void read_string()
  {
  char result[5000];
  int i = 0;
  char t;
  for(;;)
    {
    while ((t=yyinput())!='\'')
      {
      result[i++]=t;
      assert(i<5000);
      //if (t=='\n') linenumber++;
      if (t==EOF) yyerror("eof in string");
      }
    // if the next immediate character is a ' then we 
    // simply continue the string
    t=yyinput();
    if (t!='\'') 
      {
      yyunput(t,yytext);
      result[i++]=0;
      yylval=String::parse(result);
      return;
      }
    result[i++]='\'';
    result[i++]='\'';
    }
}

int yywrap()
  {
  return 1;
  }
