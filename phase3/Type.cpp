#include <iostream>
#include "Type.h"
#include "tokens.h"

using namespace std;

Type::Type(Kind kind, int specifier, int indirection, int length, Parameters* parameters)
 	: _kind(kind), _specifier(specifier), _indirection(indirection),
	 	_length(length), _parameters(parameters) {}

Kind Type::kind() const { return _kind; }

int Type::specifier() const { return _specifier; }

int Type::indirection() const { return _indirection; }

int Type::length() const { return _length; }

Parameters* Type::parameters() const { return _parameters; }

bool Type::operator!=(const Type &that) const {
	return !(*this == that);
}

bool Type::operator==(const Type &that) const {
	if (_kind != that._kind) return false;
	if (_specifier != that._specifier) return false;
	if (_indirection != that._indirection) return false;
	if (_length != that._length) return false;
  	if (!equalParam(_parameters, that._parameters)) return false;
	return true;
}

std::ostream &operator<<(std::ostream& ostr, const Type & type) {
	ostr << "kind: " << type.kind() << " specifier: " << type.specifier();
	ostr << " indirection: " << type.indirection();
	if (type.length())
		ostr << " length: " << type.length();
	if (type.parameters() != NULL) {
		ostr << " with parameters: " << endl;
		Parameters* param = type.parameters();
		for (int i = 0; i < param->size(); i++) {
			ostr << "\t" << param->at(i) << endl;
		}
	}
	return ostr;
}

bool equalParam(Parameters* p, Parameters* q) {
	if (p == q) return true;
    if (p == NULL || q ==  NULL) return false;
	if (p->size() != q->size()) return false;
	for (int i = 0; i < p->size(); i++) {
		if (p->at(i) != q->at(i)) return false;
	}
	return true;
}

/* used for testing
 *
int main() {

	Type t1(SCALAR, VOID, 0);
	Type t2(ARRAY, INT, 1);
	Type t3(SCALAR, VOID, 0);

	if (t1 != t2) cout << "t1 and t2 are not the same" << endl;
	if (t1 == t3) cout << "t1 and t3 are equal" << endl;

	cout << t1 << t2 << t3;

}
*/
