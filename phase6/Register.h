/*
 * File:	Register.h
 *
 * Description:	This file contains the class definition for registers on
 *		the Intel 64-bit processor.  Each register has three
 *		operand names depending upon the access size.  Our
 *		assembler syntax refers to these as an 8-bit byte, a 32-bit
 *		long word (since a word is historically 16-bits), and a
 *		64-bit quad word.  By default, the 64-bit quad word name
 *		will be used.
 */

# ifndef REGISTER_H
# define REGISTER_H
# include <string>
# include <ostream>

class Register {
    typedef std::string string;
    string _qword;
    string _lword;
    string _byte;

public:
    class Expression *_node;

    Register(const string &qword, const string &lword, const string &byte);
    const string &name(unsigned size = 0) const;
};

std::ostream &operator <<(std::ostream &ostr, const Register *reg);
void assign(Expression *,Register *);
void load(Expression *expr,Register *reg);
Register *getreg();

extern Register *rax;
extern Register *rdi;
extern Register *rsi;
extern Register *rdx;
extern Register *rcx;
extern Register *r8 ;
extern Register *r9 ;
extern Register *r10;
extern Register *r11;
extern Register *rbp;
extern Register *rsp;
extern std::vector<Register *> registers;
# endif /* REGISTER_H */
