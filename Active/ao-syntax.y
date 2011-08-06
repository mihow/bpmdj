%{
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
%}
%start aoheaders
%token <token> META NAME LPAREN RPAREN MESSAGE SEMICOLON NUMBER;
%token <token> INIT ACTIVE COMMA LBRACE RBRACE STAR PRECOMPILER;
%token <token> VOLATILE CLASS USES;
%token TEMPLATE_OPEN TEMPLATE_CLOSE

%union
{ 
  char           * token;
  s_arg            arg;
  vector<s_arg>  * args;
}

%type <token> aoheader contents content type init_expression varflags;
%type <args>  args args_lst
%type <arg>   arg;
%%

aoheaders
  : aoheaders aoheader
  |

aoheader
  : PRECOMPILER  { print(globhead,"%s\n",$1); }
  | USES NAME SEMICOLON {assert(pascal); print(useslist,", %s",$2);};
  | CLASS NAME SEMICOLON { print(globhead,"class %s;\n",$2); }
  | ACTIVE NAME LBRACE 
    {
      field2init.clear();
      if (!pascal)
	{
	  print(object,"class Active%s: public ActiveObject< Active%s_msg_* >\n{\n",$2,$2);
	  print(object,"  friend class %s;\n",$2);
	  print(object,"  %s * self;\n",$2);
	  print(object,
		"    virtual elementResult handle( Active%s_msg_* cmd)\n"
		"      {\n"
		"        if (cmd) return cmd->run(this);\n"
		"        else return Done;\n"
		"      };\n",$2);
	  print(globhead, "class %s;\n",$2);
	  print(globhead, "class Active%s;\n",$2);
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
	  print(globhead,"Type "T"Active%s=Class;\n",$2);
	  print(globhead,"Type "T"%s=Class;\n",$2);
	  print(globhead,"Type "T"Active%s_msg_=Class;\n",$2);
	  print(globhead,"Type "T"ActiveObject_"T"Active%s_msg_=specialize ActiveObject<"T"Active%s_msg_>;\n",$2,$2);
	  
	  // pascal implementation
	  print(object,"Type "T"Active%s=Class("T"ActiveObject_"T"Active%s_msg_)\n",$2,$2);
	  print(object,"  public meta: "T"%s;\n",$2);
	  print(object,"  public function handle(cmd: "T"Active%s_msg_): elementResult; override;\n",$2);
	  print(impl,"function "T"Active%s.handle(cmd: "T"Active%s_msg_): elementResult;\n  begin\n",$2);
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
		" */\n",$2,$2,$2,$2);
	  
	  print(msgs1,  "class Active%s_msg_\n",$2);
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
		"    }\n",$2,$2);
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
		"};\n\n",$2);
	  print(meta, 
		"/**\n"
		" * Represents the stub that will transform each incoming call (method)\n"
		" * to an object subclassed from type Active%s_msg_\n"
		" * The stub itself has an instance of the true active object.\n"
		" */\n",$2);
	  print(meta, "class %s\n{\n",$2);
	  print(meta, "  private:\n"
		"    /**\n"
		"     * The object that is covered for by this stub. The fact that the\n"
		"     * stub allocates the object ensures that only one of it exists and that any\n"
		"     * interaction must go through the stub. As such it is kept private.\n"
		"     */\n"
		"    Active%s object;\n",$2);
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
	  print(meta, "    %s(string name=\"%s\"): object(this, name) {};\n",$2,$2);
	  current_ao = strdup($2);
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
		" *)\n",$2,$2,$2,$2);
	  
	  print(msgs1,"Type "T"Active%s_msg_=class\n",$2);
	  print(msgs1,"  public function run(caller: "T"Active%s): elementResult; virtual; abstract;\n",$2);
	  print(msgs1,"  end;\n\n");
	  print(meta,"Type "T"%s=class\n",$2);
	  print(meta, "  private\n"
		"    _object: "T"Active%s;\n",$2);
	  print(meta,"  public\n");
	  print(meta, "    Constructor Create(bloody_local_var_name: string=\'%s\');\n",$2);
	  
	  print(impl,"Constructor "T"%s.Create(bloody_local_var_name: string);\nbegin\n  _object:="T"Active%s.Create(self,bloody_local_var_name);\nend;\n\n",
		$2,$2);
	  current_ao = strdup($2);
	}
    } 
    contents RBRACE SEMICOLON
    {
      if (!pascal)
	{
	  print(object,
		"  protected:\n    Active%s(%s* s, string name):\n"
		"      ActiveObject< Active%s_msg_ * >(name), self(s)\n",$2,$2,$2);
	  print(object, "      {\n");
	}
      else
	{
	  print(object,"  public constructor Create(s: "T"%s; bloody_local_name: string);\n",$2);
	  print(impl,"constructor "T"Active%s.Create(s: "T"%s; bloody_local_name: string);\n"
		"  begin\n"
		"  inherited Create(bloody_local_name);\n"
		"  meta:=s;\n",$2,$2);
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
    }

contents
  : contents content
  | {}

content
  : type NAME SEMICOLON 
  {if (!pascal)
      print(object,"  %s %s;\n",$1,$2); 
    else
      print(object,"  public %s: %s;\n",$2,$1);
    field2init[$2]=""; 
  }
  | type NAME INIT init_expression SEMICOLON
  {
    if (!pascal)
      print(object,"  %s %s;\n",$1,$2);
    else
      print(object,"  public %s: %s;\n",$2,$1); 
    field2init[$2]=$4; 
  }
  | varflags type NAME SEMICOLON 
    {print(object,"  %s %s %s;\n",$1,$2,$3); field2init[$3]=""; }
  | varflags type NAME INIT init_expression SEMICOLON
    {print(object,"  %s %s %s;\n",$1,$2,$3); field2init[$3]=$5; }
  | type NAME args SEMICOLON {print(object,"  %s %s(%s);\n",$1,$2,args2str($3));}
  | META type NAME args SEMICOLON 
    {
      print(meta,
	    "  public:\n"
	    "    %s %s(%s);\n",$2,$3,args2str($4));
    }
  | MESSAGE NAME args SEMICOLON 
    {
      // the object code
      if (!pascal)
	{
	  print(object,"  public: elementResult %s(%s);\n",$2,args2str($3));
	  print(object,"  protected: void queue_%s(%s);\n",$2,args2str($3));
	}
      else
	{
	  print(object,"  public function %s(%s): elementResult;\n",$2,args2str($3,"; ",""));
	  print(object,"  public procedure queue_%s(%s);\n",$2,args2str($3,"; ", "bloody_local_var_"));
	}
      
      // the message class
      if (!pascal)
	{
	  print(msgs,"class Active%s_msg_%s: public Active%s_msg_\n",current_ao,$2,current_ao);
	  print(msgs,"{\n    %s;\n",args2str($3,";\n    "));
	  print(msgs,"  public:\n    Active%s_msg_%s(%s)",current_ao,$2,args2str($3));
	  if ($3->size())
	    print(msgs," : %s\n      {\n      };\n",copyargs($3));
	  else
	    print(msgs,"\n      {\n      };\n");
	  print(msgs, "    virtual string declaration()\n"
		"    {\n"
		"      return \"%s::%s(%s)\";\n"
		"    }\n",current_ao,$2,args2str($3)
		);
	  print(msgs,"    virtual elementResult run(Active%s * ao)\n",current_ao);
	  print(msgs,"      {\n        ENTER_MSG;\n");
	  print(msgs,"        elementResult res = ao->%s(%s);\n",$2,argnames2str($3));
	  print(msgs,"        LEAVE_MSG;\n",$2,argnames2str($3));
	  print(msgs,"        return res;\n      };\n",$2,argnames2str($3));
	  print(msgs,"};\n\n");
	}
      else
	{
	  print(globhead,"Type "T"Active%s_msg_%s=Class;\n",current_ao,$2);
	  print(msgs,"Type "T"Active%s_msg_%s=Class("T"Active%s_msg_)\n",current_ao,$2,current_ao);
	  if ($3->size())
	    print(msgs,"  public %s;\n",args2str($3,";\n  "));
	  print(msgs,"  constructor Create(%s);\n",args2str($3,"; ","bloody_local_var_"));
	  print(impl,"constructor "T"Active%s_msg_%s.Create(%s);\n  begin\n",current_ao,$2,args2str($3,"; ","bloody_local_var_"));
	  
	  if ($3->size())
	    print(impl,"%s  end;\n\n",copyargs($3));
	  else
	    print(impl,"  end;\n\n");

	  print(msgs,"  public function run(ao: "T"Active%s): elementResult; override;\n",current_ao);
	  print(impl,"function "T"Active%s_msg_%s.run(ao: "T"Active%s): elementResult;\n  begin\n",current_ao, $2, current_ao);
	  print(impl,"  Result:=ao.%s(%s);\n  end;\n\n",$2,argnames2str($3));
	  print(msgs,"end;\n\n");
	}
      // the meta call
      if (!pascal)
	{
	  print(object2,"inline void Active%s::queue_%s(%s)\n",current_ao,$2,args2str($3));
	  print(object2,"  {\n");
	  print(object2,"    push(new Active%s_msg_%s(%s));\n  };\n",
		current_ao,$2,argnames2str($3));
	}
      else
	{
	  print(impl,"procedure "T"Active%s.queue_%s(%s);\n  begin\n",current_ao,$2,args2str($3,"; ","bloody_local_var_"));
	  print(impl,"  push("T"Active%s_msg_%s.Create(%s));\n  end;\n\n",
		current_ao,$2,argnames2str($3,"bloody_local_var_"));
	}
      if (!pascal)
	{
	  print(meta,  "  public:\n    void %s(%s)\n",$2,args2str($3,", "));
	  print(meta,  "    {\n      object.queue_%s(%s);\n    };\n",$2,argnames2str($3));
	}
      else
	{
	  print(meta,  "  public procedure %s(%s);\n",$2,args2str($3,"; ","bloody_local_var_"));
	  print(impl,  "procedure "T"%s.%s(%s);\n",current_ao,$2,args2str($3,"; ","bloody_local_var_"));
	  print(impl,  "  begin\n    _object.queue_%s(%s);\n  end;\n\n",$2,argnames2str($3,"bloody_local_var_"));
	}
    }

type
  : type STAR {$$=cat($1,"*");}
  | NAME 
  | NAME TEMPLATE_OPEN type TEMPLATE_CLOSE {$$=cat(cat($1," < "),cat($3," > ")); }
| NAME TEMPLATE_OPEN type COMMA type TEMPLATE_CLOSE {$$=  cat(cat(cat($1, "<"), cat($3,",")), cat($5,">"));}

varflags 
  : VOLATILE   {$$=strdup("volatile");}

args
  : LPAREN args_lst RPAREN {$$=$2;}
  | LPAREN RPAREN { $$=new vector<s_arg>();}

args_lst
  : args_lst COMMA arg {$$=$1; $$->push_back($3);}
  | arg {$$=new vector<s_arg>(); $$->push_back($1);}

arg
  : type NAME {$$.name = $2; $$.type=$1;}

init_expression
  : NUMBER
  | NAME
%%
