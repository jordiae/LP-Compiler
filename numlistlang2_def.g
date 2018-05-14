#header
<<
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
#define createASTlist #0=new AST;(#0)->kind="list";(#0)->right=NULL;(#0)->down=_sibling;
>>



<<

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
#token GEQUAL ">="
#token LEQUAL "<="
#token NOT_KEYWORD "not"
#token AND_KEYWORD "and"
#token OR_KEYWORD "or"
#token CONCATTAG "#"
///#token ID "('a'..'z'|'A'..'Z') ('a'..'z'|'A'..'Z'|'0'..'9')*"
///#token ID "[a-zA-Z0-9]" // FALTA PROHIBIR QUE COMENCIN PER NUMNEROS?
///#token ID "[a-zA-Z]"
#token ID "[a-zA-Z][a-zA-Z0-9]*"
#token NUM "[0-9]+"
#token OBRACKET "\["
#token CBRACKET "\]"
#token COMMA ","
#token SPACE "[\ \n]" << zzskip(); >>

lists: (list_oper)* <<#0=createASTlist(_sibling);>> ;
list_oper: if_oper | while_oper | assignment_oper | flatten_oper | print_oper | pop_oper;
if_oper: IF_KEYWORD^ OPAR! condition CPAR! THEN_KEYWORD! lists ENDIF_KEYWORD!;
while_oper: WHILE_KEYWORD^ OPAR! condition CPAR! DO_KEYWORD! lists ENDWHILE_KEYWORD!;
assignment_oper: ID ASSIGNMENT^ list;
flatten_oper: FLATTEN_KEYWORD^ ID;
print_oper: PRINT_KEYWORD^ list;
pop_oper: POP_KEYWORD^ OPAR! list CPAR!;
list: list_val | ID ("@"! | (CONCATTAG^ ID) | ) | unary | lfunc;
condition: boolean_expr (binary_boolean_op boolean_expr)*;
binary_boolean_op: AND_KEYWORD^ | OR_KEYWORD^;
list_val: OBRACKET^ ( ((list_val|NUM) (COMMA! (list_val|NUM))*) | ) CBRACKET!;


boolean_expr: (NOT_KEYWORD^|) (((empty) | (list_relational) | (OPAR! condition CPAR!))); /// parentesis i tornar a comencar (revisar: todo)
empty: EMPTY_KEYWORD^ OPAR! ID CPAR!;

list_relational: list (EQUAL^ | NEQUAL^ | GREATER^ | LESSER^ | GEQUAL | LEQUAL) list;/// nomes aplanades en realitat...
unary: HEAD_KEYWORD^ OPAR! ID CPAR!;
lfunc: lreduce | lmap | lfilter;
lreduce: LREDUCE_KEYWORD^ arithmetic ID;
lmap: LMAP_KEYWORD^ arithmetic NUM ID;
lfilter: LFILTER_KEYWORD^ relational ID;
arithmetic: (SUM|MINUS|MUL); /// segons exemple NO creen arbre
relational: (EQUAL^|NEQUAL^|GREATER^|LESSER^) NUM;


/// todo: nots a relational (no de llistes)? mirar que amb ands i ors es facin be els arbres? multiples no's, i els seus arbres?
