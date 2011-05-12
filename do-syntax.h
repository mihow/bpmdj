/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     META = 258,
     NAME = 259,
     LPAREN = 260,
     RPAREN = 261,
     MESSAGE = 262,
     SEMICOLON = 263,
     NUMBER = 264,
     DATA = 265,
     COMMA = 266,
     LBRACE = 267,
     RBRACE = 268,
     STAR = 269,
     PRECOMPILER = 270,
     READ = 271,
     WRITE = 272,
     VOLATILE = 273,
     CLASS = 274,
     COLON = 275,
     VIRTUAL = 276,
     ASSIGNMENT = 277,
     OPERATOR = 278,
     TEMPLATE_OPEN = 279,
     TEMPLATE_CLOSE = 280,
     BRACKETS = 281,
     TILDE = 282,
     CONST = 283
   };
#endif
/* Tokens.  */
#define META 258
#define NAME 259
#define LPAREN 260
#define RPAREN 261
#define MESSAGE 262
#define SEMICOLON 263
#define NUMBER 264
#define DATA 265
#define COMMA 266
#define LBRACE 267
#define RBRACE 268
#define STAR 269
#define PRECOMPILER 270
#define READ 271
#define WRITE 272
#define VOLATILE 273
#define CLASS 274
#define COLON 275
#define VIRTUAL 276
#define ASSIGNMENT 277
#define OPERATOR 278
#define TEMPLATE_OPEN 279
#define TEMPLATE_CLOSE 280
#define BRACKETS 281
#define TILDE 282
#define CONST 283




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 82 "do-syntax.y"
{ 
  char           * token;
  s_arg            arg;
  vector<s_arg>  * args;
}
/* Line 1489 of yacc.c.  */
#line 111 "do-syntax.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE dolval;

