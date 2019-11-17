#include <cstdlib>
#include <cassert>
#include <string>
#include <iostream>
#include "Scope.h"
#include "Type.h"
#include "Symbol.h"
using namespace std;

Scope::Scope(Scope *enclosing=nullptr){
  _enclosing = enclosing;
}

void Scope::insert(Symbol *symbol){
  assert(find(symbol->_name) == nullptr);
  _symbols.push_back(symbol);
}

void Scope::remove(const string & name){
  for(Symbols::iterator it = _symbols.begin(); it != _symbols.end() ; ++it)
    if((*it)->_name == name)
  {
      _symbols.erase(it);
      return;
  }
}

Symbol * Scope::find(const string & name) const{
  for(Symbols::const_iterator it = _symbols.begin(); it != _symbols.end(); ++it)
    if((*it)->_name == name)
      return *it;
  return nullptr;
}

Symbol * Scope::lookup(const string &name) const{
  Symbol *syp = find(name);
  if(syp == nullptr && _enclosing != nullptr)
    return _enclosing->lookup(name);
  return syp;
}

Scope * Scope::enclosing() const{
  return _enclosing;
}

const Symbols & Scope::symbols() const{
  return _symbols;
}
