/*
 * File:	generator.cpp
 *
 * Description:	This file contains the public and member function
 *		definitions for the code generator for Simple C.
 *
 *		Extra functionality:
 *		- putting all the global declarations at the end
 */

# include <sstream>
# include <iostream>
# include "generator.h"
# include "Register.h"
# include "machine.h"
# include "Tree.h"
# include "Label.h"
# include "tokens.h"
using namespace std;


/* This needs to be set to zero if temporaries are placed on the stack. */

# define SIMPLE_PROLOGUE 0


/* Okay, I admit it ... these are lame, but they work. */

# define isNumber(expr)		(expr->_operand[0] == '$')
# define isRegister(expr)	(expr->_register != nullptr)
# define isMemory(expr)		(!isNumber(expr) && !isRegister(expr))


int offset;
Label *returnLabel;
vector<String> Strings;
static Register *parameters[] = {rdi, rsi, rdx, rcx, r8, r9};
/*
 * Function:	suffix (private)
 *
 * Description:	Return the suffix for an opcode based on the given size.
 */

string suffix(unsigned size)
{
    return size == 1 ? "b\t" : (size == 4 ? "l\t" : "q\t");
}


/*
 * Function:	align (private)
 *
 * Description:	Return the number of bytes necessary to align the given
 *		offset on the stack.
 */

static int align(int offset)
{
    if (offset % STACK_ALIGNMENT == 0)
	    return 0;

    return STACK_ALIGNMENT - (abs(offset) % STACK_ALIGNMENT);
}


/*
 * Function:	operator << (private)
 *
 * Description:	Write an expression to the specified stream.  This function
 *		first checks to see if the expression is in a register, and
 *		if not then uses its operand.
 */

ostream &operator <<(ostream &ostr, Expression *expr)
{
    if (expr->_register != nullptr)
      return ostr << expr->_register;

    //if (expr->_operand != "")
    return ostr << expr->_operand;
    //cout << "# blank operand" << endl;
    //abort();
}


/*
 * Function:	Number::generate
 *
 * Description:	Generate code for a number.  Since there is really no code
 *		to generate, we simply update our operand.
 */

void Number::generate()
{
    stringstream ss;


    ss << "$" << _value;
    _operand = ss.str();
}


/*
 * Function:	Identifier::generate
 *
 * Description:	Generate code for an identifier.  Since there is really no
 *		code to generate, we simply update our operand.
 */

void Identifier::generate()
{
    stringstream ss;


    if (_symbol->_offset == 0)
	ss << global_prefix << _symbol->name() << global_suffix;
    else
	ss << _symbol->_offset << "(%rbp)";

    _operand = ss.str();
}


/*
 * Function:	Call::generate
 *
 * Description:	Generate code for a function call.  Arguments are first
 *		evaluated in case any them are in fact other function
 *		calls.  The first six arguments are placed in registers and
 *		any remaining arguments are pushed on the stack from right
 *		to left.  Each argument on the stack always requires eight
 *		bytes, so the stack will always be aligned on a multiple of
 *		eight bytes.  To ensure 16-byte alignment, we adjust the
 *		stack pointer if necessary.
 *
 *		NOT FINISHED: Ignores any return value.
 */

void Call::generate()
{
    unsigned size, bytesPushed = 0;
    

    /* Generate code for all the arguments first. */

    for (unsigned i = 0; i < _args.size(); i ++)
	_args[i]->generate();

    for(unsigned i=0;i<registers.size();++i) {
      //cout << "# spilling" << registers[i]->name(8) << endl;
      load(nullptr,registers[i]);
    }

    /* Adjust the stack if necessary. */

    if (_args.size() > NUM_ARGS_IN_REGS) {
	bytesPushed = align((_args.size() - NUM_ARGS_IN_REGS) * SIZEOF_ARG);

	if (bytesPushed > 0)
	    cout << "\tsubq\t$" << bytesPushed << ", %rsp" << endl;
    }


    /* Move the arguments into the correct registers or memory locations. */

    for (int i = _args.size() - 1; i >= 0; i --) {
	size = _args[i]->type().size();

	if (i < NUM_ARGS_IN_REGS) {
      
	    cout << "\tmov" << suffix(size) << _args[i] << ", ";
	    cout << parameters[i]->name(size) << endl;
	} else {
	    bytesPushed += SIZEOF_ARG;

	    if (isRegister(_args[i]))
		cout << "\tpushq\t" << _args[i]->_register->name() << endl;
	    else if (isNumber(_args[i]) || size == SIZEOF_ARG)
		cout << "\tpushq\t" << _args[i] << endl;
	    else {
		cout << "\tmov" << suffix(size) << _args[i] << ", ";
		cout << rax->name(size) << endl;
		cout << "\tpushq\t%rax" << endl;
	    }
	}
    }


    /* Call the function.  Technically, we only need to assign the number
       of floating point arguments to %eax if the function being called
       takes a variable number of arguments.  But, it never hurts. */

    if (_id->type().parameters() == nullptr)
	cout << "\tmovl\t$0, %eax" << endl;

    cout << "\tcall\t" << global_prefix << _id->name() << endl;


    /* Reclaim the space of any arguments pushed on the stack. */

    if (bytesPushed > 0)
	cout << "\taddq\t$" << bytesPushed << ", %rsp" << endl;

  assign(this,rax);


  
}


/*
 * Function:	Assignment::generate
 *
 * Description:	Generate code for an assignment statement.
 *
 *		NOT FINISHED: Only works if the right-hand side is an
 *		integer literal and the left-hand size is an integer
 *		scalar.
 */

void Assignment::generate()
{
    
    if(_left->getDeref() == nullptr){ 
    _left->generate();
    _right->generate();
    if(_right->_register == nullptr)
      load(_right,getreg());
    int size = _left->type().size();
    cout << "\tmov" << suffix(size) << _right->_register->name(size) << ", " << _left << endl;
    //assign(_left,_right->_register);
    if(_right->_operand != _left->_operand)
      assign(_right,nullptr);
    assign(_left,_right->_register);
    }
    else{
      _right->generate();
      _left->getDeref()->generate();
      if(_right->_register == nullptr)
        load(_right,getreg());
      if(_left->getDeref()->_register == nullptr)
        load(_left->getDeref(), getreg());
      cout << "\tmov" << suffix(_right->type().size()) << _right->_register->name(_right->type().size()) << ", (" << _left->getDeref() << ')' << endl;
    }

}


/*
 * Function:	Block::generate
 *
 * Description:	Generate code for this block, which simply means we
 *		generate code for each statement within the block.
 */

void Block::generate()
{
    for (unsigned i = 0; i < _stmts.size(); i ++)
	_stmts[i]->generate();
}


/*
 * Function:	Function::generate
 *
 * Description:	Generate code for this function, which entails allocating
 *		space for local variables, then emitting our prologue, the
 *		body of the function, and the epilogue.
 */

void Function::generate()
{
    offset = 0;
    unsigned numSpilled = _id->type().parameters()->size();
    const Symbols &symbols = _body->declarations()->symbols();
    Label exit;
    returnLabel = &exit;

    /* Assign offsets to all symbols within the scope of the function. */

    allocate(offset);


    /* Generate the prologue, body, and epilogue. */

    cout << global_prefix << _id->name() << ":" << endl;
    cout << "\tpushq\t%rbp" << endl;
    cout << "\tmovq\t%rsp, %rbp" << endl;

    if (SIMPLE_PROLOGUE) {
	offset -= align(offset);
	cout << "\tsubq\t$" << -offset << ", %rsp" << endl;
    } else {
	cout << "\tmovl\t$" << _id->name() << ".size, %eax" << endl;
	cout << "\tsubq\t%rax, %rsp" << endl;
    }

    if (numSpilled > NUM_ARGS_IN_REGS)
	numSpilled = NUM_ARGS_IN_REGS;

    for (unsigned i = 0; i < numSpilled; i ++) {
	unsigned size = symbols[i]->type().size();
	cout << "\tmov" << suffix(size) << parameters[i]->name(size);
	cout << ", " << symbols[i]->_offset << "(%rbp)" << endl;
    }

    _body->generate();
    cout << exit << ':' << endl;
    cout << "\tmovq\t%rbp, %rsp" << endl;
    cout << "\tpopq\t%rbp" << endl;
    cout << "\tret" << endl;


    /* Finish aligning the stack. */

    if (!SIMPLE_PROLOGUE) {
	offset -= align(offset);
	cout << "\t.set\t" << _id->name() << ".size, " << -offset << endl;
    }

    cout << "\t.globl\t" << global_prefix << _id->name() << endl << endl;
}


/*
 * Function:	generateGlobals
 *
 * Description:	Generate code for any global variable declarations.
 */

void generateGlobals(Scope *scope)
{
    const Symbols &symbols = scope->symbols();

    for (unsigned i = 0; i < symbols.size(); i ++)
	if (!symbols[i]->type().isFunction()) {
	    cout << "\t.comm\t" << global_prefix << symbols[i]->name() << ", ";
	    cout << symbols[i]->type().size() << endl;
	}

  for(unsigned i = 0; i < Strings.size(); ++i)
     cout << Strings[i]._operand << ":\t" << ".asciz\t" << Strings[i].value() << endl; 
  
}

void assigntemp(Expression *expr){
    stringstream ss;
    offset = offset - expr->type().size();
    ss << offset << "(%rbp)";
    expr->_operand = ss.str();
}

void String::generate(){
  Label label = Label();
  //Register *reg = getreg();
  //cout << label << ":\t" << ".asciz\t" << _value << endl;
  _operand = ".L" + to_string(label.number());
  Strings.push_back(*this);

  //cout << "\tleaq\t" << _operand << ',' << reg->name(8) << endl;
  //assign(this,reg);
  
}

void Not::generate(){

  string name; 
  _expr->generate();
  if(_expr->_register == nullptr)
    load(_expr,getreg());   
  cout << "\tcmp\t" << "$0" << ',' << _expr << endl;
  cout << "\tsete\t" << (name = getreg()->name(1)) << endl;
  cout << "\tmovzbl\t" << name << ',' << _expr->_register->name(4) << endl;
  assign(this,_expr->_register);
}

void Negate::generate(){
  _expr->generate();
  if(_expr->_register == nullptr)
    load(_expr,getreg());
  cout << "\tneg" << suffix(type().size()) << _expr << endl;
  assign(this,_expr->_register);
}

void Dereference::generate(){
  int size;
  
  _expr->generate();
  if(_expr->_register == nullptr)
    load(_expr,getreg());
  size = _expr->type().size();
  cout << "\tmov\t" << '(' << _expr->_register->name(size) << ")," << _expr->_register->name(size) << endl;
  assign(this,_expr->_register);
  _operand = '(' + _operand + ')';
}

void Address::generate(){
  
  if(_expr->getDeref() == nullptr){

    _expr->generate();
    assign(this,getreg());
// spill bc idk what else to do
  if(_expr->_register != nullptr) 
    load(nullptr,_expr->_register); 
  //cout << "# address register = " << this->_register->name(8) << endl;
  //cout << "# node = " << ((void *) this->_register->_node) << endl;
  cout << "\tlea\t" << _expr << ',' << this << endl;
  }
  else{
    if(_expr->getDeref()->_operand != string()){
      _operand = _expr->getDeref()->_operand;
      //cerr << _expr->getDeref()->_operand << endl;
    }
    else{
      //cerr << "oops" << endl;
      //assign(this,_expr->getDeref()->_register);
      //cout << "\tlea\t" << _expr->getDeref() << ',' << this << endl;
      //(_expr->getDeref(),getreg());
      //abort();
      _expr->getDeref()->generate();
      _expr = _expr->getDeref();
      //_operan = _expr->_operand;
      assign(this,_expr->_register);
    }
  }
  
   
}

void Cast::generate(){
  _expr->generate();
  int src_size = _expr->type().size();
  int dest_size = type().size();
  
  load(_expr,getreg());

  if(dest_size <= src_size){
    //cout << "\tmov" << suffix(dest_size) << _expr->_register->name(dest_size) << ',' << _expr->_register->name(dest_size) << endl; 
    assign(this, _expr->_register);
  }
  else{
    string suf;
    if(dest_size == 4)
      suf = "sbl";
    else{
      suf = "slq";
      if(src_size == 1)
        suf = "sbq";
    }
    cout << "\tmov" << suf << '\t' << _expr << ',' << _expr->_register->name(dest_size) << endl;
    assign(this,_expr->_register);
  }
}

void Multiply::generate(){
  _left->generate();
  _right->generate();
  if(_left->_register == nullptr)
    load(_left,getreg());
  cout << "\timul" << suffix(type().size()) << _right << ',' << _left << endl;
  assign(_right,nullptr);
  assign(this,_left->_register);
}

void Divide::generate(){
  _left->generate();
  _right->generate();

  load(_left,rax);
  
  if(_right->_register == nullptr)
    load(_right,getreg());
  
  if(type().specifier() == INT || type().specifier() == CHAR)
    cout << "\tcltd\t" << endl;

  else
    cout << "\tcqto\t" << endl;

  cout << "\tidiv" << suffix(type().size()) << _right << endl;
  assign(this,rax);
  assign(_right,nullptr);
}

void Remainder::generate(){
  _left->generate();
  _right->generate();

  load(_left,rax);

  if(_right->_register == nullptr)
    load(_right,getreg());

  if(type().specifier() == INT || type().specifier() == CHAR)
    cout << "\tcltd\t" << endl;

  else
    cout << "\tcqto\t" << endl;

  cout << "\tidiv" << suffix(type().size()) << _right << endl;  
  assign(_right,nullptr);
  assign(_left,nullptr);
  assign(this,rdx);
}

void Add::generate(){
  _left->generate();
  _right->generate();
  
  if(_left->_register == nullptr)
    load(_left,getreg());

  cout << "\tadd" << suffix(type().size()) << _right << ',' << _left << endl;
  /*if(_right->_register == nullptr)
    load(_right,getreg());
  if(_left->type().isPointer()){
    
    cout << "\timul" << suffix(type().size()) << "$" << _right->type().size() << ',' << _right << endl;
    cout << "\tadd" << suffix(type().size()) << _right << ',' << _left << endl;
  }
  else if(_right->type().isPointer()){
    cout << "\timul" << suffix(type().size()) << "$" <<  _left->type().size() << ',' << _left << endl;
    cout << "\tadd" << suffix(type().size()) << _right << ',' << _left << endl;
  }
  else{
    cout << "\tadd" << suffix(type().size()) << _right << ',' << _left << endl;
  }
*/
  assign(_right,nullptr);

  assign(this,_left->_register);
}

void Subtract::generate(){
  _left->generate();
  _right->generate();
  
  if(_left->_register == nullptr)
    load(_left,getreg());

 /* 
  if(_right->type().isPointer() && _left->type().isPointer()){
    cout << "\tsub" << suffix(type().size()) << _right << ',' << _left << endl;
    if(type().specifier() == CHAR){
      assign(this,_left->_register);
      return;
    }
    load(_left,rax);
    cout << "\tcqto\t" << endl;
    cout << "\tidiv" << suffix(type().size());
    if(type().specifier() == INT)
      cout << "$4";
    else
      cout << "$8";
    cout << endl;
  }
  else if(type().isPointer()){
    if(type().specifier() == CHAR){
      cout << "\tsub" << suffix(type().size()) << _right << ',' << _left << endl;
      load(this,_left->_register);
      return;
    }
    else if(type().specifier() == INT)
      for(int i =0;i<4;++i)
        cout << "\tsub" << suffix(type().size()) << _right << ',' << _left << endl;
    else
      for(int i =0;i<8;++i)
        cout << "\tsub" << suffix(type().size()) << _right << ',' << _left << endl;
  }
  else*/
    cout << "\tsub" << suffix(type().size()) << _right << ',' << _left << endl;
  assign(this,_left->_register);
  assign(_right,nullptr);
   
}

static void set_suff(Expression *result, Expression *left, Expression *right, string suf){
  left->generate();
  right->generate();

  if(left->_register == nullptr)
    load(left,getreg());
  
  unsigned size = result->type().size();
  if(right->_register == nullptr){
    //cerr << "hello world and all who inhabit it" << endl;
    load(right,getreg());
  }
  cout << "\tcmp" << suffix(size) << right->_register->name(size) << ',' << left->_register->name(size) << endl;
  cout << "\tset" << suf << '\t' << right->_register->name(1) << endl;
  cout << "\tmovzbl\t" << right->_register->name(1) << ',' << left->_register->name(4) << endl;

  assign(result,left->_register);
  assign(right,nullptr);
}

void LessThan::generate(){
  set_suff(this,_left,_right,"l");
}

void GreaterThan::generate(){
  set_suff(this,_left,_right,"g");
}

void LessOrEqual::generate(){
  set_suff(this,_left,_right,"le");
}

void GreaterOrEqual::generate(){
  set_suff(this,_left,_right,"ge");
}

void Equal::generate(){
  set_suff(this,_left,_right,"e");
}

void NotEqual::generate(){
  set_suff(this,_left,_right,"ne");
}

void LogicalAnd::generate(){
  Label Bit,Exit;
  _left->generate();
  if(_left->_register == nullptr)
    load(_left,getreg());
  cout << "\tcmp\t" << "$0" << ',' << _left << endl;
  cout << "\tje\t" << Exit << endl;
  _right->generate();
  if(_right->_register == nullptr)
    load(_right,getreg());
  cout << "\tcmp\t" << "$0" << ',' << _right << endl;
  cout << "\tje\t" << Bit << endl;
  cout << "\tmov\t" << "$1" << ',' << _left->_register << endl; 
  cout << "\tjmp\t" << Exit << endl;
  cout << Bit << ':' << endl;
  cout << "\tmov\t" << "$0" << ',' << _left->_register << endl;
  cout << Exit << ':' << endl;
  assign(this,_left->_register);
}

void LogicalOr::generate(){
  Label Bit,Exit;
  _left->generate();
  if(_left->_register == nullptr)
    load(_left,getreg());
  cout << "\tcmp\t" << "$0" << ',' << _left << endl;
  cout << "\tjne\t" << Bit << endl;
  _right->generate();
  if(_right->_register == nullptr)
    load(_right,getreg());
  cout << "\tcmp\t" << "$0" << ',' << _right << endl;
  cout << "\tjne\t" << Bit << endl;
  cout << "\tmov\t" << "$0" << ',' << _left->_register << endl; 
  cout << "\tjmp\t" << Exit << endl;
  cout << Bit << ':' << endl;
  cout << "\tmov\t" << "$1" << ',' << _left->_register << endl;
  cout << Exit << ':' << endl;
  assign(this,_left->_register);
  assign(nullptr,_right->_register);
}


void Return::generate(){
  _expr->generate();
  load(_expr,rax);
  cout << "\tjmp\t" << *returnLabel << endl;
  assign(nullptr, rax);
}

void While::generate(){
  Label Loop, Exit;
  cout << Loop << ':' << endl;

  _expr->generate();
  if(_expr->_register == nullptr)
    load(_expr,getreg());
  cout << "\tcmp\t" << "$0" << ',' << _expr << endl;
  cout << "\tje\t" << Exit << endl;
  _stmt->generate();
  cout << "\tjmp\t" << Loop << endl;
  cout << Exit << ':' << endl;
  assign(nullptr,_expr->_register);
  
}

void If::generate(){
  Label Else, Exit;
  _expr->generate();
  if(_expr->_register == nullptr)
    load(_expr,getreg());
  cout << "\tcmp\t" << "$0" << ',' << _expr << endl;
  if(_elseStmt == nullptr)
    cout << "\tje\t" << Exit << endl;
  else
    cout << "\tje\t" << Else << endl;
  _thenStmt->generate();
  cout << "\tjmp\t" << Exit << endl;
  if(_elseStmt != nullptr){
    cout << Else << ':' << endl;
    _elseStmt->generate();
  }
  cout << Exit << ':' << endl;
   assign(nullptr,_expr->_register);
}
