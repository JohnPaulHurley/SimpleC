#include <iostream>
#include <cstdlib>
#include <string>
#include "tokens.h"
#include "lexer.h"

using namespace std;
void expOR();
void ap();
void expAND();
void bp();
void expEQ();
void cp();
void expComp();
void dp();
void expSum();
void ep();
void expProd();
void fp();
void expParen();
void expBrack();
void gp();
void expUN();
void iD();
void specifier();
void pointers();
void exp_list();
void tUnit();
void fog();
void gDeclr();
void params();
void pList();
void param();
void declns();
void decln();
void declrList();
void declr();
void statements();
void statement();
void rdecls();
string buf, nextbuf;
int lookahead, nexttoken;
void errorexit(int token, string location){
  report("Was expecting a(n) " + to_string(token) + " in " + location + " but got " + buf + " " + to_string(lookahead),"");
  exit(255); 	
}
int peek(){
  if(!nexttoken)
    nexttoken = lexan(nextbuf);
  return nexttoken;
}
void match(int t){
  if(lookahead != t){
    report("Was expecting a match but failed","");
    exit(255);
  }
  if(nexttoken){
    lookahead = nexttoken;
    buf = nextbuf;
    nexttoken = 0;
  }
  else
    lookahead = lexan(buf);
}
void tUnit(){
   while(lookahead == INT || lookahead == LONG || lookahead == CHAR)
    fog();
}
void gDeclr(){
  pointers();
  if(lookahead != ID)
    errorexit(ID,"gDeclr"); 
  
  match(ID);
  if((lookahead != LP) && (lookahead != LB))
    return;  
  if(lookahead == LP){
    match(LP);
    if(lookahead != RP)
      errorexit(RP,"gDeclr");
    match(RP);
    return;
  }
  match(LB);
  if(lookahead != NUM)
      errorexit(NUM,"gDeclr");
  match(NUM);
  if(lookahead != RB)
      errorexit(RB,"gDeclr"); 
  match(RB);
}
void rdecls(){
  
  while(lookahead == COMMA){
    match(COMMA);
    gDeclr();
  }
  if(lookahead != SEMI)
      errorexit(SEMI,"rdecls");   
  match(SEMI);
}
void fog(){
  specifier();
  pointers();
  if(lookahead != ID)
    errorexit(ID,"fog");
  match(ID);
  if(lookahead == LP){
    match(LP);
    if(lookahead == RP){
      match(RP);
      rdecls();
      return;
    }
    params();
    if(lookahead != RP)
      errorexit(RP,"fog"); 
    match(RP);
    if(lookahead != LC)
      errorexit(LC,"fog");
    match(LC);
    declns();
    statements();
    if(lookahead != RC)
      errorexit(RC,"fog");   
    match(RC);
  }
  else if(lookahead == LB){
    match(LB);
    if(lookahead != NUM)
      errorexit(NUM,"fog"); 
    match(NUM);
    if(lookahead != RB)
      errorexit(RB,"fog");  
    match(RB);
    rdecls();
  }
  else
    rdecls();
  
}
void params(){
  if(lookahead == VOID){
    match(VOID);
    return;
  }
  pList();
}
void pList(){
  param();
  while(lookahead == COMMA){
    match(COMMA);
    param();
  }
  
}
void param(){
  specifier();
  pointers();
  if(lookahead != ID)
    errorexit(ID,"param"); 
  match(ID);
}
void declns(){
  while(lookahead == INT || lookahead == LONG || lookahead == CHAR)
    decln();
}
void decln(){
  specifier();
  declrList();
  if(lookahead != SEMI)
    errorexit(SEMI,"decln"); 
  match(SEMI);
}
void declrList(){
  declr();
  while(lookahead == COMMA){
    match(COMMA);
    declr();
  }
}
void declr(){
  pointers();
  if(lookahead != ID)
    errorexit(ID,"declr");
  match(ID);
  if(lookahead != LB)
    return;
  match(LB);
  if(lookahead != NUM)
    errorexit(NUM,"declr"); 
  match(NUM);
  if(lookahead != RB)
    errorexit(RB,"declr");
  match(RB);   
}
void statements(){
  while(lookahead != RC)
    statement();
}
void statement(){
  if(lookahead == LC){
    match(LC);
    declns();
    statements();
    if(lookahead != RC)
      errorexit(RC,"statement");
    match(RC);
  }
  else if(lookahead == RETURN){
    match(RETURN);
    expOR();
    if(lookahead != SEMI)
      errorexit(SEMI,"statement");
    match(SEMI);
  }
  else if(lookahead == WHILE){
    match(WHILE);
    if(lookahead != LP)
      errorexit(LP,"statement");
    match(LP);
    expOR();
    if(lookahead != RP)
      errorexit(RP,"statement");
    match(RP);
    statement();
  }
  else if(lookahead == IF){
    match(IF);
    if(lookahead != LP)
      errorexit(LP,"statement"); 
    match(LP);
    expOR();
    if(lookahead != RP)
      errorexit(RP,"statement");
    match(RP);
    statement();
    if(lookahead != ELSE)
      return;
    match(ELSE);
    statement();
  }
  else{
    expOR();
    if(lookahead == EQ){
      match(EQ);
      expOR();
      if(lookahead != SEMI)
	      errorexit(SEMI,"exp = exp");
      match(SEMI);
    }
    else{
      if(lookahead != SEMI)
	      errorexit(SEMI,"statement"); 
      match(SEMI);
    }
  }
}
void specifier(){
  if(lookahead == INT || lookahead == LONG || lookahead == CHAR)
    match(lookahead);
  else
    errorexit(INT + LONG + CHAR,"specifier");
}
void pointers(){
  if(lookahead == STAR){
    match(STAR);
    pointers();
  }
}
void exp_list(){
  expOR();
  if(lookahead == COMMA){
    match(COMMA);
    exp_list();
  }
}
void expOR(){
  expAND();
  ap();
}
void ap(){
  if(lookahead == OR){
    match(OR);
    expAND();
    cout << "or" << endl;
    ap();
  }
}
void expAND(){
  expEQ();
  bp();
}
void bp(){
  if(lookahead == AND){
    match(AND);
    expEQ();
    cout << "and" << endl;
    bp();
  }
  else;
}
void expEQ(){
  expComp();
  cp();
}
void cp(){
  int temp = lookahead;
  if(lookahead == EQQ || lookahead == NE){
    match(lookahead);
    expComp();
    if(temp == EQQ)
      cout << "eql" << endl;
    else
      cout << "neq" << endl;
    cp();

  }
  else;
}
void expComp(){
  expSum();
  dp();
}
void dp(){
  int temp = lookahead;
  if(lookahead == LT || lookahead == GT || lookahead == LTE || lookahead == GTE)
  {
    match(temp);
    expSum();
    if(temp == LT)
      cout << "ltn" << endl;
    else if(temp == GT)
      cout << "gtn" << endl;
    else if(temp == LTE)
      cout << "leq" << endl;
    else 
      cout << "geq" << endl;
    dp();

  }
  else;
}
void expSum(){
  expProd();
  ep();
}
void ep(){
  int temp = lookahead;
  if(lookahead == PLUS || lookahead == MINUS){
    match(lookahead);
    expProd();
    if(temp == PLUS)
      cout << "add" << endl;
    else
      cout << "sub" << endl;
    ep();

  }
  else;
}
void expProd(){
  expParen();
  fp();
}
void fp(){
  int temp = lookahead;
  if(lookahead == STAR || lookahead == DIV || lookahead == MOD){
    match(lookahead);
    expParen();
    if(temp == STAR)
      cout << "mul" << endl;
    else if (temp == DIV)
      cout << "div" << endl;
    else 
      cout << "rem" << endl;
    fp();

  }
  else;
}
void expParen(){
  peek();
  if((lookahead == LP) && (nexttoken == INT || nexttoken == CHAR || nexttoken == LONG)){
    match(LP);
    specifier();
    pointers();
    if(lookahead == RP){
      match(RP);
      expParen();
      cout << "cast" << endl;
    }
    else
      errorexit(RP,"expParen");
  }
  else
    expUN();
}
void expUN(){
  int temp = lookahead;
  if(lookahead == AMP || lookahead == STAR || 
      lookahead == BANG || lookahead == MINUS){
    match(lookahead);
    expUN();
    if (temp == AMP)
      cout << "addr" << endl;
    else if(temp == STAR)
      cout << "deref" << endl;
    else if(temp == BANG)
      cout << "not" << endl;
    else
      cout << "neg" << endl;
  }
  else if(lookahead == SIZEOF){
    match(SIZEOF);
    peek();
    if(lookahead == LP && (nexttoken == INT || nexttoken == CHAR || nexttoken == LONG))
    {
      match(LP);
      specifier();
      pointers();
      if(lookahead == RP)
        match(RP);
      else
	errorexit(RP,"expUN");
      cout << "sizeof" << endl;
    }
    else{
      expUN();
      cout << "sizeof" << endl;
    }
  }
  else
    expBrack();
}
void expBrack(){
  iD();
  gp();
}
void gp(){
  if(lookahead != LB)
    return;
  match(LB);
  expOR();
  if(lookahead != RB)
    errorexit(RB,"gp");
  match(RB);
  cout << "index" << endl;
  gp();
}
void iD(){
  if(lookahead == ID){
    match(ID);
    if(lookahead == LP){
      match(LP);
      if(lookahead == RP){
        match(RP);
        return;
      }
      exp_list();
      if(lookahead == RP){
        match(RP);
        return;
      }
      errorexit(RP,"iD expr list");
    }
    else
      return;
  }
  else if(lookahead == LP){
    match(LP);
    expOR();
    if(lookahead != RP)
      errorexit(RP,"(expr)");
    match(RP);
  }
  else if(lookahead == NUM)
    match(NUM);
  else if(lookahead == STRING)
    match(STRING);
  else
    errorexit(ID,"iD");

}
int main(){
  
  lookahead = lexan(buf);
  while(lookahead != DONE){
    tUnit();
  }
  return 0;
}
