%{
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#define YYSTYPE char*
#include "ao-syntax.h"
static int read_string(char open, char close);
%}

%option 8bit warn yylineno 

%%

\/\/.*\n             {}
[\r\n\ \t\f]       {}
\{                 return LBRACE;
\}                 return RBRACE; 
","                return COMMA;
=                  return INIT;
"`"                return read_string('`','´');
class              return CLASS;
active             return ACTIVE;
message            return MESSAGE;
meta               return META;
volatile           return VOLATILE;
uses               return USES;
[_/a-zA-Z][\._/a-zA-Z0-9]*  { 
                            aolval = strdup(yytext);
                            return NAME;
                            }
[ +-][0-9]+                  { 
                            aolval = strdup(yytext);
                            return NUMBER;
                            }
#.*                         { 
                            aolval = strdup(yytext);
                            return PRECOMPILER;
                            }
\(                return LPAREN;
\)                return RPAREN;
\*                return STAR;
\;                return SEMICOLON;
\<                return TEMPLATE_OPEN;
\>                return TEMPLATE_CLOSE;
%%

void aoerror(const char*)
{
fprintf(stderr,"aoc: parse error at line %d\n",yylineno);
exit(1);
}

static int read_string(char openchar, char closechar)
  {
  char result[5000];
  int i = 0;
  char t;
  int open_count = 1;
  while(true)
    {
    t=yyinput();
    if (t==openchar) open_count++;
    if (t==closechar) open_count--;
    if (open_count==0) 
      {
      result[i++]=0;
      aolval=strdup(result);
      // printf("Just returned `%s´\n",result);
      return NAME;
      }
    result[i++]=t;
    assert(i<5000);
    if (t==EOF) aoerror("eof in external block");
    }
  // just an yyunput to get rid of the warning
  unput(t);
}

int yywrap()
  {
  return 1;
  }

