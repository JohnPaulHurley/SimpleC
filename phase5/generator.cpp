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

using namespace std;


/* This needs to be set to zero if temporaries are placed on the stack. */

# define SIMPLE_PROLOGUE 1


/* Okay, I admit it ... these are lame, but they work. */

# define isNumber(expr)		(expr->_operand[0] == '$')
# define isRegister(expr)	(expr->_register != nullptr)
# define isMemory(expr)		(!isNumber(expr) && !isRegister(expr))


/* The registers that we are using in the assignment. */

static Register *rax = new Register("%rax", "%eax", "%al");
static Register *rdi = new Register("%rdi", "%edi", "%dil");
static Register *rsi = new Register("%rsi", "%esi", "%sil");
static Register *rdx = new Register("%rdx", "%edx", "%dl");
static Register *rcx = new Register("%rcx", "%ecx", "%cl");
static Register *r8 = new Register("%r8", "%r8d", "%r8b");
static Register *r9 = new Register("%r9", "%r9d", "%r9b");
static Register *parameters[] = {rdi, rsi, rdx, rcx, r8, r9};


/*
 * Function:	suffix (private)
 *
 * Description:	Return the suffix for an opcode based on the given size.
 */

static string suffix(unsigned size)
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

static ostream &operator <<(ostream &ostr, Expression *expr)
{
    if (expr->_register != nullptr)
	return ostr << expr->_register;

    return ostr << expr->_operand;
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
    _left->generate();
    _right->generate();
    cout << "\tmovl\t" << _right << ", " << _left << endl;
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
    int offset = 0;
    unsigned numSpilled = _id->type().parameters()->size();
    const Symbols &symbols = _body->declarations()->symbols();


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

    cout << "\tmovq\t%rbp, %rsp" << endl;
    cout << "\tpopq\t%rbp" << endl;
    cout << "\tret" << endl << endl;


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
}
