%{
#define YYDEBUG 1
#include "data.h"
#include "array-creator.h"
#include "data-token.h"
#include "symbol.h"
#define YYSTYPE Data
%}

%start main

%token BRACE_OPEN BRACE_CLOSE COLON BRACKET_OPEN BRACKET_CLOSE
       SYMBOL NUMBER STRING DIMENSION_OPEN DIMENSION_CLOSE

%{

#define y_tabbed
#include <stdlib.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
extern int yylex();
Data parse_result;
void yyerror(const char* text)
  {
  printf("%s\n",text);
  exit(10);
  }

%}

%%
main: data {$$=$1; parse_result = $1;}

data: token  {$$=$1;}
    | SYMBOL {$$=$1;}
    | NUMBER {$$=$1;}
    | STRING {$$=$1;}
    | array  {$$=$1;}
    | sequence {$$=$1;}

token : BRACE_OPEN associations BRACE_CLOSE
        {$$=$2;}

associations : associations field data
                {Token t($1);
		Symbol s($2);
		t[s]=$3; 
		$$=$1;}
	     | {$$=Token();}

field : SYMBOL COLON
        {$$=$1;}

array : DIMENSION_OPEN NUMBER COLON SYMBOL DIMENSION_CLOSE sequence
        { $$=Creator($6).convertTo($2,$4); }

sequence : BRACKET_OPEN elements BRACKET_CLOSE
        { $$=$2; }

elements : elements data
      { Creator t($1);
        t.append($2);
        $$=t;  }
    | {$$=Creator();}

%%
