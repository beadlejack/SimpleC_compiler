#include <iostream>
#include <vector>
#include <string>
#include "Type.h"
#include "Symbol.h"
#include "tokens.h"

using namespace std;

Symbol::Symbol(string name, Type type, bool defined)
	: _name(name), _type(type), _defined(defined) {}

Type Symbol::type() const { return _type; }

string Symbol::name() const { return _name; }

bool Symbol::defined() const { return _defined; }

void Symbol::setDefined(bool b) { _defined = b; }

bool Symbol::operator!=(const Symbol &that) const {
	return !(*this == that);
}

bool Symbol::operator==(const Symbol &that) const {
	if (_type != that._type) return false;
	if (_name != that._name) return false;
	return true;
}

std::ostream &operator<<(std::ostream& ostr, const Symbol & symbol) {
	ostr << "type: " << symbol.type() << "name: " << symbol.name();
	return ostr;
}

/*
int main() {

	Type t1(SCALAR, VOID, 0);
	Type t2(ARRAY, INT, 1, 10);
	Type t3(SCALAR, VOID, 0);
	Type t4(FUNCTION, INT, 0);

	Parameters *param = new Parameters;
	param->insert(param->end(),t1);
	param->insert(param->end(),t2);

	Symbol s1(t1,"foo");
	Symbol s2(t4,"bar",param);

	if (s1 != s2) cout << "not equal" << endl;

	cout << "symbol s1..." << s1 << endl;
	cout << "symbol s2..." << s2 << endl;

	return 0;
} */
