// Phillip Stewart

#ifndef PARSER_H
#define PARSER_H

#include "Lexer.h"
#include <string>
#include <vector>

// New Symbol and icode classes - basically structs
// used for holding symbol table and intermediate code
class Symbol {
public:
	int addr;
	std::string id;
	std::string type;
	Symbol();
	Symbol(int a, std::string ident, std::string t) {
		addr = a;
		id = ident;
		type = t;
	}
};

class icode {
public:
	int line;
	std::string instr;
	std::string addr;
	icode();
	icode(int l, std::string i, std::string a="") {
		line = l;
		instr = i;
		addr = std::string(a);
	}
};

class Parser {
	Lexer L;
	std::string tok,lex;
	
//new members for intermediate code
	std::vector< Symbol > symbols;
	std::vector< Symbol > funcs;
	std::vector< Symbol > func_params;
	int s_curr_addr;
	int f_curr_addr;
	std::string curr_type;
	std::vector< icode > assembly;
	int a_line;
	std::string curr_relop;
	
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
	bool ReadList();
	bool OptReads();
	bool OptStmtList();
	bool Stmt();
	bool Assign();
	bool Read();
	bool Write();
	bool If();
//pointer magic to avoid stacks and nesting issues
	bool OptElseIf(int* jzp, std::vector< int > *jp);
	bool OptElse(int jz);
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
//	void ident();

	bool get_symbol_addr(std::string l, int &a);
	void end_nop();
	
public:
	Parser(std::string filename) : L(filename) {
		s_curr_addr = 300;
		f_curr_addr = 500;
		a_line = 1;
	}
	bool run();
	void print_symbols();
	void print_assembly();
};


#endif //ifndef PARSER_H