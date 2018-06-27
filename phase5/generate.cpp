/* Jackson Beadle
 *
 * File: 		generate.cpp
 *
 * Description:	Contains all member functions to generate Intel Assembly
 * 				code for Identifier, Number, Call, Block, and Function.
 *
 */

#include "Tree.h"
#include "Type.h"
#include "Symbol.h"
#include "tokens.h"
#include <iostream>
#include <string>
#include <sstream>

using namespace std; 

void Identifier::generate() {

	stringstream s; 
	int offset = _symbol->offset(); 
	if (offset) {
		s << offset;
		location = s.str() + "(%ebp)"; 
	} else {
		location = _symbol->name();
	}	
	cout << location; 
}

void Number::generate() {
	cout << "$" << _value; 
}

void Call::generate() {
 
	cout << "\t# generating call" << endl; 
	
	for (int i = _args.size()-1; i > -1; i--) {
		cout << "\tpushl\t"; 
		_args[i]->generate(); 
		cout << endl; 
	}
	cout << "\tcall\t" << _id->name() << endl; 
}

void Block::generate() {
	cout << "\t# generating block" << endl;  
	for (unsigned i = 0; i < _stmts.size(); i++) {
		_stmts[i]->generate(); 
	}

}

void Function::generate() {


	int offset = 0; 
	allocate(offset);
	if (offset < 0) offset *= -1; 

	/* function name */
	cout << _id->name() << ":" << endl; 

	/* function prologue */
	cout << "\t# function prologue" << endl; 
	cout << "\tpushl\t%ebp" << endl; 
	cout << "\tmovl\t%esp,%ebp" << endl; 
	cout << "\tsubl\t$" << offset << ",%esp" << endl; 

	/* call block generator */
	_body->generate(); 

	/* function epilogue */
	cout << "\t# function epilogue" << endl; 
	cout << "\tmovl\t%ebp,%esp" << endl; 
	cout << "\tpopl\t%ebp" << endl; 
	cout << "\tret" << endl; 
}

void Assignment::generate() {

	cout << "\t# assignment" << endl;

	/* movl	RIGHT,%eax */
	cout << "\tmovl\t"; 
	_right->generate(); 
	cout << ",%eax" << endl;

	/* movl	%eax,LEFT */
	cout << "\tmovl\t%eax,"; 
	_left->generate(); 
	cout << endl; 
}
