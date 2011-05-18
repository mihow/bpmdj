%{
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <vector>
#include "do-lexer.h"
#include "do-printer.h"
#include "do-grammar.h"
void doerror(const char* text);
char* cat(const char* a, const char* b)
   {
     char* c = (char*)malloc(strlen(a)+strlen(b)+1);
     strcpy(c,a);
     strcat(c,b);
     return c;
   }

 char* args2str(vector<s_arg> * args, char* sep = NULL)
{
  if (sep==NULL) sep=strdup(", ");
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

char* argnames2str(vector<s_arg> * args)
  {
    char* accu=strdup("");
    for(unsigned i = 0 ; i < args->size() ; i++)
      {
	accu=cat(accu,args[0][i].name);
	if (i<args->size()-1)
	  accu=cat(accu,", ");
      }
    return accu;
  }

extern bool access_checks;
void check_access_access(bool t)
{
  assert(t==access_checks);
}

extern outputbuf object;  // the object class declaration
extern outputbuf methods; // the object methods
extern outputbuf meta;
extern outputbuf globhead;
extern char * current_do;
extern char * current_pa;
extern char * makers_emitted_for;
 
#define YYSTTYPE char*
%}
%start doheaders
%token <token> META NAME LPAREN RPAREN MESSAGE SEMICOLON NUMBER;
%token <token> DATA COMMA LBRACE RBRACE STAR PRECOMPILER READ WRITE;
%token <token> VOLATILE CLASS COLON VIRTUAL ASSIGNMENT EQUALITY NEQ OPERATOR;
%token TEMPLATE_OPEN TEMPLATE_CLOSE BRACKETS TILDE CONST

%union
{ 
  char           * token;
  s_arg            arg;
  vector<s_arg>  * args;
}

%type <token> doheader contents content type  methodflags funcstf readwrite;
%type <args>  args args_lst
%type <arg>   arg;
%%

doheaders
  : doheaders doheader
  |

doheader
  : PRECOMPILER  { print(globhead,"%s\n",$1); }
  | CLASS NAME SEMICOLON { print(globhead,"class %s;\n",$2); }
  | DATA NAME LBRACE 
    {
      current_do = strdup($2);
      print(object,"class %sData: public ReferenceCount\n{\n",$2);
      print(object,"  friend class %s;\n",$2);
      print(object,"  public: virtual ~%sData();\n",current_do);
      
      print(object,"  public: virtual ReferenceCount* clone();\n");
      if (access_checks)
	{
	  print(object,"  public: virtual ReferenceCount* reallocate();\n");
	  print(object,"  public: bool smaller_than(%sData* other);\n",$2);
	  print(object,"  public: virtual bool most_specialized_smaller_than(void* other);\n",$2);
	}
      
      print(methods,"inline ReferenceCount* %sData::clone()\n",$2);
      print(methods,"  {\n  return new %sData(*this);\n  }\n\n",$2);
      if (access_checks)
	{
	  print(methods,"inline ReferenceCount* %sData::reallocate()\n",$2);
	  print(methods,"  {static Uniqueifier<%sData> u;\n",$2);
	  print(methods,"  return u.reallocate(this);}\n",$2);
	  print(methods,"inline bool %sData::most_specialized_smaller_than(void* other)\n",$2);
	  print(methods,"  {return smaller_than((%sData*)other);}\n",$2);
	}
      
      print(globhead, "class %s;\n",$2);
      print(globhead, "class %sData;\n",$2);

      print(meta,  "class %s: public Smart<%sData>\n{\n",$2,$2);
      print(meta,"  public: template <class SmartRefChild> %s(SmartRefChild& other)\n",current_do);
      print(meta,"    {\n");
      print(meta,"    other.incref();\n");
      print(meta,"    %sData* target = dynamic_cast<%sData*>(other.ptr);\n",current_do,current_do);
      print(meta,"    assert(target);\n");
      print(meta,"    ptr=target;\n");
      print(meta,"    }\n");

      print(meta,"  public: %s(%sData * other):\n",current_do, current_do);
      print(meta,"    Smart<%sData>(other)\n",current_do);
      print(meta,"    {\n");
      print(meta,"    }\n");
    } 
    contents RBRACE SEMICOLON
    {
      if (makers_emitted_for!=current_do)
	{
	  if (access_checks)
	    {
	      print(methods,"inline bool %sData::smaller_than(%sData* other)\n",current_do,current_do);
	      print(methods,"  { return false; };\n");
	    }
	}
      print(object,"};\n\n");
      print(meta,"};\n\n");
    }
  | DATA NAME COLON NAME LBRACE
    {
      current_do = strdup($2);
      current_pa = strdup($4);
      print(object,"class %sData: public %sData\n{\n",$2,$4);
      print(object,"  friend class %s;\n",$2);
      print(object,"  public: virtual ~%sData();\n",current_do);

      print(object,"  public: virtual ReferenceCount* clone();\n");
      if (access_checks)
	{
	  print(object,"  public: virtual ReferenceCount* reallocate();\n");
	  print(object,"  public: bool smaller_than(%sData* other);\n",$2);
	  print(object,"  public: virtual bool most_specialized_smaller_than(void* other);\n",$2);
	}

      print(methods,"inline ReferenceCount* %sData::clone()\n",$2);
      print(methods,"  {\n  return new %sData(*this);\n  }\n\n",$2);
      if (access_checks)
	{
	  print(methods,"inline ReferenceCount* %sData::reallocate()\n",$2);
	  print(methods,"  {static Uniqueifier<%sData> u;\n",$2);
	  print(methods,"  return u.reallocate(this);}\n",$2);
	  print(methods,"inline bool %sData::most_specialized_smaller_than(void* other)\n",$2);
	  print(methods,"  {return smaller_than((%sData*)other);}\n",$2);
	}

      print(globhead, "class %s;\n",$2);
      print(globhead, "class %sData;\n",$2);
      print(meta,  "class %s: public Smart<%sData>\n{\n",$2,$2);
      //       print(meta,  "  private:\n    Active%s object;\n",$2);
      print(meta,"  public: template <class SmartRefChild> %s(SmartRefChild& other)\n",current_do);
      print(meta,"    {\n");
      print(meta,"    other.incref();\n");
      print(meta,"    %sData* target = dynamic_cast<%sData*>(other.ptr);\n",current_do,current_do);
      print(meta,"    assert(target);\n");
      print(meta,"    ptr=target;\n");
      print(meta,"    }\n");

      print(meta,"  public: %s(%sData * other):\n",current_do, current_do);
      print(meta,"    Smart<%sData>(other)\n",current_do);
      print(meta,"    {\n");
      print(meta,"    }\n");

      
      // print(meta,"  public: virtual ~%sData();\n",current_do);
    } 
    contents RBRACE SEMICOLON
    {
      if (makers_emitted_for!=current_do)
	{
	  print(methods,"inline bool %sData::smaller_than(%sData* other)\n",current_do,current_do);
	  print(methods,"  { return %sData::smaller_than((%sData*)other); }\n",current_pa,current_pa);
	}
      print(object,"};\n\n");
      print(meta,"};\n\n");
    }

contents
  : contents content
  | {}

content
  : type NAME SEMICOLON 
                          {
			    print(object,"  protected: %s %s;\n",$1,$2); 
			    print(object,"  public: %s get_%s();\n",$1,$2); 
			    print(object,"  public: void set_%s(%s);\n",$2,$1); 
			    
			    if (makers_emitted_for!=current_do && access_checks)
			      {
				print(object,"  public: virtual void change_access(DataAccessFlag flag);\n");
				makers_emitted_for=current_do;
			      }
			    
			    print(meta,"  public: %s get_%s();\n",$1,$2);
			    print(meta,"  public: void set_%s(%s);\n",$2,$1); 

			    print(methods,"inline %s %sData::get_%s()\n",$1,current_do,$2); 
			    print(methods,"  {\n"); 
			    print(methods,"  return %s;\n",$2); 
			    print(methods,"  }\n\n"); 
			    print(methods,"inline void %sData::set_%s(%s __arg)\n",current_do,$2,$1); 
			    print(methods,"  {\n"); 
			    if (access_checks) print(methods,"  will_write();\n");
			    print(methods,"  %s=__arg;\n",$2);
			    print(methods,"  }\n\n");

			    print(methods,"inline %s %s::get_%s()\n",$1,current_do,$2);
			    print(methods,"  {\n");
			    print(methods,"  return ptr->get_%s();\n",$2);
			    print(methods,"  }\n\n");
			    print(methods,"inline void %s::set_%s(%s __arg)\n",current_do,$2,$1); 
			    print(methods,"  {\n"); 
			    print(methods,"  ptr->set_%s(__arg);\n",$2); 
			    print(methods,"  }\n\n");
			  }
  | type funcstf args readwrite SEMICOLON  //{print(object,"  %s %s(%s);\n",$1,$2,args2str($3));}
                          {
			    print(object,"  public: %s %s(%s);\n",$1,$2,args2str($3)); 
  			    print(meta,  "  public: %s %s(%s);\n",$1,$2,args2str($3)); 
			    print(methods,"inline %s %s::%s(%s)\n",$1,current_do,$2,args2str($3));
			    print(methods,"  {\n");
			    if (strcmp($4,"write")==0)
			      print(methods,"  ptr->will_write();\n");
			    if (strcmp($1,"void")==0)
			      print(methods,"  ptr->%s(%s);\n",$2,argnames2str($3));
			    else
			      print(methods,"  return ptr->%s(%s);\n",$2,argnames2str($3));
			    print(methods,"  }\n\n");
			  }
  | type funcstf args CONST SEMICOLON  //{print(object,"  %s %s(%s);\n",$1,$2,args2str($3));}
                          {
			    print(object,"  public: %s %s(%s) const;\n",$1,$2,args2str($3)); 
  			    print(meta,  "  public: %s %s(%s) const;\n",$1,$2,args2str($3)); 
			    print(methods,"inline %s %s::%s(%s) const\n",$1,current_do,$2,args2str($3));
			    print(methods,"  {\n");
			    if (strcmp($1,"void")==0)
			      print(methods,"  ptr->%s(%s);\n",$2,argnames2str($3));
			    else
			      print(methods,"  return ptr->%s(%s);\n",$2,argnames2str($3));
			    print(methods,"  }\n\n");
			  }
  | methodflags type funcstf args readwrite SEMICOLON //{print(object,"  %s %s(%s);\n",$1,$2,args2str($3));}
                          {
			    print(object,"  public: %s %s %s(%s);\n",$1, $2,$3,args2str($4)); 
  			    print(meta,  "  public: %s %s(%s);\n",$2,$3,args2str($4)); 
			    print(methods,"inline %s %s::%s(%s)\n",$2,current_do,$3,args2str($4));
			    print(methods,"  {\n");
			    if (strcmp($5,"write")==0)
			      print(methods,"  ptr->will_write();\n");
			    if (strcmp($1,"void")==0)
			      print(methods,"  ptr->%s(%s);\n",$3,argnames2str($4));
			    else
			      print(methods,"  return ptr->%s(%s);\n",$3,argnames2str($4));
			    print(methods,"  }\n\n");
			  }
  | NAME args SEMICOLON   
                          { 
			    if (strcmp($1,current_do)!=0)
			      {
			        fprintf(stderr, "The current constructor (%s) "
					       "does not equal the dataclass name (%s)\n",
					       	$1,current_do);
				exit(1);
			      }
			    print(object,"  protected: %sData(%s);\n",$1,args2str($2)); 
  			    print(meta,  "  public: %s(%s);\n",$1,args2str($2)); 
			    print(methods,"inline %s::%s(%s):\n",current_do,current_do,args2str($2));
			    print(methods,"  Smart<%sData>(new %sData(%s))\n",current_do,current_do,argnames2str($2));
			    print(methods,"  {\n");
			    print(methods,"  }\n\n");
			  }

type
  : type STAR {$$=cat($1,"*");}
  | CONST type {$$=cat("const ",$2);}
  | NAME 
  | NAME TEMPLATE_OPEN type TEMPLATE_CLOSE {$$=cat(cat($1," < "),cat($3," > ")); }

methodflags 
  : VIRTUAL    {$$=strdup("virtual");}

readwrite
  : READ       {$$=strdup("read"); check_access_access(true);}
  | WRITE      {$$=strdup("write"); check_access_access(true);}
  |            {$$=strdup("ignore"); check_access_access(false);}
  ;

args
  : LPAREN args_lst RPAREN {$$=$2;}
  | LPAREN RPAREN { $$=new vector<s_arg>();}

args_lst
  : args_lst COMMA arg {$$=$1; $$->push_back($3);}
  | arg {$$=new vector<s_arg>(); $$->push_back($1);}

arg
  : type NAME {$$.name = $2; $$.type=$1;}

funcstf
  : OPERATOR BRACKETS {$$=strdup("operator[]");}
  | OPERATOR TEMPLATE_OPEN {$$=strdup("operator<");}
  | OPERATOR EQUALITY {$$=strdup("operator==");}
  | OPERATOR NEQ {$$=strdup("operator!=");}
  | OPERATOR ASSIGNMENT {$$=strdup("operator=");}
  | NAME {$$=$1}

%%
