#ifndef SCOPE_H
#define SCOPE_H

#include <vector>
#include "Type.h"
#include "Symbol.h"

typedef std::vector<Symbol*> Symbols;

class Scope {
	Scope *_enclosing;
	Symbols _symbols;
public:
	Scope(Scope* enclosing = NULL);
	Scope* enclosing() const;
	void setEnclosing(Scope* enclosing);
	Symbol* lookup(const std::string &name) const;
	Symbol* find(const std::string &name) const;
	Symbol* insert(std::string name, Type type);
};

#endif /* SCOPE_H */
