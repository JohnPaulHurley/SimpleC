/*
 * File:	parser.cpp
 *
 * Description:	This file contains the public and private function and
 *		variable definitions for the recursive-descent parser for
 *		Simple C.
 */

# include <cstdlib>
# include <iostream>
# include "checker.h"
# include "tokens.h"
# include "lexer.h"
# include "Type.h"

using namespace std;

static int lookahead, nexttoken;
static string lexbuf, nextbuf;

static void expression(), castExpression();
static void statement();


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
    int spec = lookahead;
    if (isSpecifier(lookahead))
	match(lookahead);
    else
	error();
  
    return spec;
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
    unsigned count=0;
    while (lookahead == '*'){
	    match('*');
      ++count;
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

static void declarator(int spec)
{
    unsigned ind;
    ind = pointers();
    string name = lexbuf;
    match(ID);

    if (lookahead == '[') {
	match('[');
  string size = lexbuf;
	match(NUM);
	match(']');
  vDec(spec,name,size,ind);
    }
    else
      vDec(spec,name, "0", ind);
    
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
    int spec;
    spec = specifier();
    declarator(spec);

    while (lookahead == ',') {
	match(',');
	declarator(spec);
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

static void primaryExpression()
{
    if (lookahead == '(') {
	match('(');
	expression();
	match(')');

    } else if (lookahead == STRING) {
	match(STRING);

    } else if (lookahead == NUM) {
	match(NUM);

    } else if (lookahead == ID) {
  sUse(lexbuf);
	match(ID);

	if (lookahead == '(') {
	    match('(');

	    if (lookahead != ')') {
		expression();

		while (lookahead == ',') {
		    match(',');
		    expression();
		}
	    }

	    match(')');
	}

    } else
	error();
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

static void postfixExpression()
{
    primaryExpression();

    while (lookahead == '[') {
	match('[');
	expression();
	match(']');
	cout << "index" << endl;
    }
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

static void unaryExpression()
{
    if (lookahead == '!') {
	match('!');
	castExpression();
	cout << "not" << endl;

    } else if (lookahead == '-') {
	match('-');
	castExpression();
	cout << "neg" << endl;

    } else if (lookahead == '*') {
	match('*');
	castExpression();
	cout << "deref" << endl;

    } else if (lookahead == '&') {
	match('&');
	castExpression();
	cout << "addr" << endl;

    } else if (lookahead == SIZEOF) {
	match(SIZEOF);

	if (lookahead == '(' && isSpecifier(peek())) {
	    match('(');
	    specifier();
	    pointers();
	    match(')');
	} else
	    unaryExpression();

	cout << "sizeof" << endl;

    } else
	postfixExpression();
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

static void castExpression()
{
    if (lookahead == '(' && isSpecifier(peek())) {
	match('(');
	specifier();
	pointers();
	match(')');
	castExpression();
	cout << "cast" << endl;

    } else
	unaryExpression();
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

static void multiplicativeExpression()
{
    castExpression();

    while (1) {
	if (lookahead == '*') {
	    match('*');
	    castExpression();
	    cout << "mul" << endl;

	} else if (lookahead == '/') {
	    match('/');
	    castExpression();
	    cout << "div" << endl;

	} else if (lookahead == '%') {
	    match('%');
	    castExpression();
	    cout << "rem" << endl;

	} else
	    break;
    }
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

static void additiveExpression()
{
    multiplicativeExpression();

    while (1) {
	if (lookahead == '+') {
	    match('+');
	    multiplicativeExpression();
	    cout << "add" << endl;

	} else if (lookahead == '-') {
	    match('-');
	    multiplicativeExpression();
	    cout << "sub" << endl;

	} else
	    break;
    }
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

static void relationalExpression()
{
    additiveExpression();

    while (1) {
	if (lookahead == '<') {
	    match('<');
	    additiveExpression();
	    cout << "ltn" << endl;

	} else if (lookahead == '>') {
	    match('>');
	    additiveExpression();
	    cout << "gtn" << endl;

	} else if (lookahead == LEQ) {
	    match(LEQ);
	    additiveExpression();
	    cout << "leq" << endl;

	} else if (lookahead == GEQ) {
	    match(GEQ);
	    additiveExpression();
	    cout << "geq" << endl;

	} else
	    break;
    }
}


/*
 * Function:	equalityExpression
 *
 * Description:	Parse an equality expression.
 *
 *		equality-expression:
 *		  relational-expression
 *		  equality-expression == relational-expression
 *		  equality-expression != relational-expression
 */

static void equalityExpression()
{
    relationalExpression();

    while (1) {
	if (lookahead == EQL) {
	    match(EQL);
	    relationalExpression();
	    cout << "eql" << endl;

	} else if (lookahead == NEQ) {
	    match(NEQ);
	    relationalExpression();
	    cout << "neq" << endl;

	} else
	    break;
    }
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

static void logicalAndExpression()
{
    equalityExpression();

    while (lookahead == AND) {
	match(AND);
	equalityExpression();
	cout << "and" << endl;
    }
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

static void expression()
{
    logicalAndExpression();

    while (lookahead == OR) {
	match(OR);
	logicalAndExpression();
	cout << "or" << endl;
    }
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

static void statement()
{
    if (lookahead == '{') {
  openScope();
	match('{');
	declarations();
	statements();
	match('}');
  closeScope();

    } else if (lookahead == RETURN) {
	match(RETURN);
	expression();
	match(';');

    } else if (lookahead == WHILE) {
	match(WHILE);
	match('(');
	expression();
	match(')');
	statement();

    } else if (lookahead == IF) {
	match(IF);
	match('(');
	expression();
	match(')');
	statement();

	if (lookahead == ELSE) {
	    match(ELSE);
	    statement();
	}

    } else {
	expression();

	if (lookahead == '=') {
	    match('=');
	    expression();
	}

	match(';');
    }
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

Type parameter()
{
    int spec = specifier();
    unsigned ind = pointers();
    string name = lexbuf;
    match(ID);
    
    vDec(spec,name,"0",ind);

    Type T(spec,ind);
    return T;
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

Parameters *parameters()
{ 
    Parameters *ans = new Parameters();
    if (lookahead == VOID)
	match(VOID);

    else {
	ans->push_back(parameter());

	while (lookahead == ',') {
	    match(',');
	    ans->push_back(parameter());
	}
    }
    
    return ans;
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

static void globalDeclarator(int spec)
{
    unsigned ind = pointers();
    string name = lexbuf;
    match(ID);

    if (lookahead == '(') {
	match('(');
	match(')');
  //cout << "declaring function " << name << ", returns " << spec << ' ' << ind << endl;
  fDec(spec,name,ind);
    } else if (lookahead == '[') {
	match('[');
  string size = lexbuf;
	match(NUM);
	match(']');
  //cout << "declaring " << spec << ' ' << ind  << " ARRAY " << name << " of size " << size << endl;
  vDec(spec,name,size,ind);
    }
    else{
      vDec(spec,name,"0",ind);
      //cout << "declaring" << spec << ' ' << ind << ' ' << name << endl;
    }
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

static void remainingDeclarators(int spec)
{
    while (lookahead == ',') {
	match(',');
	globalDeclarator(spec);
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
    int spec = specifier();
    unsigned ind = pointers();
    
    string name(lexbuf);
    
    match(ID);

    if (lookahead == '[') {
	match('[');
  string size = lexbuf;
	match(NUM);
	match(']');
  //cout << "declaring " << spec << ' ' << ind << "ARRAY " << name << " of size " << size << endl;
	vDec(spec,name,size,ind);
  
  remainingDeclarators(spec);

    } else if (lookahead == '(') {
	match('(');

	if (lookahead == ')') {
	    match(')');
      //cout << "declaring FUNCTION " << name << ", returns " << spec << ' ' <<ind << endl;
	    fDec(spec,name,ind);
      remainingDeclarators(spec);

	} else {
      openScope();
	    Parameters * params =  parameters();
	    match(')');
      fDef(spec,name,ind,params);
      //cout << "defining FUNCTION " << name << ", takes in **paramas**, returns" << spec << endl;
	    match('{');
	    declarations();
	    statements();
	    match('}');
      closeScope();
	}

    } else{
  vDec(spec,name,"0",ind);
  //cout << "declaring " << spec << ' ' << ind << ' ' << name << endl;
	remainingDeclarators(spec);
  }
}


/*
 * Function:	main
 *
 * Description:	Analyze the standard input stream.
 */

int main()
{
  lookahead = lexan(lexbuf);
    
    openScope();
  
    while (lookahead != DONE)
	globalOrFunction();

    closeScope();
  
    exit(EXIT_SUCCESS);

}
