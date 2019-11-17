/*
 * File:	checker.h
 *
 * Description:	This file contains the public function declarations for the
 *		semantic checker for Simple C.
 */

# ifndef CHECKER_H
# define CHECKER_H
# include "Scope.h"

Scope *openScope();
Scope *closeScope();

Symbol *defineFunction(const std::string &name, const Type &type);
Symbol *declareFunction(const std::string &name, const Type &type);
Symbol *declareVariable(const std::string &name, const Type &type);
Symbol *checkIdentifier(const std::string &name);

Type checkUnary(const Type &,const std::string &,bool i = false);
Type checkCast(const Type &, const Type &);
Type checkMultiplicative(const Type &,const Type &,const std::string &);
Type checkRelational(const Type &,const Type &, const std::string &);
Type checkEquality(const Type &,const Type &, const std::string &);
Type checkLogical(const Type &, const Type &, const std::string &);
Type checkLogical(const Type &);
Type checkAdditive(const Type &,const Type &, const std::string &);
Type checkPost(const Type &, const Type &);
Type checkFun(const Type &, const Parameters &);
Type checkEq(const Type &, const Type &, const bool &);
Type checkReturn(const Type &);
# endif /* CHECKER_H */
