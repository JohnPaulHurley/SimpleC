#ifndef TYPE_H
#define TYPE_H
#include <vector>
#include <ostream>

typedef std::vector<class Type> Parameters;

class Type{
  int _specifier;
  unsigned _indirection;
  enum {ARRAY, FUNCTION, SCALAR, ERROR} _kind;
  unsigned _length;
  Parameters *_parameters;

public:
  Type(int spec, unsigned ind, unsigned length);
  Type(int spec, unsigned ind, Parameters *params);
  Type(int spec, unsigned ind);
  Type();

  bool operator==(const Type &rhs) const;
  bool operator!=(const Type &rhs) const;
  
  int specifier() const;
  unsigned ind() const;
  unsigned length() const;
  Parameters *parameters() const;

  bool isArray() const;
  bool isFunction() const;
  bool isScalar() const; 
  bool isError() const; 
};

std::ostream & operator<<(std::ostream & os, const Type & type);

#endif /* TYPE_H */
