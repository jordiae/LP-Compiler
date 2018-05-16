/*
 * A n t l r  T r a n s l a t i o n  H e a d e r
 *
 * Terence Parr, Will Cohen, and Hank Dietz: 1989-2001
 * Purdue University Electrical Engineering
 * With AHPCRC, University of Minnesota
 * ANTLR Version 1.33MR33
 *
 *   antlr -gt numlistlang2_def2.g
 *
 */

#define ANTLR_VERSION	13333
#include "pcctscfg.h"
#include "pccts_stdio.h"

#include <string>
#include <iostream>
#include <map>
#include <vector>
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
#include <sstream>

// nota: ja vaig refer el canvi de passar-ho a struct en comptes de manejar les llistes com a strings

struct elem {
  bool isNull;
  bool hasValue;
  vector<elem> elems;
  int value;
};

map<string,elem> symtab;

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


string elemToString(elem e) {
  if (e.isNull)
  return "";
  if (e.hasValue) {
    string res; 
    ostringstream convert;
    convert << e.value;
    res = convert.str();
    return res;
    // mirar si deixen c++11, si si, fer-ho amb to_string. preguntar (todo)
  }
  if (e.elems.size() == 0)
  return "[]";
  string s = "[";
  for (int i = 0; i < e.elems.size(); i++) {
    s += elemToString(e.elems[i]);
    if (i != (e.elems.size() - 1))
    s += ",";
  }
  s += "]";
  return s;
}

elem head(elem e) {
  return e.elems[0]; 
}

elem pop(elem e) {
  e.elems.erase(e.elems.begin());
  return e;
}

elem concat(elem e1, elem e2) {
  e1.elems.insert(e1.elems.end(),e2.elems.begin(),e2.elems.end());
  return e1;
}

elem flatten(elem e) {
  vector<elem> elems_flat;
  for (int i = 0; i < e.elems.size(); i++) {
    if (not e.elems[i].isNull and e.elems[i].hasValue) {
      elems_flat.push_back(e.elems[i]);
    }
    else if (not e.elems[i].isNull and not e.elems[i].hasValue) {
      elem e1 = flatten(e.elems[i]);
      elems_flat.insert(elems_flat.end(),e1.elems.begin(),e1.elems.end());
    }
    
  }
  elem ef;
  ef.isNull = false;
  ef.hasValue= false;
  ef.elems = elems_flat;
  return ef;
}

elem lmap (string op, int x, elem  e) {
  if (e.isNull)
  return e;
  if (not e.isNull and e.hasValue) {
    if (op == "+") {
      e.value += x;
      return e;
    }
    else if (op == "-") {
      e.value -= x;
      return e;
    }
    else {
      e.value *= x;
      return e;
    }
  }
  for (int i = 0; i < e.elems.size(); i++) {
    e.elems[i] = lmap(op,x,e.elems[i]);
  }
  return e;
}

elem lreduce (string op, int& acum, elem  e) {
  if (e.isNull)
  return e;
  if (not e.isNull and e.hasValue) {
    if (op == "+") {
      acum += e.value;
      return e;
    }
    else if (op == "-") {
      acum -= e.value;
      return e;
    }
    else {
      acum *= e.value;
      return e;
    }
  }
  //int value;
  if (op == "+") {
    //value = 0;
    for (int i = 0; i < e.elems.size(); i++) {
      elem e2 = lreduce(op,acum,e.elems[i]);
    }
  }
  else if (op == "-") {
    //value = 0;
    for (int i = 0; i < e.elems.size(); i++) {
      elem e2 = lreduce(op,acum,e.elems[i]);
      //if (e2.hasValue)
      //value -= e2.value;
    }
  }
  else if (op == "*") {
    //value = 1;
    for (int i = 0; i < e.elems.size(); i++) {
      elem e2 = lreduce(op,acum,e.elems[i]);
      //if (e2.hasValue)
      //value *= e2.value;
    }
  }
  elem e1;
  e1.isNull = false;
  e1.hasValue = true;
  e1.value = acum;
  e.elems = vector<elem>(1,e1);
  return e;
}

elem lfilter (string op, int x, elem  e) {
  if (e.isNull)
  return e;
  if (not e.isNull and e.hasValue) {
    bool passesFilter = false;
    if (op == "==") {
      if (e.value == x)
      passesFilter = true;
    }
    else if (op == "!=") {
      if (e.value != x)
      passesFilter = true;
    }
    else if (op == "<") {
      if (e.value < x)
      passesFilter = true;
    }
    else {
      if (e.value > x)
      passesFilter = true;
    }
    if (not passesFilter) {
      e.isNull = true;
      e.hasValue = false;
    }
    return e;
  }
  for (int i = 0; i < e.elems.size(); i++) {
    e.elems[i] = lfilter(op,x,e.elems[i]);
    
    }
  for (int i = 0; i < e.elems.size(); i++) {
    if (e.elems[i].isNull) {
      e.elems.erase(e.elems.begin() + i);
      i--;
    }
    
    }
  return e;
}


elem evaluate(AST *a) {
  if (a == NULL) {
    elem e;
    e.isNull = true;
    e.hasValue = false;
    return e;
  }
  else if (a->kind == "lmap") {
    string op = child(a,0)->kind;
    int x = stoi(child(a,1)->text);
    // nomes sobre identificadors
    return lmap(op,x,symtab[child(a,2)->text]);
  }
  else if (a->kind == "lreduce") {
    string op = child(a,0)->kind;
    // nomes sobre identificadors
    int acum = 0;
    if (op == "+" or op == "-")
    return lreduce(op,acum,symtab[child(a,1)->text]);
    else if (op == "*") {
      int acum = 1;
      return lreduce(op,acum,symtab[child(a,1)->text]);
    }
  }
  else if (a->kind == "lfilter") {
    string op = child(a,0)->kind;
    int x = stoi(child(child(a,0),0)->text);
    // nomes sobre identificadors
    return lfilter(op,x,symtab[child(a,1)->text]);
  }
  else if (a->kind == "head") {
    return head(symtab[child(a,0)->text]);
  }
  else if (a->kind == "#") { // nomes sobre identificadors, assoc esquerra
    return concat(symtab[child(a,0)->text],symtab[child(a,1)->text]);
  }
  else if (a->kind == "[") {
    elem e;
    e.isNull = false;
    e.hasValue = false;
    int childN = 0;
    bool hasChildren = (child(a,childN) != NULL);
    while (hasChildren) {
      e.elems.push_back(evaluate(child(a,childN)));
      childN++;
      hasChildren = (child(a,childN) != NULL);
    }
    return e;
  }
  else if (a->kind == "id") {
    return symtab[a->text];
  }
  else if (a->kind == "intconst") {
    elem e;
    e.isNull = false;
    e.hasValue = true;
    e.value = stoi(a->text); // c++ 11 needed
    return e;
  }
}
/// todo: unificar comparacions?
bool equalLists (elem e1, elem e2) { // recordem que hem assumit que nomes s aplica a llistes aplanades
  ///return evaluate(a1) == evaluate (a2);
  if (e1.elems.size() != e2.elems.size())
  return false;
  for (int i = 0; i < e1.elems.size(); i++) {
    if (e1.elems[i].value != e2.elems[i].value)
    return false;
  }
  return true;
  
}


bool lesserThanLists (elem e1, elem e2) { // asumim nomes aplanades
  if (e1.elems.size() == 0 and e2.elems.size() != 0)
  return true;
  if (e2.elems.size() == 0)
  return false;
  bool atLeastOneStrictlyLesser = false;
  for (int i = 0; i < e1.elems.size(); i++) {
    if (i > e2.elems.size()-1)
    return false;
    if (e1.elems[i].value > e2.elems[i].value)
    return false;
    else if (not atLeastOneStrictlyLesser and e1.elems[i].value < e2.elems[i].value)
    atLeastOneStrictlyLesser = true;
  }
  return atLeastOneStrictlyLesser;
}

bool greaterThanLists (elem e1, elem e2) { // asumim nomes aplanades
  if (e1.elems.size() == 0)
  return false;
  if (e1.elems.size() > 0 and e2.elems.size() == 0)
  return true;
  bool atLeastOneStrictlyGreater = false;
  for (int i = 0; i < e1.elems.size(); i++) {
    if (i > e2.elems.size()-1)
    return true;
    if (e1.elems[i].value < e2.elems[i].value)
    return false;
    else if (not atLeastOneStrictlyGreater and e1.elems[i].value > e2.elems[i].value)
    atLeastOneStrictlyGreater = true;
  }
  return atLeastOneStrictlyGreater;
}

bool evaluateBool(AST *a) {
  if (a->kind == "not")
  return not evaluateBool(child(a,0));
  else if (a->kind == "empty") { // assumim que nomes per identificadors
    ///if (symtab[child(a,0)->text].elems.size() == 0)
    if (evaluate(child(a,0)).elems.size() == 0)
    return true;
    else
    return false;
  }
  else if (a->kind == "and") {
    return evaluateBool(child(a,0)) and evaluateBool(child(a,1));
  }
  else if (a->kind == "or") {
    return evaluateBool(child(a,0)) or evaluateBool(child(a,1));
  }
  else if (a->kind == "==") {
    return equalLists(evaluate(child(a,0)),evaluate(child(a,1)));
  }
  else if (a->kind == ">") {
    return greaterThanLists(evaluate(child(a,0)),evaluate(child(a,1)));
  }
  else if (a->kind == "<") {
    return greaterThanLists(evaluate(child(a,0)),evaluate(child(a,1)));
  }
  else if (a->kind == "<=") {
    return lesserThanLists(evaluate(child(a,0)),evaluate(child(a,1))) or equalLists(evaluate(child(a,0)),evaluate(child(a,1)));
  }
  else if (a->kind == ">=") {
    return greaterThanLists(evaluate(child(a,0)),evaluate(child(a,1))) or equalLists(evaluate(child(a,0)),evaluate(child(a,1)));
  }
  else if (a->kind == "!=") {
    return not equalLists(evaluate(child(a,0)),evaluate(child(a,1)));
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
    cout << elemToString(evaluate(child(a,0))) << endl;
    execute(a->right);
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
  }
  else if (a->kind == "flatten") {
    symtab[child(a,0)->text] = flatten(symtab[child(a,0)->text]);
    execute(a->right);
  }
  else if (a->kind == "pop") {
    symtab[child(a,0)->text] = pop(symtab[child(a,0)->text]);
    execute(a->right);
  }
  else if (a->kind == "if") {
    if (evaluateBool(child(a,0)))
    execute(child(a,1));
    /// assumim que en el llenguatge no hi ha else
    execute(a->right);
  }
  else if (a->kind == "while") {
    while (evaluateBool(child(a,0)))
    execute(child(a,1));
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
  condition2(zzSTR); zzlink(_root, &_sibling, &_tail);
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (LA(1)==OR_KEYWORD) ) {
      zzmatch(OR_KEYWORD); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
      condition2(zzSTR); zzlink(_root, &_sibling, &_tail);
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
  zzresynch(setwd2, 0x10);
  }
}

void
#ifdef __USE_PROTOS
condition2(AST**_root)
#else
condition2(_root)
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
    while ( (LA(1)==AND_KEYWORD) ) {
      zzmatch(AND_KEYWORD); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
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
    if ( (setwd2[LA(1)]&0x40) ) {
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
            else {zzFAIL(1,zzerr4,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
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
                else {zzFAIL(1,zzerr5,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
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
      else {zzFAIL(1,zzerr6,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
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
  zzresynch(setwd2, 0x80);
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
      if ( (setwd3[LA(1)]&0x1) ) {
      }
      else {zzFAIL(1,zzerr7,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
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
        if ( (setwd3[LA(1)]&0x2) ) {
          {
            zzBLOCK(zztasp4);
            zzMake0;
            {
            list_relational(zzSTR); zzlink(_root, &_sibling, &_tail);
            zzEXIT(zztasp4);
            }
          }
        }
        else {
          if ( (LA(1)==OPAR) ) {
            {
              zzBLOCK(zztasp4);
              zzMake0;
              {
              zzmatch(OPAR);  zzCONSUME;
              condition(zzSTR); zzlink(_root, &_sibling, &_tail);
              zzmatch(CPAR);  zzCONSUME;
              zzEXIT(zztasp4);
              }
            }
          }
          else {zzFAIL(1,zzerr8,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
        }
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
  zzresynch(setwd3, 0x4);
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
  zzresynch(setwd3, 0x8);
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
          else {
            if ( (LA(1)==GEQUAL) ) {
              zzmatch(GEQUAL); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
            }
            else {
              if ( (LA(1)==LEQUAL) ) {
                zzmatch(LEQUAL); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
              }
              else {zzFAIL(1,zzerr9,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
            }
          }
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
  zzresynch(setwd3, 0x10);
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
  zzresynch(setwd3, 0x20);
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
      else {zzFAIL(1,zzerr10,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
  }
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
  zzresynch(setwd3, 0x80);
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
  zzresynch(setwd4, 0x1);
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
  zzresynch(setwd4, 0x2);
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
        else {zzFAIL(1,zzerr11,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
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
  zzresynch(setwd4, 0x4);
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
          else {zzFAIL(1,zzerr12,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
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
  zzresynch(setwd4, 0x8);
  }
}
