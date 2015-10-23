// Phillip Stewart

#include <cstdlib>
#include <iostream>
#include <string>
#include <cctype>
#include "Lexer.h"


#define NUM_KEYWORDS 17
char const *keywords[] = {"int", "real", "bool", "program", "begin", "end", "function",
		"read", "write", "if", "else", "elseif", "while", "do", "until", "true", "false"};

#define NUM_OPS 14
char ops[] = "+-*/<>=;:.,()!";


bool isValID(char c)
{
	if (isalnum(c)
	 || '_' == c) return true;
	else return false;
}

bool isKeyword(std::string s)
{
	for (int i=0; i < NUM_KEYWORDS; i++) {
		if (!s.compare(keywords[i])) {
			return true;
		}
	}
	return false;
}

bool isOp(char c) {
	for (int i=0; i < NUM_OPS; i++) {
		if (ops[i] == c) {
		 	return true;
		}
	}
	return false;
}


Lexer::Lexer()
{
	haveChar = false;
//	file = NULL;
	c = '\0';
}

Lexer::Lexer(std::string filename)
{
	haveChar = false;
	c = '\0';
	file.open(filename, std::ios::in);
	if (!file.is_open()) {
		std::cerr << "Unable to open file: " << filename << std::endl;
	exit(EXIT_FAILURE);
	}
}

Lexer::~Lexer()
{
	if (file) file.close();
}

void Lexer::newFile(std::string filename)
{
	if (file) {
		file.close();
	}
	haveChar = false;
	c = '\0';
	file.open(filename, std::ios::in);
	if (!file.is_open()) {
		std::cerr << "Unable to open file: " << filename << std::endl;
	exit(EXIT_FAILURE);
	}
}


// Function parses file and returns token/lexeme pair by reference.
// Returns false on EOF
bool Lexer::getTokLex(std::string &token, std::string &lexeme)
{
	if (!file || EOF == c)
		return false;
	
	if (!haveChar) {
		c = file.get();
		if (EOF == c) return false;
		haveChar = true;
	}
	
	while (isspace(c)) {
		c = file.get();
		if (EOF == c) return false;
	}
	
	if ('/' == c && file.peek() == '/') {
		char s[80];
		file.getline(s,80);
		c = file.get();
		if (EOF == c) return false;
	}
	
	while (isspace(c)) { //again just in case...
		c = file.get();
		if (EOF == c) return false;
	}
	
	lexeme = "";
	token = "";
	
	// Keywords and Identifiers
	if (isalpha(c)) {
		lexeme += c;
		c = file.get();
		while (isValID(c)) {
			lexeme += c;
			c = file.get();
		}
		
		if ('_' == lexeme.back()) { //last letter underscore...
			token = "invalid";
			return true;
		}
		
		if (isKeyword(lexeme)) {
			token = "keyword";
			return true;
		}
		else {
			token = "identifier";
			return true;
		}
	}
	
	// Numbers
	else if (isdigit(c)) {
		while (isdigit(c)) {
			lexeme += c;
			c = file.get();
		}
		if ('.' == c) {
			if (!isdigit(file.peek())) {
				token = "integer";
				return true;
			}
			lexeme += c;
			c = file.get();
			while (isdigit(c)) {
				lexeme += c;
				c = file.get();
			}
			token = "real number";
			return true;
		}
		else {
			token = "integer";
			return true;
		}
	}
	
	// Operators
	else if (isOp(c)) {
		switch(c) {
			case '/': //comment case alread handled
			case '+':
			case '-':
			case '*':
				lexeme += c;
				token = "math op";
				c = file.get();
				return true;
			case ':':
			case ';':
			case ',':
			case '.':
			case '!':
			case '(':
			case ')':
				lexeme += c;
				token = "gen op";
				c = file.get();
				return true;
			case '<':
				lexeme += c;
				c = file.get();
				if ('-' == c) {
					lexeme += c;
					c = file.get();
					token = "gen op";
					return true;
				}
				else if ('=' == c || '>' == c) {
					lexeme += c;
					c = file.get();
					token = "rel op";
					return true;
				}
				else {
					token = "rel op";
					return true;
				}
			case '>': // move "case '=':" here if == will be used
				lexeme += c;
				c = file.get();
				token = "rel op";
				if ('=' == c) { // == would catch here
					lexeme += c;
					c = file.get();
					return true;
				}
				else {
					return true;
				}
			case '=':
				lexeme += c;
				c = file.get();
				token = "rel op";
				return true;
//			default:
//				std::cerr << "Erroniously entered default case in getTokLex\n";
		}
	}
	lexeme += c;
	c = file.get();
	token = "invalid";
	return true;
}
