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
# include <vector>
# include "generator.h"
# include "machine.h"
# include "label.h"

using namespace std;

static unsigned maxargs;
static int temp_offset; 
static Label *labelptr; 
static vector<string> stringlabels; 

unsigned Label::counter = 0; 
ostream &operator<<(ostream &ostr, const Label &lbl) {
	return ostr << ".L" << lbl.number; 
}


/*
 * Function:	operator <<
 *
 * Description:	Convenience function for writing the operand of an
 *		expression.
 */

ostream &operator <<(ostream &ostr, Expression *expr)
{
    return ostr << expr->_operand;
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


    if (_symbol->_offset != 0)
	ss << _symbol->_offset << "(%ebp)";
    else
	ss << global_prefix << _symbol->name();

    _operand = ss.str();
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


# if STACK_ALIGNMENT == 4

/*
 * Function:	Call::generate
 *
 * Description:	Generate code for a function call expression, in which each
 *		argument is simply a variable or an integer literal.
 */

void Call::generate()
{
    unsigned numBytes = 0;
	_operand = gettemp(); 

    for (int i = _args.size() - 1; i >= 0; i --) {
	_args[i]->generate();
	cout << "\tpushl\t" << _args[i] << endl;
	numBytes += _args[i]->type().size();
    }

    cout << "\tcall\t" << global_prefix << _id->name() << endl;

    if (numBytes > 0)
	cout << "\taddl\t$" << numBytes << ", %esp" << endl;
	
	cout << "\tmovl\t%eax," << this << endl; 
}

# else

/*
 * If the stack has to be aligned to a certain size before a function call
 * then we cannot push the arguments in the order we see them.  If we had
 * nested function calls, we cannot guarantee that the stack would be
 * aligned.
 *
 * Instead, we must know the maximum number of arguments so we can compute
 * the size of the frame.  Again, we cannot just move the arguments onto
 * the stack as we see them because of nested function calls.  Rather, we
 * have to generate code for all arguments first and then move the results
 * onto the stack.  This will likely cause a lot of spills.
 *
 * For now, since each argument is going to be either a number of in
 * memory, we just load it into %eax and then move %eax onto the stack.
 */

void Call::generate()
{
    if (_args.size() > maxargs)
	maxargs = _args.size();

    for (int i = _args.size() - 1; i >= 0; i --) {
	_args[i]->generate();
	cout << "\tmovl\t" << _args[i] << ", %eax" << endl;
	cout << "\tmovl\t%eax, " << i * SIZEOF_ARG << "(%esp)" << endl;
    }

    cout << "\tcall\t" << global_prefix << _id->name() << endl;
}

# endif


/*
 * Function:	Assignment::generate
 *
 * Description:	Generate code for this assignment statement, in which the
 *		right-hand side is an integer literal and the left-hand
 *		side is an integer scalar variable.  Actually, the way
 *		we've written things, the right-side can be a variable too.
 */

void Assignment::generate()
{
	bool indirect; 
    _left->generate(indirect);
    _right->generate();

    cout << "\tmovl\t" << _right << ", %eax" << endl;
	if (indirect) {
		cout << "\tmovl\t" << _left << ",%ecx" << endl; 
		if (_right->type().size() == 1) 
			cout << "\tmovb\t%al,(%ecx)" << endl;
	    else 
			cout << "\tmovl\t%eax,(%ecx)" << endl; 	
	} else {
		if (_right->type().size() == 1)
   			cout << "\tmovsbl\t%al," << _left << endl; 
		else 
			cout << "\tmovl\t%eax, " << _left << endl;
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
    int offset = 0;
	Label returnLabel; 
	labelptr = &returnLabel; 

    /* Generate our prologue. */

    allocate(offset);
    cout << global_prefix << _id->name() << ":" << endl;
    cout << "\tpushl\t%ebp" << endl;
    cout << "\tmovl\t%esp, %ebp" << endl;
    cout << "\tsubl\t$" << _id->name() << ".size, %esp" << endl;


    /* Generate the body of this function. */

	temp_offset = offset; 

    maxargs = 0;
    _body->generate();

	offset = temp_offset;

    offset -= maxargs * SIZEOF_ARG;

    while ((offset - PARAM_OFFSET) % STACK_ALIGNMENT)
	offset --;


    /* Generate our epilogue. */

	cout << returnLabel << ":" << endl; 
    cout << "\tmovl\t%ebp, %esp" << endl;
    cout << "\tpopl\t%ebp" << endl;
    cout << "\tret" << endl << endl;

    cout << "\t.globl\t" << global_prefix << _id->name() << endl;
    cout << "\t.set\t" << _id->name() << ".size, " << -offset << endl;

    cout << endl;
}


/*
 * Function:	generateGlobals
 *
 * Description:	Generate code for any global variable declarations.
 */

void generateGlobals(const Symbols &globals)
{
    if (globals.size() > 0)
	cout << "\t.data" << endl;

    for (unsigned i = 0; i < globals.size(); i ++) {
	cout << "\t.comm\t" << global_prefix << globals[i]->name();
	cout << ", " << globals[i]->type().size();
	cout << ", " << globals[i]->type().alignment() << endl;
    }


	for (unsigned j = 0; j < stringlabels.size(); j++) {
		cout << stringlabels[j] << endl; 
	}
}

/*
 * Function:	gettemp()
 *
 * Description: Generate code for a stack location for a temporary variable
 * 				to be used in other expressions
 *
 */

string gettemp() {

	stringstream ss; 
	temp_offset -= 4;
	ss << temp_offset << "(%ebp)"; 
	return ss.str(); 	
}

/* 
 * Function:	Expression::generate(bool &indirect)
 *
 * Description: Default generate(bool &indirect) function for expressions
 *
 */

void Expression::generate(bool &indirect) {
	indirect = false; 
	generate(); 
}

/*
 * Function:	Expression::generate()
 *
 * Description: Default generate() function for expression. Acts as error
 * 				check against unwritten generate() functions for derivative
 * 				classes
 *
 */

void Expression::generate() {
	cerr << "oops, not written yet" << endl; 
}

/*
 * Function:	Negate::generate()
 *
 * Description: Generate code for a negation (-) expression
 *
 */

void Negate::generate(){

	_expr->generate(); 
	_operand = gettemp(); 

	cout << "\tmovl\t" << _expr << ",%eax" << endl; 
	cout << "\tnegl\t%eax" << endl; 
	cout << "\tmovl\t%eax," << this << endl; 
}

/*
 * Function:	Not::generate()
 *
 * Description: Generate code for NOT (!) expression
 *				(!x) == (x==0)
 *
 */

void Not::generate() {
	
	_expr->generate(); 
	_operand = gettemp(); 

	cout << "\tmovl\t" << _expr << ",%eax" << endl; 
	cout << "\tcmpl\t$0,%eax" << endl; 
	cout << "\tsete\t%al" << endl; 
	cout << "\tmovzbl\t%al,%eax" << endl; 
	cout << "\tmovl\t%eax," << this << endl; 
}

/*
 * Function:	Add::generate()
 *
 * Description: Generate code for addition (+) expression
 *
 */

void Add::generate() {

	_left->generate(); 
	_right->generate(); 
	_operand = gettemp(); 

	cout << "\tmovl\t" << _left << ",%eax" << endl; 
	cout << "\taddl\t" << _right << ",%eax" << endl; 
	cout << "\tmovl\t%eax," << this << endl; 
}

/*
 * Function:	Subtract::generate()
 *
 * Description: Generate code for a subtraction (-) expression
 *
 */

void Subtract::generate() {

	_left->generate(); 
	_right->generate(); 
	_operand = gettemp(); 

	cout << "\tmovl\t" << _left << ",%eax" << endl; 
	cout << "\tsubl\t" << _right << ",%eax" << endl; 
	cout << "\tmovl\t%eax," << this << endl;
}

/* 
 * Function:	Multiply::generate()
 *
 * Description: Generate code for a multiplication (*) expression
 *
 */

void Multiply::generate() {

	_left->generate(); 
	_right->generate(); 
	_operand = gettemp(); 

	cout << "\tmovl\t" << _left << ",%eax" << endl; 
	cout << "\timull\t" << _right << ",%eax" << endl; 
	cout << "\tmovl\t%eax," << this << endl; 
}

/* 
 * Function:	Divide::generate()
 *
 * Description: Generate code for a division (/) expression
 *
 */

void Divide::generate() {

	_left->generate(); 
	_right->generate(); 
	_operand = gettemp(); 

	cout << "\tmovl\t" << _left << ",%eax" << endl; 
	cout << "\tmovl\t" << _right << ",%ecx" << endl; 
	cout << "\tcltd" << endl; 
	cout << "\tidivl\t%ecx" << endl; 
	cout << "\tmovl\t%eax," << this << endl; 
}

/*
 * Function:	Remainder::generate()
 *
 * Description: Generate code for a mod (%) expression
 *
 */

void Remainder::generate() {

	_left->generate(); 
	_right->generate(); 
	_operand = gettemp(); 

	cout << "\tmovl\t" << _left << ",%eax" << endl; 
	cout << "\tmovl\t" << _right << ",%ecx" << endl; 
	cout << "\tcltd" << endl; 
	cout << "\tidivl\t%ecx" << endl; 
	cout << "\tmovl\t%edx," << this << endl; 
}

/* 
 * Function:	LessThan::generate()
 *
 * Description: Generate code for < expression
 *
 */

void LessThan::generate() {

	_left->generate(); 
	_right->generate(); 
	_operand = gettemp(); 

	cout << "\tmovl\t" << _left << ",%eax" << endl; 
	cout << "\tcmpl\t" << _right << ",%eax" << endl; 
	cout << "\tsetl\t%al" << endl; 
	cout << "\tmovzbl\t%al,%eax" << endl; 
	cout << "\tmovl\t%eax," << this << endl;  
}

/* 
 * Function:	GreaterThan::generate()
 *
 * Description: Generate code for > expression
 *
 */ 

void GreaterThan::generate() {

	_left->generate(); 
	_right->generate(); 
	_operand = gettemp(); 

	cout << "\tmovl\t" << _left << ",%eax" << endl; 
	cout << "\tcmpl\t" << _right << ",%eax" << endl; 
	cout << "\tsetg\t%al" << endl; 
	cout << "\tmovzbl\t%al,%eax" << endl; 
	cout << "\tmovl\t%eax," << this << endl;  
}

/*
 * Function:	LessOrEqual::generate()
 *
 * Description: Generate code for <= expression
 *
 */

void LessOrEqual::generate() {

	_left->generate(); 
	_right->generate(); 
	_operand = gettemp(); 

	cout << "\tmovl\t" << _left << ",%eax" << endl; 
	cout << "\tcmpl\t" << _right << ",%eax" << endl; 
	cout << "\tsetle\t%al" << endl; 
	cout << "\tmovzbl\t%al,%eax" << endl; 
	cout << "\tmovl\t%eax," << this << endl;  
}

/* 
 * Function:	GreaterOrEqual::generate()
 *
 * Description: Generate code for >= expression
 *
 */

void GreaterOrEqual::generate() {

	_left->generate(); 
	_right->generate(); 
	_operand = gettemp(); 

	cout << "\tmovl\t" << _left << ",%eax" << endl; 
	cout << "\tcmpl\t" << _right << ",%eax" << endl; 
	cout << "\tsetge\t%al" << endl; 
	cout << "\tmovzbl\t%al,%eax" << endl; 
	cout << "\tmovl\t%eax," << this << endl;  
}

/*
 * Function:	Equal::generate()
 *
 * Description: Generate code for == expression
 *
 */

void Equal::generate() {

	_left->generate(); 
	_right->generate(); 
	_operand = gettemp(); 

	cout << "\tmovl\t" << _left << ",%eax" << endl; 
	cout << "\tcmpl\t" << _right << ",%eax" << endl; 
	cout << "\tsete\t%al" << endl; 
	cout << "\tmovzbl\t%al,%eax" << endl; 
	cout << "\tmovl\t%eax," << this << endl;  
}

/*
 * Function:	NotEqual::generate()
 *
 * Description: Generate code for != expression
 *
 */

void NotEqual::generate() {

	_left->generate(); 
	_right->generate(); 
	_operand = gettemp(); 

	cout << "\tmovl\t" << _left << ",%eax" << endl; 
	cout << "\tcmpl\t" << _right << ",%eax" << endl; 
	cout << "\tsetne\t%al" << endl; 
	cout << "\tmovzbl\t%al,%eax" << endl; 
	cout << "\tmovl\t%eax," << this << endl;  
}

/*
 * Function:	Address::generate()
 *
 * Description: Generate code to get the address (&) of an object
 *
 */

void Address::generate() {

	bool indirect; 
	_expr->generate(indirect); 
	if (indirect) {
		_operand = _expr->_operand; 
	} else {
		_operand = gettemp(); 
		cout << "\tleal\t" << _expr << ",%eax" << endl; 
		cout << "\tmovl\t%eax," << this << endl; 
	}
}

/* 
 * Function:	Dereference::generate()
 *
 * Description: Generate code for a dereference (*) expression
 *
 */

void Dereference::generate() {

	_expr->generate(); 
	_operand = gettemp(); 

	cout << "\tmovl\t" << _expr << ",%eax" << endl; 
	if (_type.size() == 1)
		cout << "\tmovsbl\t(%eax),%eax" << endl; 
	else 
		cout << "\tmovl\t(%eax),%eax" << endl; 
	cout << "\tmovl\t%eax," << this << endl; 
}

/* 
 * Function:	Dereference::generate(bool &indirect)
 *
 * Description: Set indirect to TRUE for Assignment, call 
 * 				Dereference::generate()
 *
 */

void Dereference::generate(bool &indirect) {

	indirect = true; 
	_expr->generate(); 
	_operand = _expr->_operand; 
}

/* 
 * Function:	String::generate()
 *
 * Description: Generate code to call and declare strings
 *
 */

void String::generate() {

	stringstream ss; 
	Label s; 
	ss << s;
    _operand = ss.str(); 
	ss << ":\t.asciz\t" << _value;  
	stringlabels.push_back(ss.str()); 
}

/* 
 * Function:	LogicalAnd::generate()
 *
 * Description: Generate code for an && expression
 *
 */

void LogicalAnd::generate() {

	Label lbl; 
	_operand = gettemp(); 
	
	_left->generate(); 
	cout << "\tmovl\t" << _left << ",%eax" << endl; 
	cout << "\tcmpl\t$0,%eax" << endl; 
	cout << "\tje\t" << lbl << endl; 

	_right->generate();
    cout << "\tmovl\t" << _right << ",%eax" << endl; 
	cout << "\tcmpl\t$0,%eax" << endl; 

	cout << lbl << ":" << endl; 
	cout << "\tsetne\t%al" << endl; 
	cout << "\tmovzbl\t%al,%eax" << endl; 
	cout << "\tmovl\t%eax," << this << endl;
}

/* 
 * Function:	LogicalOr::generate()
 *
 * Description: Generate code for an || expression
 *
 */

void LogicalOr::generate() {

	Label lbl; 
	_operand = gettemp(); 

	_left->generate(); 
	cout << "\tmovl\t" << _left << ",%eax" << endl; 
	cout << "\tcmpl\t$0,%eax" << endl; 
	cout << "\tjne\t" << lbl << endl; 

	_right->generate(); 
	cout << "\tmovl\t" << _right << ",%eax" << endl; 
	cout << "\tcmpl\t$0,%eax" << endl; 

	cout << lbl << ":" << endl; 
	cout << "\tsetne\t%al" << endl; 
	cout << "\tmovzbl\t%al,%eax" << endl; 
	cout << "\tmovl\t%eax," << this << endl; 
}

/* 
 * Function:	Return::generate()
 *
 * Description: Generate code to place return value into register,
 *				exit function
 *
 */

void Return::generate() {

	_expr->generate(); 
	cout << "\tmovl\t" << _expr << ",%eax" << endl; 
	cout << "\tjmp\t" << *labelptr << endl; 
}

/* 
 * Function:	If::generate()
 *
 * Description: Generate code for an IF-THEN-ELSE statement
 *
 */

void If::generate() {
	
	Label elsestmt, exit; 
	
	_expr->generate(); 
	cout << "\tmovl\t" << _expr << ",%eax" << endl; 
	cout << "\tcmpl\t$0,%eax" << endl; 
	cout << "\tje\t" << elsestmt << endl; 
	_thenStmt->generate(); 
	cout << "\tjmp\t" << exit << endl; 
	cout << elsestmt << ":" << endl; 
    if (_elseStmt != nullptr) 
		_elseStmt->generate();	
	cout << exit << ":" << endl;
}

/* 
 * Function:	While::generate()
 *
 * Description: Generate code for a WHILE loop.
 *
 */

void While::generate() {
	
	Label loop, exit; 

	cout << loop << ":" << endl;
	_expr->generate(); 
	cout << "\tmovl\t" << _expr << ",%eax" << endl; 
	cout << "\tcmpl\t$0,%eax" << endl; 
	cout << "\tje\t" << exit << endl; 
	
	_stmt->generate(); 
	cout << "\tjmp\t" << loop << endl; 

	cout << exit << ":" << endl; 
}

/* 
 * Function:	For::generate()
 *
 * Description: Generate code for a FOR loop.
 *
 */

void For::generate() {

	Label loop, exit;

	/* initialize */
	_init->generate(); 

	/* start of loop */
	cout << loop << ":" << endl; 
	_expr->generate(); 
	cout << "\tmovl\t" << _expr << ",%eax" << endl; 
	cout << "\tcmpl\t$0,%eax" << endl; 
	cout << "\tje\t" << exit << endl;

	_stmt->generate();
    _incr->generate(); 	
	cout << "\tjmp\t" << loop << endl; 

	cout << exit << ":" << endl; 
}

/* 
 * Function: 	Promote::generate()
 *
 * Description: Generate code to promote a CHAR into an INT
 *
 */

void Promote::generate() {

	Type t = _expr->type(); 
	_expr->generate(); 
	if (t.size() == 1) {
		_operand = gettemp(); 
		cout << "\tmovsbl\t" << _expr << ",%eax" << endl;
		cout << "\tmovl\t%eax," << this << endl; 
	}
}
