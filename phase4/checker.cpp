/*
 * File:	checker.cpp
 *
 * Description:	This file contains the public and private function and
 *		variable definitions for the semantic checker for Simple C.
 *
 *		If a symbol is redeclared, the existing declaration is
 *		retained and the redeclaration discarded.  This behavior
 *		seems to be consistent with GCC, and who are we to argue
 *		with GCC?
 *
 *		Extra functionality:
 *		- inserting an undeclared symbol with the error type
 */

# include <iostream>
# include <string>
# include "lexer.h"
# include "checker.h"
# include "nullptr.h"
# include "tokens.h"
# include "Symbol.h"
# include "Scope.h"
# include "Type.h"

# define FUNCDEFN 1


using namespace std;

static Scope *outermost, *toplevel;
const Type error;
const Type integer = Type(INT,0);

static string redefined = "redefinition of '%s'";
static string redeclared = "redeclaration of '%s'";
static string conflicting = "conflicting types for '%s'";
static string undeclared = "'%s' undeclared";
static string void_object = "'%s' has type void";
static string invalidReturn = "invalid return type";
static string invalidType = "invalid type for test expression";
static string lvalueRequired = "lvalue required in expression";
static string invalidOperands = "invalid operands to binary %s";
static string invalidOp = "invalid operand to unary %s";
static string notFunction = "called object is not a function";
static string invalidArguments = "invalid arguments to called function";


/*
 * Function:	checkIfVoidObject
 *
 * Description:	Check if TYPE is a proper use of the void type (if the
 *		specifier is void, then the indirection must be nonzero or
 *		the kind must be a function).  If the type is proper, it is
 *		returned.  Otherwise, the error type is returned.
 */

static Type checkIfVoidObject(const string name, const Type &type)
{
    if (type.specifier() != VOID)
	return type;

    if (type.indirection() == 0 && !type.isFunction()) {
	report(void_object, name);
	return error;
    }

    return type;
}


/*
 * Function:	openScope
 *
 * Description:	Create a scope and make it the new top-level scope.
 */

Scope *openScope()
{
    toplevel = new Scope(toplevel);

    if (outermost == nullptr)
	outermost = toplevel;

    return toplevel;
}


/*
 * Function:	closeScope
 *
 * Description:	Remove the top-level scope, and make its enclosing scope
 *		the new top-level scope.
 */

Scope *closeScope()
{
    Scope *old = toplevel;
    toplevel = toplevel->enclosing();
    return old;
}


/*
 * Function:	defineFunction
 *
 * Description:	Define a function with the specified NAME and TYPE.  A
 *		function is always defined in the outermost scope.
 */

Symbol *defineFunction(const string &name, const Type &type)
{
    Symbol *symbol = declareFunction(name, type);

    if (symbol->_attributes & FUNCDEFN)
	report(redefined, name);

    symbol->_attributes = FUNCDEFN;
    return symbol;
}


/*
 * Function:	declareFunction
 *
 * Description:	Declare a function with the specified NAME and TYPE.  A
 *		function is always declared in the outermost scope.  Any
 *		redeclaration is discarded.
 */

Symbol *declareFunction(const string &name, const Type &type)
{
    cout << name << ": " << type << endl;
    Symbol *symbol = outermost->find(name);

    if (symbol == nullptr) {
	symbol = new Symbol(name, type);
	outermost->insert(symbol);

    } else if (type != symbol->type()) {
	report(conflicting, name);
	delete type.parameters();

    } else
	delete type.parameters();

    return symbol;
}


/*
 * Function:	declareVariable
 *
 * Description:	Declare a variable with the specified NAME and TYPE.  Any
 *		redeclaration is discarded.
 */

Symbol *declareVariable(const string &name, const Type &type)
{
    cout << name << ": " << type << endl;
    Symbol *symbol = toplevel->find(name);

    if (symbol == nullptr) {
	symbol = new Symbol(name, checkIfVoidObject(name, type));
	toplevel->insert(symbol);

    } else if (outermost != toplevel)
	report(redeclared, name);

    else if (type != symbol->type())
	report(conflicting, name);

    return symbol;
}


/*
 * Function:	checkIdentifier
 *
 * Description:	Check if NAME is declared.  If it is undeclared, then
 *		declare it as having the error type in order to eliminate
 *		future error messages.
 */

Symbol *checkIdentifier(const string &name)
{
    Symbol *symbol = toplevel->lookup(name);

    if (symbol == nullptr) {
	report(undeclared, name);
	symbol = new Symbol(name, error);
	toplevel->insert(symbol);
    }

    return symbol;
}


/*
 * Function:	checkFunction
 *
 * Description:	Check if NAME is a previously declared function.  If it is
 *		undeclared, then implicitly declare it.
 */

Symbol *checkFunction(const string &name)
{
    Symbol *symbol = toplevel->lookup(name);

    if (symbol == nullptr)
	symbol = declareFunction(name, Type(INT, 0, nullptr));

    return symbol;
}

Type checkLogicalOR(const Type& left, const Type& right) {
    return checkLogical(left,right,"||");
}

Type checkLogicalAND(const Type& left, const Type& right) {
    return checkLogical(left,right,"&&");
}

Type checkLogical(const Type& left, const Type& right, const string& op) {
    cout << left << "||" << right << endl;
    if (left == error || right == error) {
        return error;
    }
    if (left.isPredicate() && right.isPredicate())
		return integer;
    report(invalidOperands,op);
    return error;
}

Type checkEquality(const Type& left, const Type& right, const string& op) {
    cout << left << " " + op + " " << right << endl;
    if (left == error || right == error) {
        return error;
    }
    if (left.isCompatibleWith(right))
        return integer;
    report(invalidOperands,op);
    return error;
}

Type checkRelational(const Type& left, const Type& right, const string& op) {
    cout << left << " " + op + " " << right << endl;
    if (left == error || right == error){
        return error;
    }
    Type leftP = left.promote();
    Type rightP = right.promote();
    if (!left.isPredicate() || !right.isPredicate()) {
        report(invalidOperands,op);
        return error;
    } else if (leftP == rightP)
        return integer;
    report(invalidOperands,op);
    return error;
}

Type checkAdditive(const Type& left, const Type& right) {
    cout << left << " + " << right << endl;
    if (left == error || right == error) {
        return error;
    }
    Type leftP = left.promote();
    Type rightP = right.promote();
    if (leftP == integer) {
        if (rightP == integer)
            return integer;
        else if (rightP.isPointer()) {
            if (!rightP.isVoidPtr())
                return Type(rightP.specifier(),rightP.indirection());
        }
    }
    if (leftP.isPointer()) {
        if (!leftP.isVoidPtr())
            if (rightP == integer)
                return Type(leftP.specifier(), leftP.indirection());
    }
    report(invalidOperands,"+");
    return error;
}

Type checkSubtraction(const Type& left, const Type& right) {
    cout << left << " - " << right << endl;
    if (left == error || right == error) {
        return error;
    }
    Type leftP = left.promote();
    Type rightP = right.promote();
    if (leftP == integer) {
        if (rightP == integer)
            return integer;
    } else if (leftP.isPointer() && !leftP.isVoidPtr()) {
        if (rightP == integer)
            return Type(leftP.specifier(),leftP.indirection());
        else if (rightP == leftP)
            return integer;
    }
    report(invalidOperands,"-");
    return error;
}

Type checkMultiplicative(const Type& left, const Type& right, const string& op) {
    cout << left << " " + op + " " << right << endl;
    if (left == error || right == error) {
        return error;
    }
    Type leftP = left.promote();
    Type rightP = right.promote();
    if (leftP == integer)
        if (rightP == integer)
            return integer;
    report(invalidOperands,op);
    return error;
}

Type checkPrefixNOT(const Type& left) {
    cout << "!" << left << endl;
    if (left == error) {
        return error;
    }
    if (left.isPredicate()) {
        return integer;
    }
    report(invalidOp,"!");
    return error;
}

Type checkPrefixNEG(const Type& left) {
    cout << "-" << left << endl;
    if (left == error) {
        return error;
    }
    if (left == integer)
        return integer;
    report(invalidOp,"-");
    return error;
}

Type checkPrefixDEREF(const Type& left) {
    cout << "*" << left << endl;
    if (left == error) {
        return error;
    }
    Type promoted = left.promote();
    if (promoted.isPointer() && !promoted.isVoidPtr()) {
        return Type(promoted.specifier(),promoted.indirection()-1);
    }
    report(invalidOp,"*");
    return error;
}

Type checkPrefixADDR(const Type& left, bool& lvalue) {
    cout << "&" << left << endl;
    if (left == error) {
        return error;
    }
    if (lvalue) {
        return Type(left.specifier(), left.indirection()+1);
    }
    report(lvalueRequired,"");
    return error;
}

Type checkPrefixSIZEOF(const Type& left) {
    cout << "sizeof " << left << endl;
    if (left == error) {
        return error;
    }
    if (left.isPredicate())
        return integer;
    report(invalidOp,"sizeof");
    return error;
}

Type checkPostfix(const Type& left, const Type& right) {
    cout << left << "[" << right << "]"<< endl;
    if (left == error || right == error) {
        return error;
    }
    Type leftP = left.promote();
    if (leftP.isPointer() && !leftP.isVoidPtr()) {
        if (right.promote() == integer)
            return Type(leftP.specifier(),leftP.indirection()-1);
    }
    report(invalidOperands,"[]");
    return error;
}

Type checkFunctionCall(const Type& type, const Parameters* arguments){
    cout << "function call " << type << endl;
    if (type == error) {
        return error;
    }
    if (!type.isFunction()){
        report(notFunction,"");
        return error;
    }
    /* check if arguments are predicate types */
    for (int j = 0; j < arguments->size(); j++) {
        if (!arguments->at(j).isPredicate()) {
            report(invalidArguments,"");
            return error;
        }
    }
    /* check if arguments and parameters have compatible types */
    Parameters* param = type.parameters();
    if (param != NULL) {
        if (param->size() == arguments->size()) {
            for (int i = 0; i < param->size(); i++) {
                if (!param->at(i).isCompatibleWith(arguments->at(i))) {
                    report(invalidArguments,"");
                    return error;
                }
            }
            /* all predicate, all compatible*/
            return Type(type.specifier(),type.indirection());
        }
    } else {
        return Type(type.specifier(),type.indirection());
    }
    report(invalidArguments,"");
    return error;
}

Type checkAssignment(const Type& left, const Type& right, bool& lvalue) {
    if (left == error || right == error) {
        return error;
    }
    if (!lvalue) {
        report(lvalueRequired,"");
        return error;
    }
    if (left.isCompatibleWith(right))
        return left;
    report(invalidOperands,"=");
    return error;
}

Type checkReturn(const Type& expr, const Type& func) {
    if (expr == error) return error;
    Type funcReturn = checkFunctionCall(func,func.parameters());
    if (expr.isCompatibleWith(funcReturn)) return expr;
    report(invalidReturn,"");
    return error;
}

Type checkConditional(const Type& expr) {
    if (expr == error) return error;
    if (expr.isPredicate()) return expr;
    report(invalidType,"");
    return error;
}
