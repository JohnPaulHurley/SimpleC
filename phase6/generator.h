/*
 * File:	generator.h
 *
 * Description:	This file contains the function declarations for the code
 *		generator for Simple C.  Most of the function declarations
 *		are actually member functions provided as part of Tree.h.
 */

# ifndef GENERATOR_H
# define GENERATOR_H
# include "Scope.h"
# include "Tree.h"
# include <iostream>
std::string suffix(unsigned int);
void generateGlobals(Scope *scope);
void assigntemp(class Expression *);
std::ostream &operator <<(std::ostream &ostr, Expression *expr);
# endif /* GENERATOR_H */
