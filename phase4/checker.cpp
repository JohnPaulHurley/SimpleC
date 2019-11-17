/*
 * File:	checker.cpp
 *
 * Description:	This file contains the public and private function and
 *		variable definitions for the semantic checker for Simple C.
 *
 *		Extra functionality:
 *		- inserting an undeclared symbol with the error type
 */

# include <iostream>
# include "lexer.h"
# include "checker.h"
# include "tokens.h"
# include "Symbol.h"
# include "Scope.h"
# include "Type.h"


using namespace std;

Type returnType;

static Scope *outermost, *toplevel;
static const Type error;

static string redefined = "redefinition of '%s'";
static string redeclared = "redeclaration of '%s'";
static string conflicting = "conflicting types for '%s'";
static string undeclared = "'%s' undeclared";
static string e1 = "invalid return type";
static string e2 = "invalid type for test expression";
static string e3 = "lvalue required in expression";
static string e4 = "invalid operands to binary %s";
static string e5 = "invalid operand to unary %s";
static string e6 = "invalid operand in cast expression";
static string e7 = "invalid operand in sizeof expression";
static string e8 = "called object is not a function";
static string e9 = "invalid arguments to called function";

/*bool operator==(const vector<Type> &lhs, const vector<Type> &rhs){
  if(lhs.size() != rhs.size())
    return false;
  vector<Type>::const_iterator it1,it2;
  it1 = lhs.begin();
  it2 = rhs.begin();
  while( it1 != lhs.end() && it2 != rhs.end())
    if(!(*it1++).isCompatible(*it2++))
      return false;
  return true;
}
*/
/*
 * Function:	openScope
 *
 * Description:	Create a scope and make it the new top-level scope.
 */

Scope *openScope()
{
    toplevel = new Scope(toplevel);

    if (outermost == nullptr)
	outermost = toplevel;

    return toplevel;
}


/*
 * Function:	closeScope
 *
 * Description:	Remove the top-level scope, and make its enclosing scope
 *		the new top-level scope.
 */

Scope *closeScope()
{
    Scope *old = toplevel;

    toplevel = toplevel->enclosing();
    return old;
}


/*
 * Function:	defineFunction
 *
 * Description:	Define a function with the specified NAME and TYPE.  A
 *		function is always defined in the outermost scope.  This
 *		definition always replaces any previous definition or
 *		declaration.
 */

Symbol *defineFunction(const string &name, const Type &type)
{
    Symbol *symbol = outermost->find(name);

    if (symbol != nullptr) {
	if (symbol->type().isFunction() && symbol->type().parameters()) {
	    report(redefined, name);
	    delete symbol->type().parameters();

	} else if (type != symbol->type())
	    report(conflicting, name);

	outermost->remove(name);
	delete symbol;
    }

    symbol = new Symbol(name, type);
    outermost->insert(symbol);
    
    returnType = Type(type.specifier(),type.indirection());

    return symbol;
}


/*
 * Function:	declareFunction
 *
 * Description:	Declare a function with the specified NAME and TYPE.  A
 *		function is always declared in the outermost scope.  Any
 *		redeclaration is discarded.
 */

Symbol *declareFunction(const string &name, const Type &type)
{
    Symbol *symbol = outermost->find(name);

    if (symbol == nullptr) {
	symbol = new Symbol(name, type);
	outermost->insert(symbol);

    } else if (type != symbol->type()) {
	report(conflicting, name);
	delete type.parameters();
    }

    return symbol;
}


/*
 * Function:	declareVariable
 *
 * Description:	Declare a variable with the specified NAME and TYPE.  Any
 *		redeclaration is discarded.
 */

Symbol *declareVariable(const string &name, const Type &type)
{
    Symbol *symbol = toplevel->find(name);

    if (symbol == nullptr) {
	symbol = new Symbol(name, type);
	toplevel->insert(symbol);

    } else if (outermost != toplevel)
	report(redeclared, name);

    else if (type != symbol->type())
	report(conflicting, name);

    return symbol;
}


/*
 * Function:	checkIdentifier
 *
 * Description:	Check if NAME is declared.  If it is undeclared, then
 *		declare it as having the error type in order to eliminate
 *		future error messages.
 */

Symbol *checkIdentifier(const string &name)
{
    Symbol *symbol = toplevel->lookup(name);

    if (symbol == nullptr) {
	report(undeclared, name);
	symbol = new Symbol(name, error);
	toplevel->insert(symbol);
    }

    return symbol;
}

Type checkUnary(const Type &l,const string &op,bool lval){
  if(l == error)
    return error; 
  Type T = l.promote();
  if(op == "*"){
    if(T.indirection() == 0){
      report(e5,op);
      return Type(); 
    }
    return Type(T.specifier(),T.indirection()-1); 
  }
  if(op == "&"){
    if(lval == false){
      report(e3);
      return Type();
    }
    return Type(T.specifier(),T.indirection()+1);  
  }
  if(op == "!"){
    if(!T.isLogical()){
      report(e5,op);
      return Type();
    }
    return Type(INT);
  } 
  if(op == "-"){
    if(!T.isNumeric()){
      report(e5,op);
      return Type();
    }
    return T;
  }
  if(op == "sizeof")
  {
    if(T.isFunction()){
      report(e7);
      return Type();
    }
    return Type(LONG);
  }
  return Type();
}

Type checkPost(const Type &l,const Type &r){
  if(l == error || r == error)
    return error;
  Type T = l.promote();
  if(T.indirection() == 0 || !r.isNumeric()){
    report(e4,"[]");
    return Type();
  }
  return Type(l.specifier(),T.indirection()-1);
}

Type checkFun(const Type &function,const Parameters &params){
  if(function == error)
    return error;

  if(!function.isFunction()){
    report(e8);
    return Type();
  }

  if(function.parameters()== nullptr)
  return Type(function.specifier(), function.indirection());
 
 if(function.parameters()->size() != params.size()){
    report(e9);
    return Type();
  }

 for(int i = 0; i< params.size(); i++)
  if(!(*function.parameters())[i].promote().isCompatible(params[i].promote()))
  {
    report(e9);
    return Type();
  }
 
    return Type(function.specifier(),function.indirection());
  
}

Type checkLogical(const Type &l,const Type &r,const string &op){
  if(l == error || r == error)
    return error;

  Type T,U;
  T = l.promote();
  U = r.promote();
  
  if(!T.isLogical() || !U.isLogical()){
    report(e4,op);
    return Type();
  }

  return Type(INT);
}

Type checkLogical(const Type &T){
  if(T == error)
    return error;
  if(T.isLogical())
    return T;
  report(e2);
  return Type();
}

Type checkEquality(const Type &l,const Type &r, const string &op){
  if(l==error || r==error)
    return error;

  Type T,U;
  T = l.promote();
  U = r.promote();

  if(T.isCompatible(U))
    return Type(INT);
  report(e4,op);
  return Type();
}

Type checkRelational(const Type &l, const Type &r, const string &op){
  if(l==error || r==error)
    return error;

  Type T,U;
  T = l.promote();
  U = r.promote();

  if(T.isCompatible(U))
    return Type(INT);
  report(e4,op);
  return Type();
}

Type checkMultiplicative(const Type &l,const Type &r, const string &op){
  if(l == error || r == error)
    return error;

  Type T,U;
  T = l.promote();
  U = r.promote();
  
  if(T.isNumeric() && U.isNumeric()){
    if(T.specifier() == LONG || U.specifier() == LONG)
      return Type(LONG);
    return Type(INT);
  }
  report(e4,op);
  return Type();
}

Type checkCast(const Type &l, const Type &r){
  if(l==error || r == error)
    return error;
  Type T = l.promote();
  Type U = r.promote();
  if((T.isNumeric() && U.isNumeric()) || (T.indirection() > 0 && U.indirection() > 0))
    return T;
  if((T.indirection() > 0 && U.specifier() == LONG) || (U.indirection() > 0 && T.specifier() == LONG))
    return T;
  report(e6);
  return Type();
}

Type checkAdditive(const Type &l, const Type &r, const std::string &op){
  
  if(l == error || r == error)
    return error;
  Type T,U;
  T = l.promote();
  U = r.promote();
  
  if(T.isNumeric() && U.isNumeric()){
    if(T.specifier() == LONG || U.specifier() == LONG)
      return Type(LONG);
    else
      return Type(INT);
  }
  if(T.indirection() > 0 && U.isNumeric())
    return T;
  if(op == "+"){
    if(T.isNumeric() && U.indirection() > 0)
      return U;
  }
  else
    if(T.specifier() == U.specifier() && T.indirection() > 0 && U.indirection()>0)
      return Type(LONG);

  report(e4,op);
  return Type();
  
}

Type checkEq(const Type &T, const Type &U, const bool &lval){
  if(T == error || U == error)
    return error;
  if(lval == false){
    report(e3);
    return Type();
  }
  if(!T.isCompatible(U)){
    report(e4,"=");
    return Type();
  }
  cout << T << ' ' << U << endl;
  return T;
}

Type checkReturn(const Type &T){
  if(T == error)
    return error;
  if(T.isCompatible(returnType))
    return T;
  report(e1);
  return Type();
}

