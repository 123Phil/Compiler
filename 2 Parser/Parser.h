// Phillip Stewart

#ifndef PARSER_H
#define PARSER_H

#include "Lexer.h"
#include <string>

class Parser {
	Lexer L;
	std::string tok,lex;
	
	bool Program();
	bool OptDeclList();
	bool Decl();
	bool OptFuncList();
	bool Func();
	bool OptParamList();
	bool OptParams();
	bool Param();
	bool Type();
	bool VarList();
	bool OptVars();
	bool OptStmtList();
	bool Stmt();
	bool Assign();
	bool Read();
	bool Write();
	bool If();
	bool OptElseIf();
	bool OptElse();
	bool While();
	bool Condition();
	bool RelOp();
	bool Expr();
	bool EPrime();
	bool Term();
	bool TPrime();
	bool Factor();
	bool FuncCall();
	bool OptArgList();
	bool OptArgs();
	void ident();
	
public:
	Parser(std::string filename) : L(filename) {}
	void run();
};


#endif //ifndef PARSER_H