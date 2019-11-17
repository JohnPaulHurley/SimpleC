#include <iostream>
#include <cstdlib>
#include "checker.h"
#include "Symbol.h"
#include "Scope.h"
#include "Type.h"
#include "lexer.h"
using namespace std;


Scope *globalScope=nullptr;
Scope *currentScope=nullptr;

void openScope(){
  cout << "Opening scope" << endl;
  if(globalScope == nullptr){
    globalScope = new Scope(nullptr);
    currentScope = globalScope;
  }
  else
    currentScope = new Scope(currentScope);
}

void closeScope(){
  cout << "Closing scope" << endl;
  currentScope = currentScope -> enclosing();
  if(currentScope == nullptr)
    globalScope = nullptr;
}

void vDec(int spec, string name, string size, unsigned ind){
  cout << "Declaring " << spec;
  if(stoul(size) == 0){
    cout << " SCALAR";
    Type *T = new Type(spec,ind);
    Symbol *sym = new Symbol(T,name,false);
    Symbol *s;
    if(currentScope == globalScope){
      if((s=globalScope->find(name)) != nullptr){
        if(*s->_type != *T)
        {
          cout << *s->_type << endl;
          cout << *T << endl;
          report("conflicting types for '%s'",name);
        }
      }
      else
        currentScope->insert(sym);
      
    }
    else{
      if((s=currentScope->find(name)) != nullptr){
        report("redeclaration of '%s'",name);
      }
      else
        currentScope->insert(sym);
    }
  }
  else{
    Type *T = new Type(spec,ind,stoul(size));
    Symbol *sym = new Symbol(T,name,false);
    Symbol *s;
    if(currentScope == globalScope){
      if((s=globalScope->find(name)) != nullptr){
        if(*s->_type != *T)
          report("conflicting types for '%s'",name);
      }
      else
        currentScope->insert(sym);
    }
    else{
      if((s=currentScope->find(name)) != nullptr){
        report("redeclaration of '%s'",name);
      }
      else
        currentScope->insert(sym);
    }
    cout << " ARRAY of size " << size;
  }
  cout << " named " << name << " with " << ind << "levels of indirection" << endl;
}

void fDec(int spec, string name, unsigned ind){
  cout << "Declaring FUNCTION " << name << " with " << ind << " levels of indirection, which returns " << spec << endl;  
  Type *T = new Type(spec,ind,nullptr);
  Symbol *sym = new Symbol(T,name,false);
  Symbol *s;
  if((s=globalScope->find(name)) != nullptr){
    if(*s->_type != *T)
      report("conflicting types for '%s'",name);
  }
  else{
    globalScope->insert(sym);
  }
}

void fDef(int spec, string name, unsigned ind, Parameters * params){
  cout << "Defining FUNCTION " << name << " with " << ind << " levels of indirection which takes **params** and returns " << spec << endl;
  Type *T = new Type(spec,ind,params);
  Symbol *sym = new Symbol(T,name,true);
  Symbol *s;

  if((s=globalScope->find(name)) != nullptr){
    if(s->_defined == true)
      report("redefinition of '%s'",name);
    else if(*(s->_type) != *T){
      report("conflicting types for '%s'",name);
      cout << *s->_type << endl;
      cout << *T << endl;
    }
    globalScope->remove(name);
  }
  
  globalScope->insert(sym);
  cout << "after" << endl;
}

void sUse(string name){
  if(currentScope->find(name) == nullptr && currentScope->lookup(name) == nullptr)
      report("'%s' undeclared",name);
  
  cout << "Using identifier " << name << endl;
}
