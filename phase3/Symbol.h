#ifndef SYMBOL_H
#define SYMBOL_H
#include "Type.h"
typedef std::string string;

class Symbol{
public:
  Type *_type;
  string _name;
  bool _defined;

  Symbol(Type *type, string name, bool defined);
  
};

#endif /* SYMBOL_H */
