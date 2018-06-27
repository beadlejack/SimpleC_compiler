#include <iostream>
#include <vector>
#include <string>
#include "Type.h"
#include "Symbol.h"
#include "Scope.h"

using namespace std;

Scope::Scope(Scope* enclosing) : _enclosing(enclosing) {}

Scope* Scope::enclosing() const { return _enclosing; }

void Scope::setEnclosing(Scope* enclosing) {
    _enclosing = enclosing;
}

Symbol* Scope::find(const string& name) const {
    cout << "searching for " << name << endl;
    //cout << "_symbols.size = " << _symbols.size() << endl;
    for (int i = 0; i < _symbols.size(); i++) {
        //cout << "_symbols[" << i << "].name = " << _symbols[i]->name() << endl;
        if (_symbols[i]->name() == name) {
            cout << "search done" << endl;
            return _symbols[i];
        }
    }
    cout << "search done" << endl;
    return NULL;
}

Symbol* Scope::lookup(const string& name) const {
    cout << "looking up for " << name << endl;
    Scope* sc = _enclosing;
    Symbol* s;
    while (sc != NULL) {
        s = sc->find(name);
        if (s != NULL)
            break;
        sc = sc->enclosing();
    }
    return s;
}

Symbol* Scope::insert(string name, Type type) {
    cout << "inserting " << name << endl;
    Symbol* s = new Symbol(name,type);
    _symbols.push_back(s);
    return s;
}
