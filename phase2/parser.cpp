#include <iostream>
#include <cstdlib>
#include <cctype>
#include <cstdio>
#include <string>
#include "lexer.h"
#include "tokens.h"
#include "parser.h"

using namespace std; 

/* function get next token from lexer */
void match(int t) {
	if (lookahead == t) 
		lookahead = lexan(lexbuf); 
}

/* expression -> expression || exprAND 
 * 			   | exprAND */
void expression() {
	exprAND(); 
	while (lookahead == OR) {
		match(lookahead); 
		exprAND(); 
		cout << "or" << endl; 
	}
}

/* exprAND -> exprAND && exprEQ 
 * 			| exprEQ */
void exprAND() {
	exprEQ(); 
	while (lookahead == AND) {
		match(lookahead); 
		exprEQ(); 
		cout << "and" << endl; 
	}
}

/* exprEQ -> exprEQ == exprComp
 * 		   | exprEQ != exprComp
 * 		   | exprComp */
void exprEQ() {
	int temp; 
	exprComp(); 
	while (lookahead == EQUAL || lookahead == NOTEQ) {
		temp = lookahead; 
		match(lookahead); 
		exprComp(); 
		if (temp == EQUAL) {
			cout << "eql" << endl; 
		} else {
			cout << "neq" << endl; 
		}
	}
}

/* exprComp -> exprComp < exprTerm
 * 		 	 | exprComp > exprTerm
 * 		 	 | exprComp <= exprTerm
 * 		 	 | exprComp >= exprTerm
 * 		 	 | exprTerm */
void exprComp() {
	int temp; 
	exprTerm(); 
	while (lookahead == '<' || lookahead == '>' || lookahead == LEQ || lookahead == GEQ) {
		temp = lookahead; 
		match(lookahead); 
		exprTerm(); 
		switch (temp) {
			case '<':
				cout << "ltn" << endl; 
				break;
			case '>':
				cout << "gtn" << endl; 
				break;
			case LEQ:
				cout << "leq" << endl; 
				break;
			case GEQ: 
				cout << "geq" << endl; 
				break; 
		}
	}
}

/* exprTerm -> exprTerm + exprFactor 
 * 			 | exprTerm - exprFactor
 * 			 | exprFactor */
void exprTerm() {
	int temp; 
	exprFactor(); 
	while (lookahead == '+' || lookahead == '-') {
		temp = lookahead; 
		match(lookahead); 
		exprFactor(); 
		switch (temp) {
			case '+': 
				cout << "add" << endl; 
				break; 
			case '-': 
				cout << "sub" << endl; 
				break;
		}
	}
}

/* exprFactor -> exprFactor * exprUnary
 * 			   | exprFactor / exprUnary
 * 			   | exprFactor % exprUnary
 * 			   | exprUnary */
void exprFactor() {
	int temp; 
	exprUnary(); 
	while (lookahead == '*' || lookahead == '/' || lookahead == '%') {
		temp = lookahead; 
		match(lookahead); 
		exprUnary(); 
		switch (temp) {
			case '*': 
				cout << "mul" << endl; 
				break; 
			case '/':
				cout << "div" << endl; 
				break; 
			case '%': 
				cout << "rem" << endl; 
				break; 
		}
	}
}

/* exprUnary -> & exprUnary
 * 			  | * exprUnary
 * 			  | ! exprUnary
 * 			  | - exprUnary
 * 			  | sizeof exprUnary
 * 			  | exprIndex */
void exprUnary() { 
	int temp; 
	if (lookahead == '*' || lookahead == '&' || lookahead == '!' || lookahead == '-' || lookahead == SIZEOF) {
		temp = lookahead; 
		match(lookahead); 
		exprUnary();  
		switch (temp) {
			case '*': 
				cout << "deref" << endl; 
				break; 
			case '&': 
				cout << "addr" << endl; 
				break; 
			case '!': 
				cout << "not" << endl; 
				break; 
			case '-': 
				cout << "neg" << endl; 
				break; 
			case SIZEOF:
				cout << "sizeof" << endl; 
				break; 
		}
	} else {
		exprIndex(); 
	}
}

/* exprIndex -> [expression]
 * 			  | exprID */
void exprIndex() {
	exprID(); 
	while (lookahead == '[') {
		match('['); 
		expression(); 
		match(']'); 
		cout << "index" << endl; 
	} /*else {
		exprID(); 
	}*/
}

/* exprID -> ID(exprList)
 * 		   | ID()
 * 		   | ID
 * 		   | NUM
 * 		   | STRING
 * 		   | (E) */
void exprID() {
	if (lookahead == ID) {
		match(ID); 
		if (lookahead == '(') {
			match('('); 
			if (lookahead != ')') {
				exprList(); 
			}
			match(')'); 
		}
	} else if (lookahead == NUM || lookahead == STRING) {
		match(lookahead); 
	} else if (lookahead == '(') {
		match('('); 
		expression(); 
		match(')'); 
	}
}

/* exprList -> expression, exprlist
 * 			 | expression */
void exprList() {
	expression(); 
	while (lookahead == ',') {
		match(','); 
		expression(); 
	}
}

/* statement -> { declarations statements }
 * 			  | RETURN expression ; 
 * 			  | WHILE ( expression ) statement
 * 			  | FOR ( assignment ; expression ; assignment ) statement
 * 			  | IF ( expression ) statement
 * 			  | IF ( expression ) statement ELSE statement
 * 			  | assignment ;  */
void statement() {
	int temp; 
	if (lookahead=='{') {
		match('{'); 
		declarations(); 
		statement();
	    while (lookahead != '}') 
			statement(); 	
		match('}');
	} else if (lookahead == WHILE || lookahead == IF) {
		temp = lookahead; 
		match(lookahead); 
		match('('); 
		expression(); 
		match(')'); 
		statement(); 
		if (temp == IF) {
			if (lookahead==ELSE) {
				match(ELSE); 
				statement(); 
			}
		}
	} else if (lookahead == FOR) {
		match(lookahead); 
		match('('); 
		assignment(); 
		match(';'); 
		expression(); 
		match(';'); 
		assignment(); 
		match(')'); 
		statement(); 
	} else if (lookahead == RETURN) {
		match(lookahead); 
		expression(); 
		match(';');
	} else {
		assignment();
	    match(';'); 	
	}
}

/* statements -> empty 
 * 			   | statement statements */

/* note: function not used */
void statements() {
	statement(); 
	while (lookahead != '}') 
		statement(); 
}

/* declarations -> empty
 * 				 | declaration declarations */
void declarations(){
	while (lookahead == INT || lookahead == CHAR || lookahead == VOID) {
		declaration(); 	
	}
}

/* declaration -> specifier declarator-list */
void declaration() {
	specifier(); 
	declarator(); 
	match(';');
}

/* declarator -> pointers ID
 * 			   | pointers ID [ NUM ] */

/* covers for declarator-list: 
 * declarator-list -> declarator
 * 					| declarator , declarator-list */
void declarator() {
	pointers(); 
	match(ID);
	if (lookahead == '[') {
		match('['); 
		match(NUM); 
		match(']'); 
	}
	if (lookahead == ',') {
		match(','); 
   		declarator(); 
 	}		
}

/* pointers -> empty 
 * 			 | * pointers */
void pointers() {
	while (lookahead == '*') 
		match('*'); 
}

/* specifier -> INT
 * 			  | CHAR
 * 			  | VOID */
void specifier() {
	if (lookahead == INT || lookahead == CHAR || lookahead == VOID) 
		match(lookahead); 
}

/* assignment -> expression
 * 			   | expression = expression */
void assignment(){
	expression(); 
	while (lookahead == '=') {
		match(lookahead); 
		expression(); 
	}
}

/* funcOrGlbl() incorporates translation-unit, global-declaration, and function-definition */
void funcOrGlbl() {
	specifier(); 
	pointers(); 
	match(ID); 
	if (lookahead == '(') {
		match('('); 
		parameters(); 
		match(')'); 
		if (lookahead == '{') {
			/*match('{'); 
			declarations();  
			statements();
		    match('}'); */
			statement(); 
		} else {
			remainingDecls(); 
		}
	} else {
		while (lookahead == '[') {
			match('['); 
			match(NUM); 
			match(']'); 
		}
		remainingDecls(); 
	}
}

/* remainingDecls -> ; 
 * 				   | globalDeclarator , remainingDecls */
void remainingDecls() {
	while (lookahead != ';') {
		match(','); 
		globalDeclarator(); 
	}
	match(';'); 
}

/* globalDeclarator -> pointers ID
 * 					 | pointers ID ( parameters )
 * 					 | pointers ID [ NUM ] */
void globalDeclarator() {
	pointers(); 
	match(ID); 
	if (lookahead == '(') {
		match('('); 
		parameters(); 
		match(')'); 
	} else if(lookahead == '[') { 
		match('['); 
		match(NUM); 
		match(']'); 
	}
}

/* parameters -> VOID 
 * 			   | parameter , parameters
 * 			   | parameter  */
void parameters() {
		parameter(); 
		while (lookahead == ',') {
			match(','); 
			parameter();
		}
}

/* parameter -> specifier pointers ID */
void parameter() {
	specifier(); 
	pointers(); 
	match(ID); 
}

int lookahead; 
string lexbuf; 

/* main function: gets first token from lexer,
 * 		calls funcOrGlbl() until EOF */
int main() {
	lookahead = lexan(lexbuf); 
	while (lookahead != DONE) 
		funcOrGlbl(); 
}
			
