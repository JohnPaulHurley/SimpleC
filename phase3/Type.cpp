#include <ostream>
#include <cassert>
#include "tokens.h"
#include "Type.h"

using namespace std;

Type::Type(int spec, unsigned ind, unsigned length) : _specifier(spec), _indirection(ind), _length(length) {
  _parameters = nullptr;
  _kind = ARRAY;
}

Type::Type(int spec, unsigned ind, Parameters *params) : _specifier(spec), _indirection(ind) {
  _parameters = params;
  _kind = FUNCTION;
}

Type::Type(int spec, unsigned ind) : _specifier(spec), _indirection(ind){
  _kind = SCALAR;
  _parameters = nullptr;
}

Type::Type(){
  _kind = ERROR;
}

bool Type::operator==(const Type &rhs) const {
  if(_kind != rhs._kind)
    return false;
  if(_specifier != rhs._specifier)
    return false;
  if(_indirection != rhs._indirection)
    return false;
  if(_kind == SCALAR)
    return true;
  if(_kind == ARRAY)
    return _length == rhs._length;
  if(!_parameters || !rhs._parameters)
    return true;
  return *_parameters == *rhs._parameters;
}

bool Type::operator!=(const Type &rhs) const{
  return ! (*this == rhs);
}

int Type::specifier() const{
  return _specifier;
}

unsigned Type::ind() const{
  return _indirection;
}

unsigned Type::length() const{
  assert(isArray());
  return _length;
}

Parameters * Type::parameters() const{
  assert(isFunction());
  return _parameters;
}

bool Type::isArray() const{
  return _kind == ARRAY;
}

bool Type::isFunction() const{
  return _kind == FUNCTION;
}

bool Type::isScalar() const{
  return _kind == SCALAR;
}

bool Type::isError() const{
  return _kind == ERROR;
}

ostream & operator<<(ostream &os, const Type & type){
  if(type.specifier() == CHAR)
    os << "char";
  else if(type.specifier() == INT)
    os << "int";
  else if(type.specifier() == LONG)
    os << "long";
  else 
    os << "error";

  if(type.ind() > 0){
    os << ' ';
    for(unsigned i =0; i<type.ind() ; ++i)
      os << '*';
  }
  if(type.isArray())
    os << '[' << type.length() << ']';
  else if(type.isFunction())
    os << "()";
  return os;
}
