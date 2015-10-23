// Phillip Stewart, 891499733
// 10 SEP 2014


#include <iostream>
#include <string>
#include <iomanip>
#include "Lexer.h"


int main(int argc, char** argv)
{
	std::string filename;
	
	if (argc == 2) {
		filename = argv[1];
	}
	else {
		std::cout << "Enter filename: ";
		std::cin >> filename;
	}
	
	Lexer L(filename);
	
	std::string tok,lex;
	while(L.getTokLex(tok,lex)) {
		std::cout.width(12);
		std::cout << std::left << lex << tok << std::endl;
	}
	
	return 0;
}
