#include <cstdlib>
#include "Symbol.h"

using namespace std;

Symbol::Symbol(Type *type, string name, bool defined){
  _type = type;
  _name = name;
  _defined = defined;
}
