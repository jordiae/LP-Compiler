#header
<<
#include <string>
#include <iostream>
using namespace std;

// struct to store information about tokens
typedef struct {
  string kind;
  string text;
} Attrib;

// function to fill token information (predeclaration)
void zzcr_attr(Attrib *attr, int type, char *text);

// fields for AST nodes
#define AST_FIELDS string kind; string text;
#include "ast.h"

// macro to create a new AST node (and function predeclaration)
#define zzcr_ast(as,attr,ttype,textt) as=createASTnode(attr,ttype,textt)
AST* createASTnode(Attrib* attr, int ttype, char *textt);
#define createASTlist #0=new AST;(#0)->kind="list";(#0)->right=NULL;(#0)->down=_sibling;
>>



<<
#include <cstdlib>
#include <cmath>
// function to fill token information
void zzcr_attr(Attrib *attr, int type, char *text) {
  if (type == NUM) {
    attr->kind = "intconst";
    attr->text = text;
  }
  else {
    attr->kind = text;
    attr->text = "";
  }
}

// function to create a new AST node
AST* createASTnode(Attrib* attr, int type, char* text) {
  AST* as = new AST;
  as->kind = attr->kind; 
  as->text = attr->text;
  as->right = NULL; 
  as->down = NULL;
  return as;
}

/// get nth child of a tree. Count starts at 0.
/// if no such child, returns NULL
AST* child(AST *a,int n) {
 AST *c=a->down;
 for (int i=0; c!=NULL && i<n; i++) c=c->right;
 return c;
} 

/// print AST, recursively, with indentation
void ASTPrintIndent(AST *a,string s)
{
  if (a==NULL) return;

  cout<<a->kind;
  if (a->text!="") cout<<"("<<a->text<<")";
  cout<<endl;

  AST *i = a->down;
  while (i!=NULL && i->right!=NULL) {
    cout<<s+"  \\__";
    ASTPrintIndent(i,s+"  |"+string(i->kind.size()+i->text.size(),' '));
    i=i->right;
  }
  
  if (i!=NULL) {
      cout<<s+"  \\__";
      ASTPrintIndent(i,s+"   "+string(i->kind.size()+i->text.size(),' '));
      i=i->right;
  }
}

/// print AST 
void ASTPrint(AST *a)
{
  while (a!=NULL) {
    cout<<" ";
    ASTPrintIndent(a,"");
    a=a->right;
  }
}

int main() {
  AST *root = NULL;
  ANTLR(lists(&root), stdin);
  ASTPrint(root);
}
>>


#lexclass START
#token ASSIGNMENT "="
#token IF_KEYWORD "if"
#token WHILE_KEYWORD "while"
#token OPAR "\("
#token CPAR "\)"
#token THEN_KEYWORD "then"
#token ENDIF_KEYWORD "endif"
#token DO_KEYWORD "do"
#token ENDWHILE_KEYWORD "endwhile"
#token FLATTEN_KEYWORD "flatten"
#token PRINT_KEYWORD "print"
#token LREDUCE_KEYWORD "lreduce"
#token LMAP_KEYWORD "lmap"
#token LFILTER_KEYWORD "lfilter"
#token HEAD_KEYWORD "head"
#token POP_KEYWORD "pop"
#token EMPTY_KEYWORD "empty"
#token SUM "\+"
#token MINUS "\-"
#token MUL "\*"
#token EQUAL "=="
#token NEQUAL "!="
#token GREATER ">"
#token LESSER "<"
#token NOT_KEYWORD "not"
#token AND_KEYWORD "and"
#token OR_KEYWORD "or"
#token CONCATTAG "#"
///#token ID "('a'..'z'|'A'..'Z') ('a'..'z'|'A'..'Z'|'0'..'9')*"
///#token ID "[a-zA-Z0-9]" // FALTA PROHIBIR QUE COMENCIN PER NUMNEROS?
#token ID "[a-zA-Z]"
#token NUM "[0-9]+"
#token OBRACKET "\["
#token CBRACKET "\]"
#token COMMA ","
#token SPACE "[\ \n]" << zzskip(); >>

///program: lists "@";
lists: (list_oper)* <<#0=createASTlist(_sibling);>> ;
///list_oper: if_oper | while_oper | assignment_oper | flatten_oper | print_oper;
list_oper: if_oper | while_oper | assignment_oper | flatten_oper | print_oper | pop_oper;
if_oper: IF_KEYWORD OPAR condition CPAR THEN_KEYWORD lists ENDIF_KEYWORD;
while_oper: WHILE_KEYWORD OPAR condition CPAR DO_KEYWORD lists ENDWHILE_KEYWORD;
assignment_oper: ID ASSIGNMENT list;
flatten_oper: FLATTEN_KEYWORD ID;
print_oper: PRINT_KEYWORD list;
pop_oper: POP_KEYWORD OPAR list CPAR;
///list: list_val | list_function | ID;
list: list_val | ID ((CONCATTAG ID) | ) | unary | lfunc;
///condition: boolean_expr ((binary_boolean_op)? boolean_expr)*;
/// no provat que aquest canvi sigui necessari perque no surt al test, pero te pinta
condition: boolean_expr ((binary_boolean_op|) boolean_expr)*;
binary_boolean_op: AND_KEYWORD | OR_KEYWORD;
list_val: OBRACKET ( ((list_val|NUM) (COMMA (list_val|NUM))*) | ) CBRACKET;
///list_val: OBRACKET ((list_val|NUM) (COMMA (list_val|NUM))*)? CBRACKET;
///list_val: (OBRACKET CBRACKET) | (OBRACKET (list_val|NUM) (COMMA (list_val|NUM))* CBRACKET);
///list_val: OBRACKET CBRACKET | OBRACKET (atom|list_val) (COMMA (atom|listval))* CBRACKET;
///list_val: (OBRACKET (list_val|NUM) (COMMA (list_val|NUM))* CBRACKET) | OBRACKET CBRACKET; /// posat al reves no ho accepta per prioritat (ambiguitat)


///boolean_expr: (NOT_KEYWORD)? ((EMPTY_KEYWORD OPAR ID CPAR) | (list_relational));
boolean_expr: (NOT_KEYWORD|) ((EMPTY_KEYWORD OPAR ID CPAR) | (list_relational)); /// nota: falten ands, ors,... expressions booleanes compostes? NO, ja esta

/// dubte: empty podria rebre una cosa que no fos un identificador? todo, i comprovar que sempre es pugui fer amb qualsevol tipus de llista (o no, si s'exigeix aplanada?)

list_relational: list (EQUAL | NEQUAL | GREATER | LESSER) list;/// nomes aplanades en realitat...
///list_function: unary | binary | lfunc;
///unary: (POP_KEYWORD | HEAD_KEYWORD | EMPTY_KEYWORD) OPAR ID CPAR;
///unary: (POP_KEYWORD | HEAD_KEYWORD) OPAR ID CPAR;
unary: HEAD_KEYWORD OPAR ID CPAR; /// pop no retorna res, no compta com a expressio
///binary: ID CONCATTAG ID;
lfunc: lreduce | lmap | lfilter;
lreduce: LREDUCE_KEYWORD arithmetic ID;
///lmap: LMAP_KEYWORD arithmetic ID;
lmap: LMAP_KEYWORD arithmetic NUM ID;
lfilter: LFILTER_KEYWORD relational ID;
///arithmetic: (SUM|MINUS|MUL) NUM;
/// todo, dubte: assumim que lmap nomes rep un operador i en canvi lmap operador i nombre?
arithmetic: (SUM|MINUS|MUL);
relational: (EQUAL|NEQUAL|GREATER|LESSER) NUM;
///(LREDUCE_KEYWORD | LMAP_KEYWORD | LFILTER_KEYWORD) lop ID
