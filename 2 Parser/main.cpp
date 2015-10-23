// Phillip Stewart, 891499733
// OCT 2014

// compiled on OSX 10.9.5 using g++ which invokes clang, with:
//   $ g++ main.cpp Parser.cpp Lexer.cpp

// can be run supplying filename as command line argument:
//   ./a.out program.txt
// or run and supplied filename during runtime:
//   ./a.out
//  >Enter filename: program.txt


#include <iostream>
#include <string>
#include "Lexer.h"
#include "Parser.h"


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

	Parser P(filename);
	P.run();

	return 0;
}
