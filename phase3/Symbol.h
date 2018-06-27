#ifndef SYMBOL_H
#define SYMBOL_H

#include <ostream>
#include <vector>
#include <string>
#include "Type.h"

class Symbol {
	Type _type;
	std::string _name;
	bool _defined;
public:
	Symbol(std::string name, Type type, bool defined = false);
	Type type() const;
	std::string name() const;
	bool defined() const;
	void setDefined(bool b);
	bool operator==(const Symbol &that) const;
	bool operator!=(const Symbol &that) const;
};

std::ostream &operator<<(std::ostream& ostr, const Type & type);

#endif /* SYMBOL_H */
