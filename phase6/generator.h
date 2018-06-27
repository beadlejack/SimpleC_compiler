/*
 * File:	generator.h
 *
 * Description:	This file contains the function declarations for the code
 *		generator for Simple C.  Most of the function declarations
 *		are actually member functions provided as part of Tree.h.
 */

# ifndef GENERATOR_H
# define GENERATOR_H
# include "Tree.h"
# include <string>

void generateGlobals(const Symbols &globals);
std::string gettemp(); 

# endif /* GENERATOR_H */
