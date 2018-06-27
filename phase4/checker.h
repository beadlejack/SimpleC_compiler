/*
 * File:	checker.h
 *
 * Description:	This file contains the public function declarations for the
 *		semantic checker for Simple C.
 */

# ifndef CHECKER_H
# define CHECKER_H
# include "Scope.h"

Scope *openScope();
Scope *closeScope();

Symbol *defineFunction(const std::string &name, const Type &type);
Symbol *declareFunction(const std::string &name, const Type &type);
Symbol *declareVariable(const std::string &name, const Type &type);
Symbol *checkIdentifier(const std::string &name);
Symbol *checkFunction(const std::string &name);
Type checkLogicalOR(const Type& left, const Type& right);
Type checkLogicalAND(const Type& left, const Type& right);
Type checkLogical(const Type& left, const Type& right, const std::string& op);
Type checkEquality(const Type& left, const Type& right, const std::string& op);
Type checkRelational(const Type& left, const Type& right, const std::string& op);
Type checkAdditive(const Type& left, const Type& right);
Type checkSubtraction(const Type& left, const Type& right);
Type checkMultiplicative(const Type& left, const Type& right, const std::string& op);
Type checkPrefixNOT(const Type& left);
Type checkPrefixNEG(const Type& left);
Type checkPrefixDEREF(const Type& left);
Type checkPrefixADDR(const Type& left, bool& lvalue);
Type checkPrefixSIZEOF(const Type& left);
Type checkPostfix(const Type& left, const Type& right);
Type checkFunctionCall(const Type& type, const Parameters* param);
Type checkAssignment(const Type& left, const Type& right, bool& lvalue);
Type checkReturn(const Type& expr, const Type& func);
Type checkConditional(const Type& type);

# endif /* CHECKER_H */
