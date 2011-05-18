%{
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#define YYSTYPE char*
#include "do-syntax.h"
static int read_string(char open, char close);
%}

%option 8bit warn yylineno 

%%

\/\/.*\n           {}
[\r\n\ \t\f]       {}
\{                 return LBRACE;
\}                 return RBRACE; 
\[\]               return BRACKETS;
","                return COMMA;
=                  return ASSIGNMENT;
==                 return EQUALITY;
!=                 return NEQ;
"`"                return read_string('`','´');
class              return CLASS;
data               return DATA;
message            return MESSAGE;
meta               return META;
\[read\]           return READ;
\[write\]          return WRITE;
const              return CONST;
virtual            return VIRTUAL;
operator           return OPERATOR;
[_/a-zA-Z][\._/a-zA-Z0-9&]*  { 
                             dolval = strdup(yytext);
                             return NAME;
                             }
[ +-][0-9]+                  { 
                             dolval = strdup(yytext);
                             return NUMBER;
                             }
#.*                          { 
                             dolval = strdup(yytext);
                             return PRECOMPILER;
                             }
\(                return LPAREN;
\)                return RPAREN;
\*                return STAR;
\;                return SEMICOLON;
:                 return COLON;
\<                return TEMPLATE_OPEN;
\>                return TEMPLATE_CLOSE;
~                 return TILDE;
%%

void doerror(const char* text)
{
fprintf(stderr,"doc: parse error at line %d\n",yylineno);
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
      dolval=strdup(result);
      // printf("Just returned `%s´\n",result);
      return NAME;
      }
    result[i++]=t;
    assert(i<5000);
    if (t==EOF) doerror("eof in external block");
    }
  unput(t);
}

int yywrap()
  {
  return 1;
  }   

