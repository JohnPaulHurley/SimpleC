#ifndef SCOPE_H
#define SCOPE_H
#include <cstdlib>
#include <vector>
#include "Symbol.h"

typedef std::vector<Symbol *> Symbols;


class Scope{
  typedef std::string string;
  
  Scope *_enclosing;
  Symbols _symbols;

public:
  Scope(Scope *enclosing);

  void insert(Symbol *symbol);
  void remove(const string & name);
  Symbol *find(const string & name) const;
  Symbol *lookup(const string & name) const;

  Scope *enclosing() const;
  const Symbols & symbols() const;
};


#endif /* SCOPE_H */
