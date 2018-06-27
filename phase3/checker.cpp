#include <iostream>
#include <string>
#include <stack>
#include "checker.h"
#include "Type.h"
#include "Symbol.h"
#include "Scope.h"
#include "lexer.h"
#include "tokens.h"

using namespace std;


Scope* globalScope;
Scope* currentScope;

/* [E1] */ static string conflicting = "conflicting types for '%s'";
/* [E2] */ static string redefinition = "redefinition of '%s'";
/* [E3] */ static string redeclaration = "redeclaration of '%s'";
/* [E4] */ static string undeclared = "'%s' undeclared";
/* [E5] */ static string typevoid = "'%s' has type void";


stack<Scope*> scopes;

void openScope() {
	cout << "opening scope" << endl;
	Scope* scope;
	if (!scopes.empty())
		scope = new Scope(scopes.top());
	else
		scope = new Scope();
	scopes.push(scope);
	if (scopes.size() == 1) {
		globalScope = scopes.top();
	}
	currentScope = scopes.top();
}

void closeScope() {
	cout << "closing scope" << endl;
	Scope* s = scopes.top();
	scopes.pop();
	if (scopes.empty())
		currentScope = globalScope = NULL;
	else
		currentScope = scopes.top();
	delete s;
}

void declareFunc(string name, Type type, bool defined) {
	cout << "declare function " << name << endl;
	Symbol* s = globalScope->find(name);
	if (s != NULL) {
		if (s->type() != type) {
			report(conflicting, name);
			cout << "conflicting types for " << name << endl;
			return;
		}
		if (s->defined()) {
			report(redefinition, name);
			cout << "redef of " << name << endl;
			return;
		}
	} else {
		s = globalScope->insert(name, type);
		s->setDefined(defined);
		cout << "function " << name << " declared" << endl;
	}
}

void declareArray(string name, Type type) {
	declareVar(name, type);
}

void declareVar(string name, Type type) {
	cout << "declare variable " << name << endl;
	Symbol* s = currentScope->find(name);
	if (s != NULL) {
		if (s == NULL) cout << "returned NULL when found" << endl;
		//cout << s->name() << " found in scope" << endl;
		if(currentScope == globalScope) {
			cout << "checking types" << endl;
			cout << "s->type(): " << s->type() << endl;
			//cout << "parameter type: " << type << endl;
			//if (s->type() == type) cout << "types match" << endl;
			if (s->type() != type) {
				cout << "types don't match" << endl;
				report(conflicting, name);
				return;
			}
			cout << "types matched" << endl;
		} else {
			cout << "not in global scope, no redeclaration" << endl;
			report(redeclaration,name);
			return;
		}
	} else {
		if (isVoid(type)) {
			cout << "found type void" << endl;
			report(typevoid,name);
			return;
		} else {
			currentScope->insert(name,type);
			cout << "variable " << name << " declared" << endl;
			return;
		}
	}
}

void useSymbol(string name, Type type) {
	cout << "use " << name << endl;
	Symbol* s = currentScope->find(name);
	if (s == NULL) {
		s = currentScope->lookup(name);
		if (s == NULL) {
			if (type.kind() == FUNCTION)
				declareFunc(name, type);
			else {
				report(undeclared,name);
				cout << name << " not found" << endl;
			}
		}
	}
}

void defineFunc(string name, Type type) {
	cout << "define function " << name << endl;
	Symbol* s = globalScope->find(name);
	if (s != NULL) {
		cout << "s->type(): " << endl;
		cout << "type: " << endl;
		if (s->defined()) {
			cout << "redefinition of " << name << endl;
			report(redefinition,name);
		} else if (s->type() != type) {
			cout << "conflicting types for " << name << endl;
			report(conflicting, name);
		}
		else {
			s->setDefined(true);
			cout << "function " << name << " defined" << endl;
		}
	} else {
		declareFunc(name, type, true);
		//globalScope->insert(name,Type(FUNCTION,INT));
	}
}

bool isVoid(Type type) {
	return (type.specifier() == VOID && type.indirection() == 0);
}
