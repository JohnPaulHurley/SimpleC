/*
 * File:	parser.cpp
 *
 * Description:	This file contains the public and private function and
 *		variable definitions for the recursive-descent parser for
 *		Simple C.
 */

# include <cstdlib>
# include <iostream>
# include <vector>
# include "checker.h"
# include "tokens.h"
# include "lexer.h"

using namespace std;

static int lookahead, nexttoken;
static string lexbuf, nextbuf;

static Type expression(bool &), castExpression(bool &);
static Type statement();


/*
 * Function:	error
 *
 * Description:	Report a syntax error to standard error.
 */

static void error()
{
    if (lookahead == DONE)
	report("syntax error at end of file");
    else
	report("syntax error at '%s'", lexbuf);

    exit(EXIT_FAILURE);
}


/*
 * Function:	match
 *
 * Description:	Match the next token against the specified token.  A
 *		failure indicates a syntax error and will terminate the
 *		program since our parser does not do error recovery.
 */

static void match(int t)
{
    if (lookahead != t)
	error();

    if (nexttoken) {
	lookahead = nexttoken;
	lexbuf = nextbuf;
	nexttoken = 0;
    } else
	lookahead = lexan(lexbuf);
}


/*
 * Function:	peek
 *
 * Description:	Return the next token in the input stream and save it so
 *		that match() will later return it.
 */

static int peek()
{
    if (!nexttoken)
	nexttoken = lexan(nextbuf);

    return nexttoken;
}


/*
 * Function:	number
 *
 * Description:	Match the next token as a number and return its value.
 */

static unsigned long number()
{
    string buf;


    buf = lexbuf;
    match(NUM);
    return strtoul(buf.c_str(), NULL, 0);
}


/*
 * Function:	identifier
 *
 * Description:	Match the next token as an identifier and return its name.
 */

static string identifier()
{
    string buf;


    buf = lexbuf;
    match(ID);
    return buf;
}


/*
 * Function:	isSpecifier
 *
 * Description:	Return whether the given token is a type specifier.
 */

static bool isSpecifier(int token)
{
    return token == CHAR || token == INT || token == LONG;
}


/*
 * Function:	specifier
 *
 * Description:	Parse a type specifier.  Simple C has char, int, and long.
 *
 *		specifier:
 *		  char
 *		  int
 *		  long
 */

static int specifier()
{
    int typespec = ERROR;


    if (isSpecifier(lookahead)) {
	typespec = lookahead;
	match(lookahead);
    } else
	error();

    return typespec;
}


/*
 * Function:	pointers
 *
 * Description:	Parse pointer declarators (i.e., zero or more asterisks).
 *
 *		pointers:
 *		  empty
 *		  * pointers
 */

static unsigned pointers()
{
    unsigned count = 0;


    while (lookahead == '*') {
	match('*');
	count ++;
    }

    return count;
}


/*
 * Function:	declarator
 *
 * Description:	Parse a declarator, which in Simple C is either a scalar
 *		variable or an array, with optional pointer declarators.
 *
 *		declarator:
 *		  pointers identifier
 *		  pointers identifier [ num ]
 */

static void declarator(int typespec)
{
    unsigned indirection;
    string name;


    indirection = pointers();
    name = identifier();

    if (lookahead == '[') {
	match('[');
	declareVariable(name, Type(typespec, indirection, number()));
	match(']');

    } else
	declareVariable(name, Type(typespec, indirection));
}


/*
 * Function:	declaration
 *
 * Description:	Parse a local variable declaration.  Global declarations
 *		are handled separately since we need to detect a function
 *		as a special case.
 *
 *		declaration:
 *		  specifier declarator-list ;
 *
 *		declarator-list:
 *		  declarator
 *		  declarator , declarator-list
 */

static void declaration()
{
    int typespec;


    typespec = specifier();
    declarator(typespec);

    while (lookahead == ',') {
	match(',');
	declarator(typespec);
    }

    match(';');
}


/*
 * Function:	declarations
 *
 * Description:	Parse a possibly empty sequence of declarations.
 *
 *		declarations:
 *		  empty
 *		  declaration declarations
 */

static void declarations()
{
    while (isSpecifier(lookahead))
	declaration();
}


/*
 * Function:	primaryExpression
 *
 * Description:	Parse a primary expression.
 *
 *		primary-expression:
 *		  ( expression )
 *		  identifier ( expression-list )
 *		  identifier ( )
 *		  identifier
 *		  string
 *		  num
 *
 *		expression-list:
 *		  expression
 *		  expression , expression-list
 */

static Type primaryExpression(bool &lval)
{
  lval = false;
  Type T;
  Parameters Tv;
  string name; 
    if (lookahead == '(') {
	match('(');
	T = expression(lval);
	match(')');

    } else if (lookahead == STRING) {
	T = Type(CHAR,0,lexbuf.size() + 1);
  match(STRING);
  

    } else if (lookahead == NUM) {
  char c = *(lexbuf.end()-1);
  if( c=='l' || c=='L')
    T = Type(LONG);
  else{
    errno = 0;
    stoi(lexbuf.c_str(), NULL, 0);

    if (errno != 0)
      report("integer constant too large");

    T = Type(INT); 
	  match(NUM);
    
    }
  } 
  else if (lookahead == ID) {
    name = identifier();
	  T=checkIdentifier(name)->type();
 if(T.isScalar()) 
  lval = true; 
else
  lval = false;
	  if (lookahead == '(') {
      lval = false;
	    match('(');

	    if (lookahead != ')') {
		Tv.push_back(expression(lval));

		while (lookahead == ',') {
		    match(',');
		   Tv.push_back(expression(lval));
		}
	    }

	    match(')');
      T = checkFun(T,Tv);
	}

    } else
	error();

  return T;
}


/*
 * Function:	postfixExpression
 *
 * Description:	Parse a postfix expression.
 *
 *		postfix-expression:
 *		  primary-expression
 *		  postfix-expression [ expression ]
 */

static Type postfixExpression(bool &lval)
{
    Type l = primaryExpression(lval);
    Type r;
    while (lookahead == '[') {
	match('[');
	r = expression(lval);
	match(']');
  l = checkPost(l,r);
  lval = true;
    }
  return l;
}


/*
 * Function:	unaryExpression
 *
 * Description:	Parse a unary expression.
 *
 *		unary-expression:
 *		  postfix-expression
 *		  ! cast-expression
 *		  - cast-expression
 *		  * cast-expression
 *		  & cast-expression
 *		  sizeof unary-expression
 *		  sizeof ( specifier pointers )
 */

static Type unaryExpression(bool &lval)
{ 
    Type l;
    lval = false;
    if (lookahead == '!') {
	match('!');
	l = castExpression(lval);
  l = checkUnary(l,"!");

    } else if (lookahead == '-') {
	match('-');
	l = castExpression(lval);
  l = checkUnary(l,"-");

    } else if (lookahead == '*') {
	match('*');
	l = castExpression(lval);
  l = checkUnary(l,"*");
  lval = true;

    } else if (lookahead == '&') {
	match('&');
	l = castExpression(lval);
  l = checkUnary(l,"&",lval); 
  lval = false;
    } else if (lookahead == SIZEOF) {
	match(SIZEOF);

	if (lookahead == '(' && isSpecifier(peek())) {
	    match('(');
	    int spec = specifier();
	    unsigned ind = pointers();
	    match(')');
      l = Type(spec,ind);
      l = checkUnary(l,"sizeof");
	} else
	    l = unaryExpression(lval);

    } else
	    l = postfixExpression(lval);
    return l;
}


/*
 * Function:	castExpression
 *
 * Description:	Parse a cast expression.  If the token after the opening
 *		parenthesis is not a type specifier, we could have a
 *		parenthesized expression instead.
 *
 *		cast-expression:
 *		  unary-expression
 *		  ( specifier pointers ) cast-expression
 */

static Type castExpression(bool &lval)
{
  Type l,r;
    if (lookahead == '(' && isSpecifier(peek())) {
	match('(');
	int spec = specifier();
	unsigned ind = pointers();
  l = Type(spec,ind);
	match(')');
	r = castExpression(lval);
  l = checkCast(l,r);
  lval = false;
    } else
	  l = unaryExpression(lval);
  return l;
}


/*
 * Function:	multiplicativeExpression
 *
 * Description:	Parse a multiplicative expression.
 *
 *		multiplicative-expression:
 *		  cast-expression
 *		  multiplicative-expression * cast-expression
 *		  multiplicative-expression / cast-expression
 *		  multiplicative-expression % cast-expression
 */

static Type multiplicativeExpression(bool &lval)
{
    Type r = castExpression(lval);
    Type l;
    while (1) {
	if (lookahead == '*') {
	    match('*');
	    l = castExpression(lval);
      r = checkMultiplicative(l,r,"*");
      lval = false;

	} else if (lookahead == '/') {
	    match('/');
	    l = castExpression(lval);
      r = checkMultiplicative(l,r,"/");
      lval = false;

	} else if (lookahead == '%') {
	    match('%');
	    l = castExpression(lval);
      r = checkMultiplicative(l,r,"%");
      lval = false;

	} else
	    break;
    }
  return r;
}


/*
 * Function:	additiveExpression
 *
 * Description:	Parse an additive expression.
 *
 *		additive-expression:
 *		  multiplicative-expression
 *		  additive-expression + multiplicative-expression
 *		  additive-expression - multiplicative-expression
 */

static Type additiveExpression(bool &lval)
{
    Type l = multiplicativeExpression(lval);
    Type r;
    while (1) {
	if (lookahead == '+') {
	    match('+');
	    r = multiplicativeExpression(lval);
      l = checkAdditive(l,r,"+");
      lval = false;

	} else if (lookahead == '-') {
	    match('-');
	    r = multiplicativeExpression(lval);
      l = checkAdditive(l,r,"-");
      lval = false;

	} else
	    break;
    }
  return l;
}


/*
 * Function:	relationalExpression
 *
 * Description:	Parse a relational expression.  Note that Simple C does not
 *		have shift operators, so we go immediately to additive
 *		expressions.
 *
 *		relational-expression:
 *		  additive-expression
 *		  relational-expression < additive-expression
 *		  relational-expression > additive-expression
 *		  relational-expression <= additive-expression
 *		  relational-expression >= additive-expression
 */

static Type relationalExpression(bool &lval)
{
    Type l = additiveExpression(lval);
    Type r;
    while (1) {
	if (lookahead == '<') {
	    match('<');
	    r = additiveExpression(lval);
      l = checkRelational(l,r,"<");
      lval = false;

	} else if (lookahead == '>') {
	    match('>');
	    r = additiveExpression(lval);
      l = checkRelational(l,r,">");
      lval = false;

	} else if (lookahead == LEQ) {
	    match(LEQ);
	    r = additiveExpression(lval);
      l = checkRelational(l,r,"<=");
      lval = false;

	} else if (lookahead == GEQ) {
	    match(GEQ);
	    r = additiveExpression(lval);
      l = checkRelational(l,r,">=");
      lval = false;

	} else
	    break;
    }
  return l;
}


/*
 * Function:	equalityExpression
 *
 * Description:	Parse an equality expression.
 *
 *		equality-expression:
 *,lval		  relational-expression
    oype operator=(const Type &);
 *		  equality-expression == relational-expression
 *		  equality-expression != relational-expression
 */

static Type equalityExpression(bool &lval)
{
    Type l = relationalExpression(lval);
    Type r; 
    while (1) {
	if (lookahead == EQL) {
	    match(EQL);
	    r = relationalExpression(lval);
      l = checkEquality(l,r,"==");
      lval = false;

	} else if (lookahead == NEQ) {
	    match(NEQ);
	    r = relationalExpression(lval);
      l = checkEquality(l,r,"!=");
      lval = false;
	} else
	    break;
    }
    return l;
}


/*
 * Function:	logicalAndExpression
 *
 * Description:	Parse a logical-and expression.  Note that Simple C does
 *		not have bitwise-and expressions.
 *
 *		logical-and-expression:
 *		  equality-expression
 *		  logical-and-expression && equality-expression
 */

static Type logicalAndExpression(bool &lval)
{
    Type l = equalityExpression(lval);

    while (lookahead == AND) {
	match(AND);
	Type r = equalityExpression(lval);
  l = checkLogical(l,r,"&&");
  lval = false;
    }
  return l;
}


/*
 * Function:	expression
 *
 * Description:	Parse an expression, or more specifically, a logical-or
 *		expression, since Simple C does not allow comma or
 *		assignment as an expression operator.
 *
 *		expression:
 *		  logical-and-expression
 *		  expression || logical-and-expression
 */

static Type expression(bool &lval)
{
    Type l = logicalAndExpression(lval);

    while (lookahead == OR) {
	match(OR);
	Type r = logicalAndExpression(lval);
  l = checkLogical(l,r,"||");
  lval = false;
    }
  return l;
}


/*
 * Function:	statements
 *
 * Description:	Parse a possibly empty sequence of statements.  Rather than
 *		checking if the next token starts a statement, we check if
 *		the next token ends the sequence, since a sequence of
 *		statements is always terminated by a closing brace.
 *
 *		statements:
 *		  empty
 *		  statement statements
 */

static void statements()
{
    while (lookahead != '}')
	statement();
}


/*
 * Function:	statement
 *
 * Description:	Parse a statement.  Note that Simple C has so few
 *		statements that we handle them all in this one function.
 *
 *		statement:
 *		  { declarations statements }
 *		  return expression ;
 *		  while ( expression ) statement
 *		  if ( expression ) statement
 *		  if ( expression ) statement else statement
 *		  expression = expression ;
 *		  expression ;
 */

static Type statement()
{
  Type T,U;
  bool lval;
    if (lookahead == '{') {
	match('{');
	openScope();
	declarations();
	statements();
	closeScope();
	match('}');

    } else if (lookahead == RETURN) {
	match(RETURN);
	T = expression(lval);
  checkReturn(T);
	match(';');

    } else if (lookahead == WHILE) {
	match(WHILE);
	match('(');
	T = expression(lval);
  checkLogical(T);
	match(')');
	statement();
  

    } else if (lookahead == IF) {
	match(IF);
	match('(');
	T = expression(lval);
  checkLogical(T);
	match(')');
	statement();

	if (lookahead == ELSE) {
	    match(ELSE);
	    statement();
	}

    } else {
	T = expression(lval);

	if (lookahead == '=') {
	    match('=');
      //checkLval(T,lval);
      bool rvalue;
	    U = expression(rvalue);
      T = checkEq(T,U,lval);
	}

	match(';');
    }
  return T;
}


/*
 * Function:	parameter
 *
 * Description:	Parse a parameter, which in Simple C is always a scalar
 *		variable with optional pointer declarators.
 *
 *		parameter:
 *		  specifier pointers ID
 */

static Type parameter()
{
    unsigned indirection;
    int typespec;
    string name;


    typespec = specifier();
    indirection = pointers();
    name = identifier();

    Type type = Type(typespec, indirection);
    declareVariable(name, type);
    return type;
}


/*
 * Function:	parameters
 *
 * Description:	Parse the parameters of a function, but not the opening or
 *		closing parentheses.
 *
 *		parameters:
 *		  void
 *		  parameter-list
 *
 *		parameter-list:
 *		  parameter
 *		  parameter , parameter-list
 */

static Parameters *parameters()
{
    Parameters *params = new Parameters();


    if (lookahead == VOID)
	match(VOID);

    else {
	params->push_back(parameter());

	while (lookahead == ',') {
	    match(',');
	    params->push_back(parameter());
	}
    }

    return params;
}


/*
 * Function:	globalDeclarator
 *
 * Description:	Parse a declarator, which in Simple C is either a scalar
 *		variable, an array, or a function, with optional pointer
 *		declarators.
 *
 *		global-declarator:
 *		  pointers identifier
 *		  pointers identifier ( )
 *		  pointers identifier [ num ]
 */

static void globalDeclarator(int typespec)
{
    unsigned indirection;
    string name;


    indirection = pointers();
    name = identifier();

    if (lookahead == '(') {
	match('(');
	declareFunction(name, Type(typespec, indirection, nullptr));
	match(')');

    } else if (lookahead == '[') {
	match('[');
	declareVariable(name, Type(typespec, indirection, number()));
	match(']');

    } else
	declareVariable(name, Type(typespec, indirection));
}


/*
 * Function:	remainingDeclarators
 *
 * Description:	Parse any remaining global declarators after the first.
 *
 * 		remaining-declarators
 * 		  ;
 * 		  , global-declarator remaining-declarators
 */

static void remainingDeclarators(int typespec)
{
    while (lookahead == ',') {
	match(',');
	globalDeclarator(typespec);
    }

    match(';');
}


/*
 * Function:	globalOrFunction
 *
 * Description:	Parse a global declaration or function definition.
 *
 * 		global-or-function:
 * 		  specifier pointers identifier remaining-decls
 * 		  specifier pointers identifier [ num ] remaining-decls
 * 		  specifier pointers identifier ( ) remaining-decls 
 * 		  specifier pointers identifier ( parameters ) { ... }
 */

static void globalOrFunction()
{
    unsigned indirection;
    int typespec;
    string name;


    typespec = specifier();
    indirection = pointers();
    name = identifier();

    if (lookahead == '[') {
	match('[');
	declareVariable(name, Type(typespec, indirection, number()));
	match(']');
	remainingDeclarators(typespec);

    } else if (lookahead == '(') {
	match('(');

	if (lookahead == ')') {
	    declareFunction(name, Type(typespec, indirection, nullptr));
	    match(')');
	    remainingDeclarators(typespec);

	} else {
	    openScope();
	    defineFunction(name, Type(typespec, indirection, parameters()));
	    match(')');
	    match('{');
	    declarations();
	    statements();
	    closeScope();
	    match('}');
	}

    } else {
	declareVariable(name, Type(typespec, indirection));
	remainingDeclarators(typespec);
    }
}


/*
 * Function:	main
 *
 * Description:	Analyze the standard input stream.
 */

int main()
{
    openScope();
    lookahead = lexan(lexbuf);

    while (lookahead != DONE)
	globalOrFunction();

    closeScope();
    exit(EXIT_SUCCESS);
}
