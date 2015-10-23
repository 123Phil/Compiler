// Phillip Stewart

#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <string>
#include <fstream>


class Lexer {
	bool haveChar;
	std::ifstream file;
	char c;

public:
	Lexer();
	Lexer(std::string filename);
	~Lexer();
	void newFile(std::string filename);
	bool getTokLex(std::string&, std::string&);
};


#endif //ifndef LEXER_H