// Phillip Stewart, 891499733
// 10 SEP 2014


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
