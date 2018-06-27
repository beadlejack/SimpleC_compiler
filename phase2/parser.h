# ifndef PARSER_H
# define PARSER_H

extern int lookahead; 
extern std::string lexbuf; 

void match(int t); 
void expression(); 
void exprAND(); 
void exprEQ(); 
void exprComp(); 
void exprTerm(); 
void exprFactor(); 
void exprUnary(); 
void exprIndex(); 
void exprID(); 
void exprList();
void statements();
void statement(); 
void declarations();
void declaration();
void declarator(); 
void specifier(); 
void pointers();
void funcOrGlbl(); 
void remainingDecls(); 
void globalDeclarator(); 
void parameters(); 
void parameter(); 
void assignment(); 

# endif /* PARSER_H */
