/*
 * File:	Register.cpp
 *
 * Description:	This file contains the member function definitions for
 *		registers on the Intel 64-bit processor.
 */
# include <cassert>
# include <iostream>
# include "tokens.h"
# include "Tree.h"
# include "Register.h"
# include "generator.h"
using namespace std;

Register *rax = new Register("%rax","%eax","%al");
Register *rdi = new Register("%rdi","%edi","%dil");
Register *rsi = new Register("%rsi","%esi","%sil");
Register *rdx = new Register("%rdx","%edx","%dl");
Register *rcx = new Register("%rcx","%ecx","%cl");
Register *r8 = new Register("%r8","%r8d","%r8b");
Register *r9 = new Register("%r9","%r9d","%r9b");
Register *r10 = new Register("%r10","%r10d","%r10b");
Register *r11 = new Register("%r11","%r11d","%r11b");
Register *rbp = new Register("%rbp","%ebp","%bpl");
Register *rsp = new Register("%rsp","%esp","%spl");
std::vector<Register *> registers = {r11,r10,r9,r8,rcx,rdx,rsi,rdi,rax};

/*
 * Function:	Register::Register (constructor)
 *
 * Description:	Initialize this register with its correct operand names.
 */

Register::Register(const string &qword, const string &lword, const string &byte)
    : _qword(qword), _lword(lword), _byte(byte), _node(nullptr)
{
}


/*
 * Function:	Register::name
 *
 * Description:	Return the correct operand name given an access size.  The
 *		default is to return the 64-bit operand name.
 */

const string &Register::name(unsigned size) const
{
    return size == 1 ? _byte : (size == 4 ? _lword : _qword);
}


/*
 * Function:	operator <<
 *
 * Description:	Write a register to a stream.  The operand name is
 *		determined by the type of the associated expression if
 *		present.  Otherwise, the default name will be used.
 */

ostream &operator <<(ostream &ostr, const Register *reg)
{
    if (reg->_node != nullptr)
	return ostr << reg->name(reg->_node->type().size());

    return ostr << reg->name();
}

void assign(Expression *expr, Register *reg){
  if(expr != nullptr){
    if(expr->_register != nullptr)
      expr->_register->_node = nullptr;
    expr->_register = reg;
  }
  if(reg != nullptr){
    if(reg->_node != nullptr)
      reg->_node->_register = nullptr;
    reg->_node = expr;
  }

 // May need to change this for different types
 
  //if (expr != nullptr && expr->_operand == "" && expr->_register == nullptr)
    //cerr << "helo0o " << endl;


}

void load(Expression *expr, Register *reg){
  if(reg->_node != expr){
    //cout << "# inside load" << endl;
    if(reg->_node != nullptr){
      unsigned size = reg->_node->type().size();
      assigntemp(reg->_node);
      cout << "\tmov\t" << reg->name(size);
      cout << ", " << reg->_node->_operand;
      cout << "\t# spill" << endl;
    }
    if(expr != nullptr){
      //cout << "# size:" << size << endl
        //  << "# expr->size:" << expr->type().size() << endl
          //<< "# expr:" << expr<< endl;
      unsigned size = expr->type().size();
      //cerr << "hello everyone" << endl;
      cout << "\tmov" << suffix(size) << expr << ", ";
      cout << reg->name(size) << endl;
      
      //if(expr->_operand == "")
        //abort();

    }

    assign(expr,reg);
  }
}

Register *getreg(){
  for(unsigned i = 0; i<registers.size(); ++i)
    if(registers[i]->_node == nullptr)
      return registers[i];

  load(nullptr,registers[0]);
  return registers[0];
}
