/*
 * A n t l r  T r a n s l a t i o n  H e a d e r
 *
 * Terence Parr, Will Cohen, and Hank Dietz: 1989-2001
 * Purdue University Electrical Engineering
 * With AHPCRC, University of Minnesota
 * ANTLR Version 1.33MR33
 *
 *   antlr -gt numlistlang2_interpret.g
 *
 */

#define ANTLR_VERSION	13333
#include "pcctscfg.h"
#include "pccts_stdio.h"

#include <string>
#include <iostream>
#include <map>
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
#define createASTlist (*_root)=new AST;((*_root))->kind="list";((*_root))->right=NULL;((*_root))->down=_sibling;
#define GENAST

#include "ast.h"

#define zzSET_SIZE 8
#include "antlr.h"
#include "tokens.h"
#include "dlgdef.h"
#include "mode.h"

/* MR23 In order to remove calls to PURIFY use the antlr -nopurify option */

#ifndef PCCTS_PURIFY
#define PCCTS_PURIFY(r,s) memset((char *) &(r),'\0',(s));
#endif

#include "ast.c"
zzASTgvars

ANTLR_INFO


#include <cstdlib>
#include <cmath>

map<string,string> symtab; /// keep variables, com al lab. values son string perque llistes poden ser heterogenies

// function to fill token information
void zzcr_attr(Attrib *attr, int type, char *text) {
  if (type == NUM) {
    attr->kind = "intconst";
    attr->text = text;
    ///cout << "zzcr_attr if" <<  attr->text << endl;
  }
  else if (type == ID) {
    attr->kind = "id";
    attr->text = text;
  }
  else {
    ///cout << "zzcr_attr else" <<  text << " " << type << endl;
    attr->kind = text;
    attr->text = "";
  }
  /*attr->kind = type;
  attr->text = text;
  cout << type << " " << text << endl;*/
  /*if (type == NUM) {
    attr->kind = NUM;
    attr->text = text;
  }
  else if (type == ID) {
    attr->kind = ID;
    attr->text = "";
  }
  else if (type == ASSIGNMENT) {
    attr->kind = ASSIGNMENT;
    attr->text = "";
  }
  else if (type == OBRACKET) {
    attr->kind = OBRACKET;
    attr->text = "";
  }
  else {
    attr->kind = text;
    attr->text = "";
  }*/
}

// function to create a new AST node
AST* createASTnode(Attrib* attr, int type, char* text) {
  AST* as = new AST;
  as->kind = attr->kind; 
  as->text = attr->text;
  ///as->type = attr->type; ///
  as->right = NULL; 
  as->down = NULL;
  return as;
}

/*AST* createASTlist(AST *child) {
  AST *as=new AST;
  as->kind="list";
  as->right=NULL;
  as->down=child;
  return as;
}*/

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
  
  ///cout<<a->kind;
  /*if (a->text!="") cout<<"("<<a->text<<")";*/
  /// (pels parentesis de intconst)
  if (a->text!="") cout<<a->text;
  else cout<<a->kind;
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

/*string evaluate(AST *a) {
  return 0;
  if (a == NULL) {
    cout << "NULL_EVALUATE" << endl;
    return "";
  }
  else if (a->kind == "num") {
    cout << "num_EVALUATE" << endl;
    return (a->text.c_str());
  }
  else if (a->kind == "assignment") {
    cout << "assignment_EVALUATE";
    return "";
  }
  else if (a->kind == "obracket") {
    cout << "obracket_EVALUATE";
    return "";
  }
  else {
    cout << "FAIL_EVALUATE";
    return "FAIL_EVALUATE";
  }
}*/

/*void execute(AST *a) {
  return;
  if (a == NULL) {
    cout << "NULL_EXECUTE" << endl;
    return;  
  }
  else if (a->kind == "assignment_oper") {
    cout << "assignment_oper_EXECUTE" << endl;
    symtab[child(a,0)->text] = evaluate(child(a,1));
  }
  else if (a->kind == "print") {
    cout << "print_EXECUTE" << endl;
    cout << evaluate(child(a,0)) << endl;
  }
  else
  cout << "FAIL_EXECUTE" << endl;
  
  execute(a->right);
}*/

string concat(string s1, string s2) {
  if (s1.length() == 2)
  return s2;
  else if (s2.length() == 2)
  return s1;
  string s1_ = s1.substr(1,s1.length()-2);
  string s2_ = s2.substr(1,s2.length()-2);
  return "[" + s1_ + "," + s2_ + "]";
}

string head_pop(string s, int& posi, int& posf) {
  if (s[1] == ']') {
    return "";
    posi = 0;
    posf = 0;
  }
  string h = "";
  if (s[1] >= '0' and s[1] <= '9') {
    int i = 1;
    while (s[i] >= '0' and s[i] <= '9') {
      h += s[i];
      i++;
    }
    posi = 1;
    posf = i-1;
  }
  else {
    int bracket_counter = 1;
    h = s[1];
    int i = 2;
    while (bracket_counter > 0 and i < s.length()) {
      if (s[i] == '[')
      bracket_counter++;
      else if (s[i] == ']')
      bracket_counter--;
      h+= s[i];
      i++;
    }
    posi = 1;
    posf = i-1;
    
    }
  return h;
}

string head (string s) {
  int x = 0;
  int y = 0;
  return head_pop(s,x,y);
}

string flatten(string s) {
  string r = "";
  int j = 0;
  for (int i = 0; i < s.length(); i++) {
    if ((s[i] != '[' and s[i] != ']' and s[i] != ',') or (s[i] == ',' and (r[j-1] != ',' and r[j-1] != '[')) or i == 0 or i == s.length()-1) {
      r += s[i];
      j++;
    }
  }
  return r;
}

string pop(string s) {
  int x = 0;
  int y = 0;
  string f = head_pop(s,x,y);
  ///string r = s.substr(x,(y-x+1));
  ///cout << "pop: " << f << " " << s << " " << r << " " << x << " " << y << endl;
  string r = s.substr(0,x) + s.substr(y+2,s.length());
  return r;
}

string evaluate(AST *a) {
  if (a == NULL) {
    return "";
  }
  else if (a->kind == "head") {
    return head(symtab[child(a,0)->text]);
  }
  else if (a->kind == "#") { // nomes sobre identificadors, assoc esquerra
    return concat(symtab[child(a,0)->text],symtab[child(a,1)->text]);
  }
  else if (a->kind == "[") {
    if (a->right == NULL)
    return "[" + evaluate(child(a,0)) + "]";
    else {
      return "[" + evaluate(child(a,0)) + "]" + "," + evaluate(a->right)  ;
    }
    ///return "[" + evaluate(child(a,0)) + "]";
  }
  else if (a->kind == "id") {
    return symtab[a->text];
  }
  else if (a->kind == "intconst") {
    if (a->right == NULL)
    return a->text;
    else {
      return a->text + "," + evaluate(a->right);
    }
  }
  else if (a->kind == "empty") { // not tested
    if (symtab[child(a,0)->text] == "[]")
    return "True";
    else
    return "False";
  }
}

void execute(AST *a) {
  if (a == NULL) {
    return;  
  }
  else if (a->kind == "list") {
    execute(a->down);
  }
  else if (a->kind == "=") {
    symtab[child(a,0)->text] = evaluate(child(a,1));
    execute(a->right);
  }
  else if (a->kind == "print") {
    /// todo: list: list_val | ID ((CONCATTAG^ ID) | ) | unary | lfunc;
    ///cout << symtab[child(a,0)->text] << endl;
    cout << evaluate(child(a,0)) << endl;
    /*if (child(a,0)->kind == "id") {
      cout << symtab[child(a,0)->text] << endl;
    }
    else {
      cout << evaluate(child(a,0)) << endl;
    }
    return;
    if (child(a,0)->kind == "[") {
      cout << evaluate(child(a,0)) << endl;
    }
    else if (child(a,0)->kind == "#") {
      cout << evaluate(child(a,0)) << endl;
    }
    else {
      cout << symtab[child(a,0)->text] << endl;
    }*/
    execute(a->right);
  }
  else if (a->kind == "flatten") {
    symtab[child(a,0)->text] = flatten(symtab[child(a,0)->text]);
    execute(a->right);
  }
  else if (a->kind == "pop") {
    symtab[child(a,0)->text] = pop(symtab[child(a,0)->text]);
    execute(a->right);
  }
  
}


int main() {
  AST *root = NULL;
  ANTLR(lists(&root), stdin);
  ///ANTLR(program(&root), stdin);
  ASTPrint(root);
  execute(root);
}

void
#ifdef __USE_PROTOS
lists(AST**_root)
#else
lists(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (setwd1[LA(1)]&0x1) ) {
      list_oper(zzSTR); zzlink(_root, &_sibling, &_tail);
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
  (*_root)=createASTlist(_sibling);
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x2);
  }
}

void
#ifdef __USE_PROTOS
list_oper(AST**_root)
#else
list_oper(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==IF_KEYWORD) ) {
    if_oper(zzSTR); zzlink(_root, &_sibling, &_tail);
  }
  else {
    if ( (LA(1)==WHILE_KEYWORD) ) {
      while_oper(zzSTR); zzlink(_root, &_sibling, &_tail);
    }
    else {
      if ( (LA(1)==ID) ) {
        assignment_oper(zzSTR); zzlink(_root, &_sibling, &_tail);
      }
      else {
        if ( (LA(1)==FLATTEN_KEYWORD) ) {
          flatten_oper(zzSTR); zzlink(_root, &_sibling, &_tail);
        }
        else {
          if ( (LA(1)==PRINT_KEYWORD) ) {
            print_oper(zzSTR); zzlink(_root, &_sibling, &_tail);
          }
          else {
            if ( (LA(1)==POP_KEYWORD) ) {
              pop_oper(zzSTR); zzlink(_root, &_sibling, &_tail);
            }
            else {zzFAIL(1,zzerr1,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
          }
        }
      }
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x4);
  }
}

void
#ifdef __USE_PROTOS
if_oper(AST**_root)
#else
if_oper(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(IF_KEYWORD); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(OPAR);  zzCONSUME;
  condition(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzmatch(CPAR);  zzCONSUME;
  zzmatch(THEN_KEYWORD);  zzCONSUME;
  lists(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzmatch(ENDIF_KEYWORD);  zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x8);
  }
}

void
#ifdef __USE_PROTOS
while_oper(AST**_root)
#else
while_oper(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(WHILE_KEYWORD); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(OPAR);  zzCONSUME;
  condition(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzmatch(CPAR);  zzCONSUME;
  zzmatch(DO_KEYWORD);  zzCONSUME;
  lists(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzmatch(ENDWHILE_KEYWORD);  zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x10);
  }
}

void
#ifdef __USE_PROTOS
assignment_oper(AST**_root)
#else
assignment_oper(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(ASSIGNMENT); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  list(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x20);
  }
}

void
#ifdef __USE_PROTOS
flatten_oper(AST**_root)
#else
flatten_oper(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(FLATTEN_KEYWORD); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x40);
  }
}

void
#ifdef __USE_PROTOS
print_oper(AST**_root)
#else
print_oper(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(PRINT_KEYWORD); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  list(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x80);
  }
}

void
#ifdef __USE_PROTOS
pop_oper(AST**_root)
#else
pop_oper(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(POP_KEYWORD); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(OPAR);  zzCONSUME;
  list(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzmatch(CPAR);  zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x1);
  }
}

void
#ifdef __USE_PROTOS
list(AST**_root)
#else
list(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==OBRACKET) ) {
    list_val(zzSTR); zzlink(_root, &_sibling, &_tail);
  }
  else {
    if ( (LA(1)==ID) ) {
      zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
      {
        zzBLOCK(zztasp2);
        zzMake0;
        {
        if ( (LA(1)==1) ) {
          zzmatch(1);  zzCONSUME;
        }
        else {
          if ( (LA(1)==CONCATTAG) ) {
            {
              zzBLOCK(zztasp3);
              zzMake0;
              {
              zzmatch(CONCATTAG); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
              zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
              zzEXIT(zztasp3);
              }
            }
          }
          else {
            if ( (setwd2[LA(1)]&0x2) ) {
            }
            else {zzFAIL(1,zzerr2,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
          }
        }
        zzEXIT(zztasp2);
        }
      }
    }
    else {
      if ( (LA(1)==HEAD_KEYWORD) ) {
        unary(zzSTR); zzlink(_root, &_sibling, &_tail);
      }
      else {
        if ( (setwd2[LA(1)]&0x4) ) {
          lfunc(zzSTR); zzlink(_root, &_sibling, &_tail);
        }
        else {zzFAIL(1,zzerr3,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
      }
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x8);
  }
}

void
#ifdef __USE_PROTOS
condition(AST**_root)
#else
condition(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  boolean_expr(zzSTR); zzlink(_root, &_sibling, &_tail);
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (setwd2[LA(1)]&0x10) ) {
      binary_boolean_op(zzSTR); zzlink(_root, &_sibling, &_tail);
      boolean_expr(zzSTR); zzlink(_root, &_sibling, &_tail);
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x20);
  }
}

void
#ifdef __USE_PROTOS
binary_boolean_op(AST**_root)
#else
binary_boolean_op(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==AND_KEYWORD) ) {
    zzmatch(AND_KEYWORD); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  }
  else {
    if ( (LA(1)==OR_KEYWORD) ) {
      zzmatch(OR_KEYWORD); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
    }
    else {zzFAIL(1,zzerr4,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x40);
  }
}

void
#ifdef __USE_PROTOS
list_val(AST**_root)
#else
list_val(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(OBRACKET); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (setwd2[LA(1)]&0x80) ) {
      {
        zzBLOCK(zztasp3);
        zzMake0;
        {
        {
          zzBLOCK(zztasp4);
          zzMake0;
          {
          if ( (LA(1)==OBRACKET) ) {
            list_val(zzSTR); zzlink(_root, &_sibling, &_tail);
          }
          else {
            if ( (LA(1)==NUM) ) {
              zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
            }
            else {zzFAIL(1,zzerr5,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
          }
          zzEXIT(zztasp4);
          }
        }
        {
          zzBLOCK(zztasp4);
          zzMake0;
          {
          while ( (LA(1)==COMMA) ) {
            zzmatch(COMMA);  zzCONSUME;
            {
              zzBLOCK(zztasp5);
              zzMake0;
              {
              if ( (LA(1)==OBRACKET) ) {
                list_val(zzSTR); zzlink(_root, &_sibling, &_tail);
              }
              else {
                if ( (LA(1)==NUM) ) {
                  zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
                }
                else {zzFAIL(1,zzerr6,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
              }
              zzEXIT(zztasp5);
              }
            }
            zzLOOP(zztasp4);
          }
          zzEXIT(zztasp4);
          }
        }
        zzEXIT(zztasp3);
        }
      }
    }
    else {
      if ( (LA(1)==CBRACKET) ) {
      }
      else {zzFAIL(1,zzerr7,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
  zzmatch(CBRACKET);  zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd3, 0x1);
  }
}

void
#ifdef __USE_PROTOS
boolean_expr(AST**_root)
#else
boolean_expr(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (LA(1)==NOT_KEYWORD) ) {
      zzmatch(NOT_KEYWORD); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
    }
    else {
      if ( (setwd3[LA(1)]&0x2) ) {
      }
      else {zzFAIL(1,zzerr8,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    {
      zzBLOCK(zztasp3);
      zzMake0;
      {
      if ( (LA(1)==EMPTY_KEYWORD) ) {
        {
          zzBLOCK(zztasp4);
          zzMake0;
          {
          empty(zzSTR); zzlink(_root, &_sibling, &_tail);
          zzEXIT(zztasp4);
          }
        }
      }
      else {
        if ( (setwd3[LA(1)]&0x4) ) {
          {
            zzBLOCK(zztasp4);
            zzMake0;
            {
            list_relational(zzSTR); zzlink(_root, &_sibling, &_tail);
            zzEXIT(zztasp4);
            }
          }
        }
        else {zzFAIL(1,zzerr9,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
      }
      zzEXIT(zztasp3);
      }
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd3, 0x8);
  }
}

void
#ifdef __USE_PROTOS
empty(AST**_root)
#else
empty(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(EMPTY_KEYWORD); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(OPAR);  zzCONSUME;
  zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(CPAR);  zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd3, 0x10);
  }
}

void
#ifdef __USE_PROTOS
list_relational(AST**_root)
#else
list_relational(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  list(zzSTR); zzlink(_root, &_sibling, &_tail);
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (LA(1)==EQUAL) ) {
      zzmatch(EQUAL); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
    }
    else {
      if ( (LA(1)==NEQUAL) ) {
        zzmatch(NEQUAL); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
      }
      else {
        if ( (LA(1)==GREATER) ) {
          zzmatch(GREATER); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
        }
        else {
          if ( (LA(1)==LESSER) ) {
            zzmatch(LESSER); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
          }
          else {zzFAIL(1,zzerr10,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
        }
      }
    }
    zzEXIT(zztasp2);
    }
  }
  list(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd3, 0x20);
  }
}

void
#ifdef __USE_PROTOS
unary(AST**_root)
#else
unary(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(HEAD_KEYWORD); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(OPAR);  zzCONSUME;
  zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(CPAR);  zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd3, 0x40);
  }
}

void
#ifdef __USE_PROTOS
lfunc(AST**_root)
#else
lfunc(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==LREDUCE_KEYWORD) ) {
    lreduce(zzSTR); zzlink(_root, &_sibling, &_tail);
  }
  else {
    if ( (LA(1)==LMAP_KEYWORD) ) {
      lmap(zzSTR); zzlink(_root, &_sibling, &_tail);
    }
    else {
      if ( (LA(1)==LFILTER_KEYWORD) ) {
        lfilter(zzSTR); zzlink(_root, &_sibling, &_tail);
      }
      else {zzFAIL(1,zzerr11,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd3, 0x80);
  }
}

void
#ifdef __USE_PROTOS
lreduce(AST**_root)
#else
lreduce(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(LREDUCE_KEYWORD); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  arithmetic(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd4, 0x1);
  }
}

void
#ifdef __USE_PROTOS
lmap(AST**_root)
#else
lmap(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(LMAP_KEYWORD); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  arithmetic(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd4, 0x2);
  }
}

void
#ifdef __USE_PROTOS
lfilter(AST**_root)
#else
lfilter(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(LFILTER_KEYWORD); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  relational(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd4, 0x4);
  }
}

void
#ifdef __USE_PROTOS
arithmetic(AST**_root)
#else
arithmetic(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (LA(1)==SUM) ) {
      zzmatch(SUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
    }
    else {
      if ( (LA(1)==MINUS) ) {
        zzmatch(MINUS); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
      }
      else {
        if ( (LA(1)==MUL) ) {
          zzmatch(MUL); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
        }
        else {zzFAIL(1,zzerr12,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
      }
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd4, 0x8);
  }
}

void
#ifdef __USE_PROTOS
relational(AST**_root)
#else
relational(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (LA(1)==EQUAL) ) {
      zzmatch(EQUAL); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
    }
    else {
      if ( (LA(1)==NEQUAL) ) {
        zzmatch(NEQUAL); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
      }
      else {
        if ( (LA(1)==GREATER) ) {
          zzmatch(GREATER); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
        }
        else {
          if ( (LA(1)==LESSER) ) {
            zzmatch(LESSER); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
          }
          else {zzFAIL(1,zzerr13,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
        }
      }
    }
    zzEXIT(zztasp2);
    }
  }
  zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd4, 0x10);
  }
}
