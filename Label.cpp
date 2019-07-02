#include <iostream>
#include "Label.h"

using namespace std;

unsigned Label::_counter = 0;

Label::Label(){
  _number = _counter++;
}

unsigned Label::number() const{
  return _number;
}

std::ostream &operator <<(ostream &os, const Label &label){
  return os << ".L" << label.number();
}


