#ifndef tokens_h
#define tokens_h
/* tokens.h -- List of labelled tokens and stuff
 *
 * Generated from: numlistlang2_def2.g
 *
 * Terence Parr, Will Cohen, and Hank Dietz: 1989-2001
 * Purdue University Electrical Engineering
 * ANTLR Version 1.33MR33
 */
#define zzEOF_TOKEN 1
#define ASSIGNMENT 2
#define IF_KEYWORD 3
#define WHILE_KEYWORD 4
#define OPAR 5
#define CPAR 6
#define THEN_KEYWORD 7
#define ENDIF_KEYWORD 8
#define DO_KEYWORD 9
#define ENDWHILE_KEYWORD 10
#define FLATTEN_KEYWORD 11
#define PRINT_KEYWORD 12
#define LREDUCE_KEYWORD 13
#define LMAP_KEYWORD 14
#define LFILTER_KEYWORD 15
#define HEAD_KEYWORD 16
#define POP_KEYWORD 17
#define EMPTY_KEYWORD 18
#define SUM 19
#define MINUS 20
#define MUL 21
#define EQUAL 22
#define NEQUAL 23
#define GREATER 24
#define LESSER 25
#define GEQUAL 26
#define LEQUAL 27
#define NOT_KEYWORD 28
#define AND_KEYWORD 29
#define OR_KEYWORD 30
#define CONCATTAG 31
#define ID 32
#define NUM 33
#define OBRACKET 34
#define CBRACKET 35
#define COMMA 36
#define SPACE 37

#ifdef __USE_PROTOS
void lists(AST**_root);
#else
extern void lists();
#endif

#ifdef __USE_PROTOS
void list_oper(AST**_root);
#else
extern void list_oper();
#endif

#ifdef __USE_PROTOS
void if_oper(AST**_root);
#else
extern void if_oper();
#endif

#ifdef __USE_PROTOS
void while_oper(AST**_root);
#else
extern void while_oper();
#endif

#ifdef __USE_PROTOS
void assignment_oper(AST**_root);
#else
extern void assignment_oper();
#endif

#ifdef __USE_PROTOS
void flatten_oper(AST**_root);
#else
extern void flatten_oper();
#endif

#ifdef __USE_PROTOS
void print_oper(AST**_root);
#else
extern void print_oper();
#endif

#ifdef __USE_PROTOS
void pop_oper(AST**_root);
#else
extern void pop_oper();
#endif

#ifdef __USE_PROTOS
void list(AST**_root);
#else
extern void list();
#endif

#ifdef __USE_PROTOS
void condition(AST**_root);
#else
extern void condition();
#endif

#ifdef __USE_PROTOS
void condition2(AST**_root);
#else
extern void condition2();
#endif

#ifdef __USE_PROTOS
void list_val(AST**_root);
#else
extern void list_val();
#endif

#ifdef __USE_PROTOS
void boolean_expr(AST**_root);
#else
extern void boolean_expr();
#endif

#ifdef __USE_PROTOS
void empty(AST**_root);
#else
extern void empty();
#endif

#ifdef __USE_PROTOS
void list_relational(AST**_root);
#else
extern void list_relational();
#endif

#ifdef __USE_PROTOS
void unary(AST**_root);
#else
extern void unary();
#endif

#ifdef __USE_PROTOS
void lfunc(AST**_root);
#else
extern void lfunc();
#endif

#ifdef __USE_PROTOS
void lreduce(AST**_root);
#else
extern void lreduce();
#endif

#ifdef __USE_PROTOS
void lmap(AST**_root);
#else
extern void lmap();
#endif

#ifdef __USE_PROTOS
void lfilter(AST**_root);
#else
extern void lfilter();
#endif

#ifdef __USE_PROTOS
void arithmetic(AST**_root);
#else
extern void arithmetic();
#endif

#ifdef __USE_PROTOS
void relational(AST**_root);
#else
extern void relational();
#endif

#endif
extern SetWordType zzerr1[];
extern SetWordType setwd1[];
extern SetWordType zzerr2[];
extern SetWordType zzerr3[];
extern SetWordType zzerr4[];
extern SetWordType zzerr5[];
extern SetWordType zzerr6[];
extern SetWordType setwd2[];
extern SetWordType zzerr7[];
extern SetWordType zzerr8[];
extern SetWordType zzerr9[];
extern SetWordType zzerr10[];
extern SetWordType setwd3[];
extern SetWordType zzerr11[];
extern SetWordType zzerr12[];
extern SetWordType setwd4[];
