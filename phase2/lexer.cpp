/*
 * File:	lexer.cpp
 *
 * Description:	This file contains the public and private function and
 *		variable definitions for the lexical analyzer for Simple C.
 */

# include <cstdio>
# include <cctype>
# include <string>
# include <iostream>
# include "lexer.h"
# include "tokens.h"

using namespace std;
int numerrors, lineno = 1;


/* Later, we will associate token values with each keyword */

static struct {
    string lexeme;
    int token;
} keywords[] = {
    {"auto",     AUTO},
    {"break",    BREAK},
    {"case",     CASE},
    {"char",     CHAR},
    {"const",    CONST},
    {"continue", CONTINUE},
    {"default",  DEFAULT},
    {"do",       DO},
    {"double",   DOUBLE},
    {"else",     ELSE},
    {"enum",     ENUM},
    {"extern",   EXTERN},
    {"float",    FLOAT},
    {"for",      FOR},
    {"goto",     GOTO},
    {"if",       IF},
    {"int",      INT},
    {"long",     LONG},
    {"register", REGISTER},
    {"return",   RETURN},
    {"short",    SHORT},
    {"signed",   SIGNED},
    {"sizeof",   SIZEOF},
    {"static",   STATIC},
    {"struct",   STRUCT},
    {"switch",   SWITCH},
    {"typedef",  TYPEDEF},
    {"union",    UNION},
    {"unsigned", UNSIGNED},
    {"void",     VOID},
    {"volatile", VOLATILE},
    {"while",    WHILE},
};

# define numKeywords (sizeof(keywords) / sizeof(keywords[0]))


/*
 * Function:	report
 *
 * Description:	Report an error to the standard error prefixed with the
 *		line number.  We'll be using this a lot later with an
 *		optional string argument, but C++'s stupid streams don't do
 *		positional arguments, so we actually resort to snprintf.
 *		You just can't beat C for doing things down and dirty.
 */

void report(const string &str, const string &arg)
{
    char buf[1000];

    snprintf(buf, sizeof(buf), str.c_str(), arg.c_str());
    cerr << "line " << lineno << ": " << buf << endl;
    numerrors ++;
}


/*
 * Function:	lexan
 *
 * Description:	Read and tokenize the standard input stream.  The lexeme is
 *		stored in a buffer.
 */

int lexan(string &lexbuf)
{
    int p;
    unsigned i;
    static int c = cin.get();


    /* The invariant here is that the next character has already been read
       and is ready to be classified.  In this way, we eliminate having to
       push back characters onto the stream, merely to read them again. */

    while (!cin.eof()) {
	lexbuf.clear();


	/* Ignore white space */

	while (isspace(c)) {
	    if (c == '\n')
		lineno ++;

	    c = cin.get();
	}


	/* Check for an identifier or a keyword */

	if (isalpha(c) || c == '_') {
	    do {
		lexbuf += c;
		c = cin.get();
	    } while (isalnum(c) || c == '_');

	    for (i = 0; i < numKeywords; i ++)
		if (keywords[i].lexeme == lexbuf)
		    break;

	    if (i < numKeywords)
		// cout << "keyword:" << lexbuf << endl;
		return keywords[i].token; 
	    else
		// cout << "identifier:" << lexbuf << endl;
		return ID; 


	/* Check for a number */

	} else if (isdigit(c)) {
	    do {
		lexbuf += c;
		c = cin.get();
	    } while (isdigit(c));

	    // cout << "number:" << lexbuf << endl;
	    return NUM;


	/* There must be an easier way to do this.  It might seem stupid at
	   this point to recognize each token separately, but eventually
	   we'll be returning separate token values to the parser, so we
	   might as well do it now. */

	} else {
	    lexbuf += c;

	    switch(c) {


	    /* Check for '||' */

	    case '|':
		c = cin.get();

		if (c == '|') {
		    lexbuf += c;
		    c = cin.get();
		    // cout << "operator:" << lexbuf << endl;
		    return OR;
		}

		// cout << "illegal:" << lexbuf << endl;
		return ERROR;


	    /* Check for '=' and '==' */

	    case '=':
		c = cin.get();

		if (c == '=') {
		    lexbuf += c;
		    c = cin.get();
			return EQUAL; 
		}

		// cout << "operator:" << lexbuf << endl;
		return '=';


	    /* Check for '&' and '&&' */

	    case '&':
		c = cin.get();

		if (c == '&') {
		    lexbuf += c;
		    c = cin.get();
			return AND; 
		}

		// cout << "operator:" << lexbuf << endl;
		return '&';


	    /* Check for '!' and '!=' */

	    case '!':
		c = cin.get();

		if (c == '=') {
		    lexbuf += c;
		    c = cin.get();
			return NOTEQ; 
		}

		// cout << "operator:" << lexbuf << endl;
		return '!';


	    /* Check for '<' and '<=' */

	    case '<':
		c = cin.get();

		if (c == '=') {
		    lexbuf += c;
		    c = cin.get();
			return LEQ; 
		}

		// cout << "operator:" << lexbuf << endl;
		return '<';


	    /* Check for '>' and '>=' */

	    case '>':
		c = cin.get();

		if (c == '=') {
		    lexbuf += c;
		    c = cin.get();
			return GEQ; 
		}

		// cout << "operator:" << lexbuf << endl;
		return '>';


	    /* Check for '-', '--', and '->' */

	    case '-':
		c = cin.get();

		if (c == '-') {
		    lexbuf += c;
		    c = cin.get();
			return DECREMENT; 
		} else if (c == '>') {
		    lexbuf += c;
		    c = cin.get();
			return POINTER; 
		}

		// cout << "operator:" << lexbuf << endl;
		return '-';


	    /* Check for '+' and '++' */

	    case '+':
		c = cin.get();

		if (c == '+') {
		    lexbuf += c;
		    c = cin.get();
			return INCREMENT; 
		}

		// cout << "operator:" << lexbuf << endl;
		return '+';


	    /* Check for simple, single character tokens */

	    case '*': case '%': case ':': case ';':
	    case '(': case ')': case '[': case ']':
	    case '{': case '}': case '.': case ',':
		c = cin.get();
		// cout << "operator:" << lexbuf << endl;
		return lexbuf[0];


	    /* Check for '/' or a comment */

	    case '/':
		c = cin.get();

		if (c == '*') {
		    do {
			while (c != '*' && !cin.eof()) {
			    if (c == '\n')
				lineno ++;

			    c = cin.get();
			}

			c = cin.get();
		    } while (c != '/' && !cin.eof());

		    c = cin.get();
		    break;

		} else {
		    // cout << "operator:" << lexbuf << endl;
		    return '/';
		}


	    /* Check for a string literal */

	    case '"':
		do {
		    p = c;
		    c = cin.get();
		    lexbuf += c;
		} while ((c != '"' || p == '\\') && c != '\n' && !cin.eof());

		if (c == '\n' || cin.eof())
		    report("malformed string literal");

		c = cin.get();
		//cout << "string:" << lexbuf << endl;
		return STRING;


	    /* Handle EOF here as well */

	    case EOF:
		return DONE;


	    /* Everything else is illegal */

	    default:
		c = cin.get();
		break;
	    }
	}
    }

    return ERROR;
}


/*
 * Function:	main
 *
 * Description:	Read and tokenize and standard input stream.
 */

/*
int main()
{
    string lexbuf;

    while (lexan(lexbuf))
	continue;

    return 0;
}
*/
