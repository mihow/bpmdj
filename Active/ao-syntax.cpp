
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse         aoparse
#define yylex           aolex
#define yyerror         aoerror
#define yylval          aolval
#define yychar          aochar
#define yydebug         aodebug
#define yynerrs         aonerrs


/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "ao-syntax.y"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <vector>
#include "ao-lexer.h"
#include "sao-printer.h"
#include "ao-grammar.h"

/**
 * Since some people want prefixes such as 'T' in the generated pascal code
 * it is possible to declare them here. By default, the prefix is omitted
 * since I believe it is a stupid convention; Active objects are no longer objects
 * and even the 'objects' are already pointers, so they should basically be called
 * pointers, but then it becomes even more confusing. However, T as a prefix
 * is also not completely correct, semantically speaking. Maybe the prefix A would
 * make more sense, or AO. One could also argue, that if your active obejcts needs a 
 * prefix, then you should call it like that.
 */
#define T ""

void aoerror(const char* text);
char* cat(const char* a, const char* b)
{
  if (a==NULL && b==NULL) return strdup("");
  if (a==NULL) return strdup(b);
  if (b==NULL) return strdup(a);
  char* c = (char*)malloc(strlen(a)+strlen(b)+1);
  strcpy(c,a);
  strcat(c,b);
  return c;
}
 
extern bool pascal; 
 
char* args2str(vector<s_arg> * args, string sepi = ", ", string prefix= "")
{
  if (!pascal)
    {
      char* sep=strdup(sepi.c_str());
      char* accu=strdup("");
      for(unsigned i = 0 ; i < args->size() ; i++)
	{
	  accu=cat(accu,args[0][i].type);
	  accu=cat(accu," ");
	  accu=cat(accu,args[0][i].name);
	  if (i<args->size()-1)
	    accu=cat(accu,sep);
	}
      return accu;
    }
  else
    {
      char* sep=strdup(sepi.c_str());
      char* accu=strdup("");
      char* prefix_=strdup(prefix.c_str());
      for(unsigned i = 0 ; i < args->size() ; i++)
	{
	  accu=cat(accu,prefix_);
	  accu=cat(accu,args[0][i].name);
	  accu=cat(accu,": ");
	  accu=cat(accu,args[0][i].type);
	  if (i<args->size()-1)
	    accu=cat(accu,sep);
	}
      return accu;
    }
}
 
char* argnames2str(vector<s_arg> * args,string prefix="")
{
  char* pref=strdup(prefix.c_str());
  char* accu=strdup("");
  for(unsigned i = 0 ; i < args->size() ; i++)
    {
      accu=cat(accu,pref);
      accu=cat(accu,args[0][i].name);
      if (i<args->size()-1)
	accu=cat(accu,", ");
    }
  return accu;
}
 
char* copyargs(vector<s_arg> * args)
{
  if (!pascal)
    {
      char* accu=strdup("");
      for(unsigned i = 0 ; i < args->size() ; i++)
	{
	  char temp[1000];
	  sprintf(temp,"%s(%s)",args[0][i].name,args[0][i].name);
	  accu=cat(accu,temp);
	  if (i<args->size()-1)
	    accu=cat(accu,", ");
	}
      return accu;
    }
  else
    {
      char* accu=strdup("");
      for(unsigned i = 0 ; i < args->size() ; i++)
	{
	  char temp[1000];
	  sprintf(temp,"  %s:=bloody_local_var_%s;\n",args[0][i].name,args[0][i].name);
	  accu=cat(accu,temp);
	  //if (i<args->size()-1)
	  //	    accu=cat(accu,", ");
	}
      return accu;
    }
}

extern outputbuf object;  // the object class declaration
extern outputbuf object2; // the object methods
extern outputbuf meta;
extern outputbuf msgs; 
extern outputbuf msgs1; 
extern outputbuf impl; 
extern outputbuf useslist;
extern outputbuf globhead;
extern char * current_ao;
extern map<string,string> field2init;
 
#define YYSTTYPE char*


/* Line 189 of yacc.c  */
#line 210 "ao-syntax.cpp"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


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
     INIT = 265,
     ACTIVE = 266,
     COMMA = 267,
     LBRACE = 268,
     RBRACE = 269,
     STAR = 270,
     PRECOMPILER = 271,
     VOLATILE = 272,
     CLASS = 273,
     USES = 274,
     TEMPLATE_OPEN = 275,
     TEMPLATE_CLOSE = 276
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 135 "ao-syntax.y"
 
  char           * token;
  s_arg            arg;
  vector<s_arg>  * args;



/* Line 214 of yacc.c  */
#line 275 "ao-syntax.cpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 287 "ao-syntax.cpp"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   61

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  22
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  12
/* YYNRULES -- Number of rules.  */
#define YYNRULES  29
/* YYNRULES -- Number of states.  */
#define YYNSTATES  63

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   276

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     6,     7,     9,    13,    17,    18,    26,
      29,    30,    34,    40,    45,    52,    57,    63,    68,    71,
      73,    78,    85,    87,    91,    94,    98,   100,   103,   105
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      23,     0,    -1,    23,    24,    -1,    -1,    16,    -1,    19,
       4,     8,    -1,    18,     4,     8,    -1,    -1,    11,     4,
      13,    25,    26,    14,     8,    -1,    26,    27,    -1,    -1,
      28,     4,     8,    -1,    28,     4,    10,    33,     8,    -1,
      29,    28,     4,     8,    -1,    29,    28,     4,    10,    33,
       8,    -1,    28,     4,    30,     8,    -1,     3,    28,     4,
      30,     8,    -1,     7,     4,    30,     8,    -1,    28,    15,
      -1,     4,    -1,     4,    20,    28,    21,    -1,     4,    20,
      28,    12,    28,    21,    -1,    17,    -1,     5,    31,     6,
      -1,     5,     6,    -1,    31,    12,    32,    -1,    32,    -1,
      28,     4,    -1,     9,    -1,     4,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   147,   147,   148,   151,   152,   153,   155,   154,   341,
     342,   345,   352,   360,   362,   364,   365,   371,   454,   455,
     456,   457,   460,   463,   464,   467,   468,   471,   474,   475
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "META", "NAME", "LPAREN", "RPAREN",
  "MESSAGE", "SEMICOLON", "NUMBER", "INIT", "ACTIVE", "COMMA", "LBRACE",
  "RBRACE", "STAR", "PRECOMPILER", "VOLATILE", "CLASS", "USES",
  "TEMPLATE_OPEN", "TEMPLATE_CLOSE", "$accept", "aoheaders", "aoheader",
  "$@1", "contents", "content", "type", "varflags", "args", "args_lst",
  "arg", "init_expression", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    22,    23,    23,    24,    24,    24,    25,    24,    26,
      26,    27,    27,    27,    27,    27,    27,    27,    28,    28,
      28,    28,    29,    30,    30,    31,    31,    32,    33,    33
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     0,     1,     3,     3,     0,     7,     2,
       0,     3,     5,     4,     6,     4,     5,     4,     2,     1,
       4,     6,     1,     3,     2,     3,     1,     2,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     1,     0,     4,     0,     0,     2,     0,     0,
       0,     7,     6,     5,    10,     0,     0,    19,     0,     0,
      22,     9,     0,     0,     0,     0,     0,     8,     0,    18,
       0,     0,     0,     0,     0,    11,     0,     0,     0,     0,
       0,    20,    24,     0,     0,    26,    17,    29,    28,     0,
      15,    13,     0,    16,     0,    27,    23,     0,    12,     0,
      21,    25,    14
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     1,     7,    14,    15,    21,    43,    23,    34,    44,
      45,    49
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -23
static const yytype_int8 yypact[] =
{
     -23,     2,   -23,    15,   -23,    26,    27,   -23,    29,    36,
      38,   -23,   -23,   -23,   -23,     0,    43,    30,    44,    41,
     -23,   -23,     1,    43,     7,    43,    46,   -23,    31,   -23,
       8,    46,    12,    34,    45,   -23,    28,    47,    35,    48,
      43,   -23,   -23,    11,    22,   -23,   -23,   -23,   -23,    49,
     -23,   -23,    28,   -23,    14,   -23,   -23,    43,   -23,    50,
     -23,   -23,   -23
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -23,   -23,   -23,   -23,   -23,   -23,   -15,   -23,   -22,   -23,
      -5,     9
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      22,    24,     2,    16,    17,    28,    37,    18,    30,    39,
      32,    31,    38,     3,    19,    55,    29,    20,     4,     8,
       5,     6,    29,    29,    40,    54,    29,    29,    56,    29,
       9,    10,    47,    41,    57,    60,    33,    48,    17,    35,
      42,    36,    11,    51,    12,    52,    13,    17,    26,    27,
      25,    33,    61,    46,     0,    50,    53,    58,    62,     0,
       0,    59
};

static const yytype_int8 yycheck[] =
{
      15,    16,     0,     3,     4,     4,    28,     7,    23,    31,
      25,     4,     4,    11,    14,     4,    15,    17,    16,     4,
      18,    19,    15,    15,    12,    40,    15,    15,     6,    15,
       4,     4,     4,    21,    12,    21,     5,     9,     4,     8,
       6,    10,    13,     8,     8,    10,     8,     4,     4,     8,
      20,     5,    57,     8,    -1,     8,     8,     8,     8,    -1,
      -1,    52
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    23,     0,    11,    16,    18,    19,    24,     4,     4,
       4,    13,     8,     8,    25,    26,     3,     4,     7,    14,
      17,    27,    28,    29,    28,    20,     4,     8,     4,    15,
      28,     4,    28,     5,    30,     8,    10,    30,     4,    30,
      12,    21,     6,    28,    31,    32,     8,     4,     9,    33,
       8,     8,    10,     8,    28,     4,     6,    12,     8,    33,
      21,    32,     8
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 4:

/* Line 1455 of yacc.c  */
#line 151 "ao-syntax.y"
    { print(globhead,"%s\n",(yyvsp[(1) - (1)].token)); ;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 152 "ao-syntax.y"
    {assert(pascal); print(useslist,", %s",(yyvsp[(2) - (3)].token));;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 153 "ao-syntax.y"
    { print(globhead,"class %s;\n",(yyvsp[(2) - (3)].token)); ;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 155 "ao-syntax.y"
    {
      field2init.clear();
      if (!pascal)
	{
	  print(object,"class Active%s: public ActiveObject< Active%s_msg_* >\n{\n",(yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token));
	  print(object,"  friend class %s;\n",(yyvsp[(2) - (3)].token));
	  print(object,"  %s * self;\n",(yyvsp[(2) - (3)].token));
	  print(object,
		"    virtual elementResult handle( Active%s_msg_* cmd)\n"
		"      {\n"
		"        if (cmd) return cmd->run(this);\n"
		"        else return Done;\n"
		"      };\n",(yyvsp[(2) - (3)].token));
	  print(globhead, "class %s;\n",(yyvsp[(2) - (3)].token));
	  print(globhead, "class Active%s;\n",(yyvsp[(2) - (3)].token));
	  print(globhead, "#ifdef TRACE_MESSAGES\n"
		"#define ENTER_MSG cerr << \"Start \" << declaration() << \"\\n\";\n"
		"#define LEAVE_MSG cerr << \"Stop \" << declaration()  << \"\\n\";\n"
		"#endif\n");
	  print(globhead, "#ifndef ENTER_MSG\n"
		"#define ENTER_MSG ;\n"
		"#endif\n"
		"#ifndef LEAVE_MSG\n"
		"#define LEAVE_MSG ;\n"
		"#endif\n");
	}
      else
	{
	  // pascal declarations
	  print(globhead,"Type "T"Active%s=Class;\n",(yyvsp[(2) - (3)].token));
	  print(globhead,"Type "T"%s=Class;\n",(yyvsp[(2) - (3)].token));
	  print(globhead,"Type "T"Active%s_msg_=Class;\n",(yyvsp[(2) - (3)].token));
	  print(globhead,"Type "T"ActiveObject_"T"Active%s_msg_=specialize ActiveObject<"T"Active%s_msg_>;\n",(yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token));
	  
	  // pascal implementation
	  print(object,"Type "T"Active%s=Class("T"ActiveObject_"T"Active%s_msg_)\n",(yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token));
	  print(object,"  public meta: "T"%s;\n",(yyvsp[(2) - (3)].token));
	  print(object,"  public function handle(cmd: "T"Active%s_msg_): elementResult; override;\n",(yyvsp[(2) - (3)].token));
	  print(impl,"function "T"Active%s.handle(cmd: "T"Active%s_msg_): elementResult;\n  begin\n",(yyvsp[(2) - (3)].token));
	  print(impl,"  if (assigned(cmd)) then Result:=cmd.run(self)\n"
		"  else Result:=Done;\n"
		"  end;\n\n");
      	}
      if (!pascal)
	{
	  print(msgs1,
		"/**\n"
		" * Represents the basic message that is used to queue or deliver a call to \n"
		" * the true Active%s. Active%s_msg_ has a run method which is invoked\n"
		" * by ActiveObject whenever it wants to handle the message. For each \n"
		" * declared method in the active object description, a specific subclass\n"
		" * of Active%s_msg_ has been generated. See inheritance diagram.\n"
		" * The message classes are automatically instantiated by the active object\n"
		" * stub %s\n"
		" */\n",(yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token));
	  
	  print(msgs1,  "class Active%s_msg_\n",(yyvsp[(2) - (3)].token));
	  print(msgs1,  "{\n"
		"  public:\n"
		"    /**\n"
		"     * Called by ActiveObject to handle this queued message.\n"
		"     * %%arg caller is the Active%s itself.\n"
		"     */\n"
		"    virtual elementResult run(Active%s * /* caller */)\n"
		"    {\n"
		"      assert(0);\n"
		"      return Revisit;\n"
		"    }\n",(yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token));
	  print(msgs1, 
		"    /**\n"
		"     * Returns the name of this message. Since this is the message baseclass\n"
		"     * it has no identity and will return 'Unknown Message' \n"
		"     */\n"
		"    virtual string declaration()\n"
		"    {\n"
		"      return \"Unknown message\";\n"
		"    }\n"
		"    /**\n"
		"     * The following is necessary so that child desctructors are invoked\n"
		"     * when a message is deleted in the handle_message loop.\n"
		"     */\n"
		"    virtual ~Active%s_msg_()\n"
		"    {\n"
		"    }\n"
		"};\n\n",(yyvsp[(2) - (3)].token));
	  print(meta, 
		"/**\n"
		" * Represents the stub that will transform each incoming call (method)\n"
		" * to an object subclassed from type Active%s_msg_\n"
		" * The stub itself has an instance of the true active object.\n"
		" */\n",(yyvsp[(2) - (3)].token));
	  print(meta, "class %s\n{\n",(yyvsp[(2) - (3)].token));
	  print(meta, "  private:\n"
		"    /**\n"
		"     * The object that is covered for by this stub. The fact that the\n"
		"     * stub allocates the object ensures that only one of it exists and that any\n"
		"     * interaction must go through the stub. As such it is kept private.\n"
		"     */\n"
		"    Active%s object;\n",(yyvsp[(2) - (3)].token));
	  print(meta,"  public:\n"
		"    /**\n"
		"     * The constructor of the stub will also directly initalize the main\n"
		"     * object. Because object construction and delayed calls interfere\n"
		"     * somewhat we prohibit the actual implementation (and especially use)\n"
		"     * of a specialized Active Object constructor. Instead, simply the name\n"
		"     * is passed to the object. If you need to initialize the stub, you\n"
		"     * should consider adding an init message to the active object and\n"
		"     * calling (well sending a message to) it directly when the stub is\n"
		"     * generated.\n"
		"     */\n");
	  print(meta, "    %s(string name=\"%s\"): object(this, name) {};\n",(yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token));
	  current_ao = strdup((yyvsp[(2) - (3)].token));
	}
      else
	{
	  // pascal
	  print(msgs1,
		"(*\n"
		" * Represents the basic message that is used to queue or deliver a call to \n"
		" * the true Active%s. Active%s_msg_ has a run method which is invoked\n"
		" * by ActiveObject whenever it wants to handle the message. For each \n"
		" * declared method in the active object description, a specific subclass\n"
		" * of Active%s_msg_ has been generated. See inheritance diagram.\n"
		" * The message classes are automatically instantiated by the active object\n"
		" * stub %s\n"
		" *)\n",(yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token));
	  
	  print(msgs1,"Type "T"Active%s_msg_=class\n",(yyvsp[(2) - (3)].token));
	  print(msgs1,"  public function run(caller: "T"Active%s): elementResult; virtual; abstract;\n",(yyvsp[(2) - (3)].token));
	  print(msgs1,"  end;\n\n");
	  print(meta,"Type "T"%s=class\n",(yyvsp[(2) - (3)].token));
	  print(meta, "  private\n"
		"    _object: "T"Active%s;\n",(yyvsp[(2) - (3)].token));
	  print(meta,"  public\n");
	  print(meta, "    Constructor Create(bloody_local_var_name: string=\'%s\');\n",(yyvsp[(2) - (3)].token));
	  
	  print(impl,"Constructor "T"%s.Create(bloody_local_var_name: string);\nbegin\n  _object:="T"Active%s.Create(self,bloody_local_var_name);\nend;\n\n",
		(yyvsp[(2) - (3)].token),(yyvsp[(2) - (3)].token));
	  current_ao = strdup((yyvsp[(2) - (3)].token));
	}
    ;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 297 "ao-syntax.y"
    {
      if (!pascal)
	{
	  print(object,
		"  protected:\n    Active%s(%s* s, string name):\n"
		"      ActiveObject< Active%s_msg_ * >(name), self(s)\n",(yyvsp[(2) - (7)].token),(yyvsp[(2) - (7)].token),(yyvsp[(2) - (7)].token));
	  print(object, "      {\n");
	}
      else
	{
	  print(object,"  public constructor Create(s: "T"%s; bloody_local_name: string);\n",(yyvsp[(2) - (7)].token));
	  print(impl,"constructor "T"Active%s.Create(s: "T"%s; bloody_local_name: string);\n"
		"  begin\n"
		"  inherited Create(bloody_local_name);\n"
		"  meta:=s;\n",(yyvsp[(2) - (7)].token),(yyvsp[(2) - (7)].token));
	}
      map<string,string>::iterator it=field2init.begin();
      while(it!=field2init.end())
	{
	  if (it->second!="")
	    {
	      if (pascal)
		print(impl,"      %s := %s;\n",it->first.c_str(), it->second.c_str());
	      else
		print(object,"      %s = %s;\n",it->first.c_str(), it->second.c_str());
	    }
	  it++;
	}
      field2init.clear();
      if (!pascal)
	{
	  print(object, "      };\n");
	  print(object,"};\n\n");
	  print(meta,"};\n\n");
	}
      else
	{
	  print(object,"  end;\n\n");
	  print(impl,"  end;\n\n");
	  print(meta,"end;\n\n");
	}
    ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 342 "ao-syntax.y"
    {;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 346 "ao-syntax.y"
    {if (!pascal)
      print(object,"  %s %s;\n",(yyvsp[(1) - (3)].token),(yyvsp[(2) - (3)].token)); 
    else
      print(object,"  public %s: %s;\n",(yyvsp[(2) - (3)].token),(yyvsp[(1) - (3)].token));
    field2init[(yyvsp[(2) - (3)].token)]=""; 
  ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 353 "ao-syntax.y"
    {
    if (!pascal)
      print(object,"  %s %s;\n",(yyvsp[(1) - (5)].token),(yyvsp[(2) - (5)].token));
    else
      print(object,"  public %s: %s;\n",(yyvsp[(2) - (5)].token),(yyvsp[(1) - (5)].token)); 
    field2init[(yyvsp[(2) - (5)].token)]=(yyvsp[(4) - (5)].token); 
  ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 361 "ao-syntax.y"
    {print(object,"  %s %s %s;\n",(yyvsp[(1) - (4)].token),(yyvsp[(2) - (4)].token),(yyvsp[(3) - (4)].token)); field2init[(yyvsp[(3) - (4)].token)]=""; ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 363 "ao-syntax.y"
    {print(object,"  %s %s %s;\n",(yyvsp[(1) - (6)].token),(yyvsp[(2) - (6)].token),(yyvsp[(3) - (6)].token)); field2init[(yyvsp[(3) - (6)].token)]=(yyvsp[(5) - (6)].token); ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 364 "ao-syntax.y"
    {print(object,"  %s %s(%s);\n",(yyvsp[(1) - (4)].token),(yyvsp[(2) - (4)].token),args2str((yyvsp[(3) - (4)].args)));;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 366 "ao-syntax.y"
    {
      print(meta,
	    "  public:\n"
	    "    %s %s(%s);\n",(yyvsp[(2) - (5)].token),(yyvsp[(3) - (5)].token),args2str((yyvsp[(4) - (5)].args)));
    ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 372 "ao-syntax.y"
    {
      // the object code
      if (!pascal)
	{
	  print(object,"  public: elementResult %s(%s);\n",(yyvsp[(2) - (4)].token),args2str((yyvsp[(3) - (4)].args)));
	  print(object,"  protected: void queue_%s(%s);\n",(yyvsp[(2) - (4)].token),args2str((yyvsp[(3) - (4)].args)));
	}
      else
	{
	  print(object,"  public function %s(%s): elementResult;\n",(yyvsp[(2) - (4)].token),args2str((yyvsp[(3) - (4)].args),"; ",""));
	  print(object,"  public procedure queue_%s(%s);\n",(yyvsp[(2) - (4)].token),args2str((yyvsp[(3) - (4)].args),"; ", "bloody_local_var_"));
	}
      
      // the message class
      if (!pascal)
	{
	  print(msgs,"class Active%s_msg_%s: public Active%s_msg_\n",current_ao,(yyvsp[(2) - (4)].token),current_ao);
	  print(msgs,"{\n    %s;\n",args2str((yyvsp[(3) - (4)].args),";\n    "));
	  print(msgs,"  public:\n    Active%s_msg_%s(%s)",current_ao,(yyvsp[(2) - (4)].token),args2str((yyvsp[(3) - (4)].args)));
	  if ((yyvsp[(3) - (4)].args)->size())
	    print(msgs," : %s\n      {\n      };\n",copyargs((yyvsp[(3) - (4)].args)));
	  else
	    print(msgs,"\n      {\n      };\n");
	  print(msgs, "    virtual string declaration()\n"
		"    {\n"
		"      return \"%s::%s(%s)\";\n"
		"    }\n",current_ao,(yyvsp[(2) - (4)].token),args2str((yyvsp[(3) - (4)].args))
		);
	  print(msgs,"    virtual elementResult run(Active%s * ao)\n",current_ao);
	  print(msgs,"      {\n        ENTER_MSG;\n");
	  print(msgs,"        elementResult res = ao->%s(%s);\n",(yyvsp[(2) - (4)].token),argnames2str((yyvsp[(3) - (4)].args)));
	  print(msgs,"        LEAVE_MSG;\n",(yyvsp[(2) - (4)].token),argnames2str((yyvsp[(3) - (4)].args)));
	  print(msgs,"        return res;\n      };\n",(yyvsp[(2) - (4)].token),argnames2str((yyvsp[(3) - (4)].args)));
	  print(msgs,"};\n\n");
	}
      else
	{
	  print(globhead,"Type "T"Active%s_msg_%s=Class;\n",current_ao,(yyvsp[(2) - (4)].token));
	  print(msgs,"Type "T"Active%s_msg_%s=Class("T"Active%s_msg_)\n",current_ao,(yyvsp[(2) - (4)].token),current_ao);
	  if ((yyvsp[(3) - (4)].args)->size())
	    print(msgs,"  public %s;\n",args2str((yyvsp[(3) - (4)].args),";\n  "));
	  print(msgs,"  constructor Create(%s);\n",args2str((yyvsp[(3) - (4)].args),"; ","bloody_local_var_"));
	  print(impl,"constructor "T"Active%s_msg_%s.Create(%s);\n  begin\n",current_ao,(yyvsp[(2) - (4)].token),args2str((yyvsp[(3) - (4)].args),"; ","bloody_local_var_"));
	  
	  if ((yyvsp[(3) - (4)].args)->size())
	    print(impl,"%s  end;\n\n",copyargs((yyvsp[(3) - (4)].args)));
	  else
	    print(impl,"  end;\n\n");

	  print(msgs,"  public function run(ao: "T"Active%s): elementResult; override;\n",current_ao);
	  print(impl,"function "T"Active%s_msg_%s.run(ao: "T"Active%s): elementResult;\n  begin\n",current_ao, (yyvsp[(2) - (4)].token), current_ao);
	  print(impl,"  Result:=ao.%s(%s);\n  end;\n\n",(yyvsp[(2) - (4)].token),argnames2str((yyvsp[(3) - (4)].args)));
	  print(msgs,"end;\n\n");
	}
      // the meta call
      if (!pascal)
	{
	  print(object2,"inline void Active%s::queue_%s(%s)\n",current_ao,(yyvsp[(2) - (4)].token),args2str((yyvsp[(3) - (4)].args)));
	  print(object2,"  {\n");
	  print(object2,"    push(new Active%s_msg_%s(%s));\n  };\n",
		current_ao,(yyvsp[(2) - (4)].token),argnames2str((yyvsp[(3) - (4)].args)));
	}
      else
	{
	  print(impl,"procedure "T"Active%s.queue_%s(%s);\n  begin\n",current_ao,(yyvsp[(2) - (4)].token),args2str((yyvsp[(3) - (4)].args),"; ","bloody_local_var_"));
	  print(impl,"  push("T"Active%s_msg_%s.Create(%s));\n  end;\n\n",
		current_ao,(yyvsp[(2) - (4)].token),argnames2str((yyvsp[(3) - (4)].args),"bloody_local_var_"));
	}
      if (!pascal)
	{
	  print(meta,  "  public:\n    void %s(%s)\n",(yyvsp[(2) - (4)].token),args2str((yyvsp[(3) - (4)].args),", "));
	  print(meta,  "    {\n      object.queue_%s(%s);\n    };\n",(yyvsp[(2) - (4)].token),argnames2str((yyvsp[(3) - (4)].args)));
	}
      else
	{
	  print(meta,  "  public procedure %s(%s);\n",(yyvsp[(2) - (4)].token),args2str((yyvsp[(3) - (4)].args),"; ","bloody_local_var_"));
	  print(impl,  "procedure "T"%s.%s(%s);\n",current_ao,(yyvsp[(2) - (4)].token),args2str((yyvsp[(3) - (4)].args),"; ","bloody_local_var_"));
	  print(impl,  "  begin\n    _object.queue_%s(%s);\n  end;\n\n",(yyvsp[(2) - (4)].token),argnames2str((yyvsp[(3) - (4)].args),"bloody_local_var_"));
	}
    ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 454 "ao-syntax.y"
    {(yyval.token)=cat((yyvsp[(1) - (2)].token),"*");;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 456 "ao-syntax.y"
    {(yyval.token)=cat(cat((yyvsp[(1) - (4)].token)," < "),cat((yyvsp[(3) - (4)].token)," > ")); ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 457 "ao-syntax.y"
    {(yyval.token)=  cat(cat(cat((yyvsp[(1) - (6)].token), "<"), cat((yyvsp[(3) - (6)].token),",")), cat((yyvsp[(5) - (6)].token),">"));;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 460 "ao-syntax.y"
    {(yyval.token)=strdup("volatile");;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 463 "ao-syntax.y"
    {(yyval.args)=(yyvsp[(2) - (3)].args);;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 464 "ao-syntax.y"
    { (yyval.args)=new vector<s_arg>();;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 467 "ao-syntax.y"
    {(yyval.args)=(yyvsp[(1) - (3)].args); (yyval.args)->push_back((yyvsp[(3) - (3)].arg));;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 468 "ao-syntax.y"
    {(yyval.args)=new vector<s_arg>(); (yyval.args)->push_back((yyvsp[(1) - (1)].arg));;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 471 "ao-syntax.y"
    {(yyval.arg).name = (yyvsp[(2) - (2)].token); (yyval.arg).type=(yyvsp[(1) - (2)].token);;}
    break;



/* Line 1455 of yacc.c  */
#line 1951 "ao-syntax.cpp"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 476 "ao-syntax.y"


