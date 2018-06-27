#ifndef TYPE_H
#define TYPE_H

#include <ostream>
#include <vector>

typedef std::vector<class Type> Parameters;

enum Kind {
	SCALAR, ARRAY, FUNCTION
};

class Type {
	Kind _kind;
	int _specifier;
	int _indirection;
	int _length;
  	Parameters *_parameters;
public:
	Type(Kind kind, int specifier = 285, int indirection = 0,
	int length = 0, Parameters *parameters = NULL);
	Kind kind() const;
	int specifier() const;
	int indirection() const;
	int length() const;
  	Parameters* parameters() const;
	bool operator==(const Type &that) const;
	bool operator!=(const Type &that) const;
};

std::ostream&operator<<(std::ostream& ostr, const Type & type);
bool equalParam(Parameters* p, Parameters* q);

#endif /* TYPE_H */
