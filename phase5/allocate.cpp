/* 
 * File: 	allocate.cpp
 *
 */


#include "Tree.h"
#include "Type.h"
#include "Symbol.h"
#include "Scope.h"
#include "tokens.h"

using namespace std; 

void Function::allocate(int &offset) const {

	int noparams = _id->type().parameters()->size(); 
	Scope* decls = _body->declarations(); 
	Symbols symbols = decls->symbols(); 
	Type temp; 

	/* setting offsets for parameters */
	/* first parameter at fp+8 */
	offset = 8; 
	for (int i = 0; i < noparams; i++) {
		symbols[i]->setOffset(offset); 
		temp = symbols[i]->type(); 
		if (temp.isPointer() || temp == Type(INT))
			offset += 4; 
		else if(temp.isArray())
			offset += 4 * temp.length(); 
	}

	/* setting offsets for remainging declarations */
	/* first declaration at fp-sizeof(declaration) */
	offset = 0; 
	for  (unsigned j = noparams; j < symbols.size(); j++) {
		temp = symbols[j]->type(); 
		if (temp.isPointer() || temp == Type(INT))
			offset -= 4; 
		else if (temp.isArray())
			offset -= 4 * temp.length(); 
		symbols[j]->setOffset(offset); 
	}

}
