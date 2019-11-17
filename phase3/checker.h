#ifndef CHECKER_H
#define CHECKER_H
#include <string>
#include "Symbol.h"

typedef std::string string;

void openScope();
void closeScope();
void vDec(int spec, string name, string size, unsigned ind);
void fDec(int spec, string name, unsigned ind);
void fDef(int spec, string name, unsigned ind, Parameters *params);
void sUse(string name);

#endif /* CHECKER_H */
