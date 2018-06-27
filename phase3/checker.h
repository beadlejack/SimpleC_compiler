#ifndef CHECKER_H
#define CHECKER_H

#include <string>
#include "Type.h"
#include "Symbol.h"
#include "Scope.h"


void openScope();
void closeScope();
void declareFunc(std::string name, Type type, bool defined = false);
void declareArray(std::string name, Type type);
void declareVar(std::string name, Type type);
void useSymbol(std::string name, Type type);
void defineFunc(std::string name, Type type);
bool isVoid(Type type);

#endif /* CHECKER_H */
