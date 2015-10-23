// Phillip Stewart

#include <iostream>
#include <cctype>
#include <iomanip>
#include "Parser.h"
#include "Lexer.h"

/* Basic patterns:

//follows
	if ("follow" == lex) {
		std::cout << "Rule => null\n";
		return true;
	}

//match
	if (!("match" == lex)) {
		std::cout << "Expected 'match'\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}

//identifier
if (!("identifier" == tok)) {
		std::cout << "Expected identifier\n";
		return false;
	} else {
		ident();
		L.getTokLex(tok,lex);
	}

// get symbol addr, add assembly instr.
	int a;
	if (!get_symbol_addr(lex, a)) {
		std::cout << "Error: Variable '" << lex << "' not declared.\n";
		return false;
	}
	assembly.push_back(icode(a_line,"PUSHI"));
	++a_line;
		
*/

bool Parser::run()
{
	if (!Program()) {
		std::cout << "Parser failed to reach end of file.\n";
		return false;
	}
	end_nop();
	return true;
}	
	
bool Parser::Program()
{
	L.getTokLex(tok,lex);
	if (!("program" == lex)) {
		std::cout << "Program did not start with keyword 'program'\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	if (!OptDeclList()) return false;
	if (!OptFuncList()) return false;
	if (!("begin" == lex)) {
		std::cout << "Program main did not start with keyword 'begin'\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	if (!OptStmtList()) return false;
	if (!("end" == lex)) {
		std::cout << "Program main did not end with keyword 'end'\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	if (!("." == lex)) {
		std::cout << "Program main did not end with '.'\n";
		return false;
	}
	
//	std::cout << "Program => program OptDeclList OptFuncList begin OptStmtList end .\n";
	return true;
}

bool Parser::OptDeclList()
{
	if ("function" == lex ||
		"begin" == lex) {
//		std::cout << "OptDeclList => null\n";
		return true;
	}
	
	if (!Decl()) return false;
	if (!OptDeclList()) return false;
	
//	std::cout << "OptDeclList => Decl OptDeclList\n";
	return true;
}

bool Parser::Decl()
{
	if (!Type()) return false;
	if (!VarList()) return false;
	
	if (";" == lex) {
		L.getTokLex(tok,lex);
//		std::cout << "Decl => Type VarList ;\n";
		return true;
	}
	else {
		std::cout << "Declaration did not end with ';'\n";
		return false;
	}
}

bool Parser::OptFuncList()
{
	if ("begin" == lex) {
//		std::cout << "OptFuncList => null\n";
		return true;
	}
	
	if (!Func()) return false;
	if (!OptFuncList()) return false;
	
//	std::cout << "OptFuncList => Func OptFuncList\n";
	return true;
}

bool Parser::Func()
{
	if (!("function" == lex)) {
		std::cout << "Function did not start with keyword 'function'\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	
	std::string func_name;
	if (!("identifier" == tok)) {
		std::cout << "Invalid identifier in parameter list of function\n";
		return false;
	} else {
//		ident();
		func_name = lex;
		L.getTokLex(tok,lex);
	}

	if (!("(" == lex)) {
		std::cout << "Function parameters missing opening parenthesis\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	
	if (!OptParamList()) return false;
	
	if (!(")" == lex)) {
		std::cout << "Function parameters missing closing parenthesis\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	
	if (!(":" == lex)) {
		std::cout << "Function missing colon\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	
	if (!Type()) return false;
	
	for (int i=0; i < funcs.size(); i++) {
		if (lex == funcs[i].id) {
			std::cout << "Error: function '" << lex << "' declared more than once.\n";
			return false;
		}
	}
	funcs.push_back(Symbol(f_curr_addr, func_name, curr_type));
	++f_curr_addr;
	
	if (!(";" == lex)) {
		std::cout << "Function type not followed by semicolon\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}

// +++++++++++++++++++++	
// If functions actually implemented in working compiler, revise this
// to a functionDeclList, or note symbol scope.
	if (!OptDeclList()) return false;
	
	if (!("begin" == lex)) {
		std::cout << "Function body did not start with keyword 'begin'\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}

// +++++++++++++++++++++
// If functions actually implemented in working compiler, revise this
// so that the function address jumps to these statements,
// and add a return at function end.
	if (!OptStmtList()) return false;
	
	if (!("end" == lex)) {
		std::cout << "Function body did not end with keyword 'end'\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	
//	std::cout << "Func => function ident (OptParamList):Type; OptDeclList begin OptStmtList end\n";
	return true;
}

bool Parser::OptParamList()
{
	if (")" == lex) {
//		std::cout << "OptParamList => null\n";
		return true;
	}
	
	if (!Param()) return false;
	if (!OptParams()) return false;
	
//	std::cout << "OptParamList => Param OptParams\n";
	return true;
}

bool Parser::OptParams()
{
	if (")" == lex) {
//		std::cout << "OptParams => null\n";
		return true;
	}
	
	if (!("," == lex)) {
		std::cout << "Parameter list not delimited by ','\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	
	if (!Param()) return false;
	if (!OptParams()) return false;
	
//	std::cout << "OptParamList => Param OptParams\n";
	return true;
}

bool Parser::Param()
{
	if (!Type()) return false;
	
	if (!("identifier" == tok)) {
		std::cout << "Invalid identifier in parameter list\n";
		return false;
	} else {
//		ident();
		func_params.push_back(Symbol(f_curr_addr, lex, curr_type));
		L.getTokLex(tok,lex);
	}
	
//	std::cout << "Param => Type ident\n";
	return true;
}

bool Parser::Type()
{
	if ("int"  == lex ||
		"real" == lex ||
		"bool" == lex) {
		//std::cout << "Type => " << lex << std::endl;
		curr_type = lex;
		L.getTokLex(tok,lex);
		return true;
	}
	else {
		std::cout << "Invalid type encountered: " << lex << std::endl;
		return false;
	}
}

bool Parser::VarList()
{
	if (!("identifier" == tok)) {
		std::cout << "Invalid identifier in variable list: " << lex << std::endl;
		return false;
	} else {
//		ident();
		for (int i=0; i < funcs.size(); i++) {
			if (lex == funcs[i].id) {
				std::cout << "Error: variable '" << lex << "' already declared as a function.\n";
				return false;
			}
		}
		for (int i=0; i < symbols.size(); i++) {
			if (lex == symbols[i].id) {
				std::cout << "Error: variable '" << lex << "' declared more than once.\n";
				return false;
			}
		}
		symbols.push_back(Symbol(s_curr_addr, lex, curr_type));
		++s_curr_addr;
		L.getTokLex(tok,lex);
	}
	
	if (!OptVars()) return false;
	
//	std::cout << "VarList => ident OptVars\n";
	return true;
}

bool Parser::OptVars()
{
	if (";" == lex) {
//		std::cout << "OptVars => null\n";
		return true;
	}
	
	if (!("," == lex)) {
		std::cout << "Variable list not delimited by ','\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}

	if (!("identifier" == tok)) {
		std::cout << "Invalid identifier in variable list\n";
		return false;
	} else {
//		ident();
		symbols.push_back(Symbol(s_curr_addr, lex, curr_type));
		++s_curr_addr;
		L.getTokLex(tok,lex);
	}
	
	if (!OptVars()) return false;
	
//	std::cout << "OptVars => , ident OptVars\n";
	return true;
}

bool Parser::OptStmtList()
{
	if ("end" == lex) {
//		std::cout << "OptStmtList => null\n";
		return true;
	}
	
	if (!Stmt()) return false;
	if (!OptStmtList()) return false;
	
//	std::cout << "OptStmtList => Stmt OptStmtList\n";
	return true;
}

bool Parser::Stmt()
{
	if ("identifier" == tok) {
		if (!Assign()) return false;
		else {
//			std::cout << "Stmt => Assign\n";
			return true;
		}
	} else if ("read" == lex) {
		if (!Read()) return false;
		else {
//			std::cout << "Stmt => Read\n";
			return true;
		}
	} else if ("write" == lex) {
		if (!Write()) return false;
		else {
//			std::cout << "Stmt => Write\n";
			return true;
		}
	} else if ("if" == lex) {
		if (!If()) return false;
		else {
//			std::cout << "Stmt => If\n";
			return true;
		}
	} else if ("while" == lex) {
		if (!While()) return false;
		else {
//			std::cout << "Stmt => While\n";
			return true;
		}
	} else {
		std::cout << "Error with statement\n";
		return false;
	}
}

bool Parser::ReadList()
{
	if (!("identifier" == tok)) {
		std::cout << "Invalid identifier in variable list: " << lex << std::endl;
		return false;
	} else {
//		ident();
		int a;
		if (!get_symbol_addr(lex, a)) {
			std::cout << "Error: Variable '" << lex << "' not declared.\n";
			return false;
		}
		assembly.push_back(icode(a_line,"PUSHI"));
		++a_line;
		assembly.push_back(icode(a_line,"POPM",std::to_string(a)));
		++a_line;
		L.getTokLex(tok,lex);
	}
	
	if (!OptReads()) return false;
	
//	std::cout << "VarList => ident OptVars\n";
	return true;
}

bool Parser::OptReads()
{
	if (")" == lex) {
//		std::cout << "OptVars => null\n";
		return true;
	}
	
	if (!("," == lex)) {
		std::cout << "Variable list not delimited by ','\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}

	if (!("identifier" == tok)) {
		std::cout << "Invalid identifier in variable list\n";
		return false;
	} else {
//		ident();
		int a;
		if (!get_symbol_addr(lex, a)) {
			std::cout << "Error: Variable '" << lex << "' not declared.\n";
			return false;
		}
		assembly.push_back(icode(a_line,"PUSHI"));
		++a_line;
		assembly.push_back(icode(a_line,"POPM",std::to_string(a)));
		++a_line;
		L.getTokLex(tok,lex);
	}
	
	if (!OptReads()) return false;
	
//	std::cout << "OptVars => , ident OptVars\n";
	return true;
}

bool Parser::Assign()
{
	int rhs;
	if (!("identifier" == tok)) {
		std::cout << "Expected identifier\n";
		return false;
	} else {
//		ident();
		if (!get_symbol_addr(lex, rhs)) {
			std::cout << "Error: Variable '" << lex << "' not declared.\n";
			return false;
		}
		L.getTokLex(tok,lex);
	}
	
	if (!("<-" == lex)) {
		std::cout << "Expected assignment, missing '<-'\n" << lex;
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	
	if (!Expr()) return false;
	
	assembly.push_back(icode(a_line,"POPM",std::to_string(rhs)));
	++a_line;
	
	if (!(";" == lex)) {
		std::cout << "Assignment not terminated by';'\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	
//	std::cout << "Assign => ident <- Expr ;\n";
	return true;
}

bool Parser::Read()
{
	if (!("read" == lex)) {
		std::cout << "Expected read\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	
	if (!("(" == lex)) {
		std::cout << "'read' keyword not followed by '('\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	
	if (!ReadList()) return false;
	
	if (!(")" == lex)) {
		std::cout << "Argument list in Read not followed by ')'\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	
	if (!(";" == lex)) {
		std::cout << "Read statement not ended with ';'\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	
//	std::cout << "Read => read ( VarList ) ;\n";
	return true;
}

bool Parser::Write()
{
	if (!("write" == lex)) {
		std::cout << "Expected write\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	
	if (!("(" == lex)) {
		std::cout << "'write' keyword not followed by '('\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	
	if (!Expr()) return false;
	
	assembly.push_back(icode(a_line, "POPO"));
	++a_line;
	
	if (!(")" == lex)) {
		std::cout << "Expression in Write not followed by ')'\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	
	if (!(";" == lex)) {
		std::cout << "Write statement not ended with ';'\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	
//	std::cout << "Write => write ( Expr ) ;\n";
	return true;
}

bool Parser::If()
{
// watch out for pointer magic in these 'if' 'elseif' 'else' functions
	if (!("if" == lex)) {
		std::cout << "Expected 'if'\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	
	if (!("(" == lex)) {
		std::cout << "if keyword not followed by '('\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	
	if (!Condition()) return false;
	
	if (!(")" == lex)) {
		std::cout << "if condition not followed by ')'\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	
	if (!("begin" == lex)) {
		std::cout << "if body does not begin with keyword 'begin'\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	
	assembly.push_back(icode(a_line, "JMPZ"));
	int jz = a_line-1;
	++a_line;
	int *jzp = &(jz);
	
	if (!OptStmtList()) return false;
	
	if (!("end" == lex)) {
		std::cout << "if body does not end with keyword 'end'\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}

	std::vector< int > jumps;
	std::vector< int > *jp = &jumps;
	
	if (!OptElseIf(jzp, jp)) return false;
	if (!OptElse(jz)) return false;
	
	for (int i=0; i < jumps.size(); i++) {
		assembly[jumps[i]].addr = std::to_string(a_line);
	}
	
//	std::cout << "If => if ( Condition ) begin OptStmtList end OptElseIf OptElse\n";
	return true;
}

bool Parser::OptElseIf(int* jzp, std::vector< int > *jp)
{
	if (!("elseif" == lex)) {
//		std::cout << "OptElseIf => null\n";
		return true;
	} else {
		L.getTokLex(tok,lex);
	}	
	
	assembly.push_back(icode(a_line,"JUMP"));
	(*jp).push_back(a_line-1);
	++a_line;
	assembly[(*jzp)].addr = std::to_string(a_line);
	
	if (!("(" == lex)) {
		std::cout << "elseif keyword not followed by '('\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	
	if (!Condition()) return false;
	
	if (!(")" == lex)) {
		std::cout << "elseif condition not followed by ')'\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	
	if (!("begin" == lex)) {
		std::cout << "elseif body does not begin with keyword 'begin'\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	
	assembly.push_back(icode(a_line, "JMPZ"));
	*jzp = a_line-1;
	++a_line;
	
	if (!OptStmtList()) return false;
	
	if (!("end" == lex)) {
		std::cout << "elseif body does not end with keyword 'end'\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	
	if (!OptElseIf(jzp, jp)) return false;
	
//	std::cout << "OptElseIf => elseif ( Condition ) begin OptStmtList end OptElseIf\n";
	return true;
}

bool Parser::OptElse(int jz)
{
	if (!("else" == lex)) {
//		std::cout << "OptElse => null\n";
		assembly[jz].addr = std::to_string(a_line);
		return true;
	} else {
		L.getTokLex(tok,lex);
	}
	
	if (!("begin" == lex)) {
		std::cout << "else body does not begin with keyword 'begin'\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	
	assembly.push_back(icode(a_line, "JUMP"));
	int jump_line = a_line-1;
	++a_line;
	assembly[jz].addr = std::to_string(a_line);
	
	if (!OptStmtList()) return false;
	
	if (!("end" == lex)) {
		std::cout << "else body does not end with keyword 'end'\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	
	assembly[jump_line].addr = std::to_string(a_line);
	
//	std::cout << "OptElse => else begin OptStmtList end\n";
	return true;
}

bool Parser::While()
{
	if (!("while" == lex)) {
		std::cout << "Expected 'while'\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	
	if (!("(" == lex)) {
		std::cout << "while keyword not followed by '('\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	
	int cond_line = a_line;
	
	if (!Condition()) return false;
	
	if (!(")" == lex)) {
		std::cout << "while condition not followed by ')'\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	
	if (!("begin" == lex)) {
		std::cout << "while body does not begin with keyword 'begin'\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	
	assembly.push_back(icode(a_line, "JMPZ"));
	int jz_instr = a_line-1;
	++a_line;
	
	if (!OptStmtList()) return false;
	
	assembly.push_back(icode(a_line,"JUMP",std::to_string(cond_line)));
	++a_line;
	assembly[jz_instr].addr = std::to_string(a_line);
	
	if (!("end" == lex)) {
		std::cout << "while body does not end with keyword 'end'\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	
//	std::cout << "While => while ( Condition ) begin OptStmtList end\n";
	return true;
}

bool Parser::Condition()
{
	if (!Expr()) return false;
	
	if (!RelOp()) return false;
	
	if (!Expr()) return false;
	
	if (curr_relop == "<") {
		assembly.push_back(icode(a_line, "LESS"));
		++a_line;
	} else if (curr_relop == ">") {
		assembly.push_back(icode(a_line, "GRTR"));
		++a_line;
	} else if (curr_relop == "=") {
		assembly.push_back(icode(a_line, "EQL"));
		++a_line;
	} else if (curr_relop == "<=") {
		assembly.push_back(icode(a_line, "GRTR"));
		++a_line;
		assembly.push_back(icode(a_line, "NOT"));
		++a_line;
	} else if (curr_relop == ">=") {
		assembly.push_back(icode(a_line, "LESS"));
		++a_line;
		assembly.push_back(icode(a_line, "NOT"));
		++a_line;
	} else if (curr_relop == "<>") {
		assembly.push_back(icode(a_line, "EQL"));
		++a_line;
		assembly.push_back(icode(a_line, "NOT"));
		++a_line;
	} else {
		std::cout << "Invalid relation encountered: " << curr_relop << std::endl;
		return false;
	}
	
//	std::cout << "Condition => Expr RelOp Expr\n";
	return true;
}

bool Parser::RelOp()
{
	if ("rel op"  == tok) {
//		std::cout << "RelOp => " << lex << std::endl;
		curr_relop = lex;
		L.getTokLex(tok,lex);
		return true;
	}
	else {
		std::cout << "Invalid relation encountered: " << lex << std::endl;
		return false;
	}
}

bool Parser::Expr()
{
	if (!Term()) return false;
	
	if (!EPrime()) return false;
	
//	std::cout << "Expr => Term EPrime\n";
	return true;
}

bool Parser::EPrime()
{
	bool plus = false;
	if ("+" == lex) {
		plus = true;
		L.getTokLex(tok,lex);
	} else if ("-" == lex) {
		L.getTokLex(tok,lex);
	} else {
//		std::cout << "EPrime => null\n";
		return true;
	}
	
	if (!Term()) return false;

	if (plus) {
		assembly.push_back(icode(a_line, "ADD"));
		++a_line;
	} else {
		assembly.push_back(icode(a_line, "SUB"));
		++a_line;
	}
	
	if (!EPrime()) return false;
	
//	if (plus) {
//		std::cout << "EPrime => + Term EPrime\n";
//	} else {
//		std::cout << "EPrime => - Term EPrime\n";
//	}
	return true;
}

bool Parser::Term()
{
	if (!Factor()) return false;
	
	if (!TPrime()) return false;
	
//	std::cout << "Term => Factor TPrime\n";
	return true;
}

bool Parser::TPrime()
{
	bool div = false;
	if ("*" == lex) {
		L.getTokLex(tok,lex);
	} else if ("/" == lex) {
		div = true;
		L.getTokLex(tok,lex);
	} else {
//		std::cout << "TPrime => null\n";
		return true;
	}
	
	if (!Factor()) return false;
	
	if (!TPrime()) return false;

	if (div) {
		assembly.push_back(icode(a_line, "DIV"));
		++a_line;
	} else {
		assembly.push_back(icode(a_line, "MUL"));
		++a_line;
	}

//	if (div) {
//		std::cout << "TPrime => / Factor TPrime\n";
//	} else {
//		std::cout << "TPrime => * Factor TPrime\n";
//	}
	return true;
}

bool Parser::Factor()
{
	if ("identifier" == tok) { //identifier or function call
//		ident();
		int a;
		if (!get_symbol_addr(lex, a)) {
//++++++++++++++++++++++++++++++++++++++
// check funcs. if found call FuncCall(), else not declared
			std::cout << "Error: Variable '" << lex << "' not declared.\n";
			return false;
		}
		assembly.push_back(icode(a_line,"PUSHM",std::to_string(a)));
		++a_line;
		L.getTokLex(tok,lex);		
//		std::cout << "Factor => ident\n";
		return true;
	} else if ("integer" == tok) { //integer
		assembly.push_back(icode(a_line,"PUSHV",lex));
		++a_line;
//		std::cout << "int = " << lex << std::endl;
		L.getTokLex(tok,lex);
//		std::cout << "Factor => int\n";
		return true;
	} else if ("real number" == tok) { // real
		assembly.push_back(icode(a_line,"PUSHV",lex));
		++a_line;
//		std::cout << "real = " << lex << std::endl;
		L.getTokLex(tok,lex);
//		std::cout << "Factor => real\n";
		return true;
	} else if ("true" == lex) { //true
		assembly.push_back(icode(a_line,"PUSHV","1"));
		++a_line;
		L.getTokLex(tok,lex);
//		std::cout << "Factor => bool\n";
		return true;
	} else if ("false" == lex) { //false
		assembly.push_back(icode(a_line,"PUSHV","0"));
		++a_line;
		L.getTokLex(tok,lex);
//		std::cout << "Factor => bool\n";
		return true;
	} else if ("(" == lex) { // ( Expr )
		L.getTokLex(tok,lex);
		if (!Expr()) return false;
		if (!(")" == lex)) {
			std::cout << "Expression not closed by ')'\n";
			return false;
		} else {
			L.getTokLex(tok,lex);
//			std::cout << "Factor => ( Expr )\n";
			return true;
		}
	} else {
		std::cout << "Error parsing factor\n";
		return false;
	}
}

bool Parser::FuncCall()
{

//++++++++++++++++++++++++++++++++++++++
// Worry about all this later

	if (!("identifier" == tok)) {
		std::cout << "Invalid identifier in function call\n";
		return false;
	} else {
//		ident();
//++++++++++++++++++++++++++++++++++++++
// find function, save identifier
		L.getTokLex(tok,lex);
	}
	
	if (!("(" == lex)) {
		std::cout << "Function identifier not followed by '('\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	
	if (!OptArgList()) return false;
//++++++++++++++++++++++++++++++++++++++
// check that all parameters have been given args
	
	if (!(")" == lex)) {
		std::cout << "Function argument list not followed by ')'\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}


//++++++++++++++++++++++++++++++++++++++
// call function

//	std::cout << "FuncCall => ident ( OptArgList )\n";
	return true;
}

bool Parser::OptArgList()
{
	if (")" == lex) {
//		std::cout << "OptArgList => null\n";
		return true;
	}
	
	if (!Expr()) return false;
//++++++++++++++++++++++++++++++++++++++
//check that Expr type matches param type...

	if (!OptArgs()) return false;
	
//	std::cout << "OptArgList => Expr OptArgs\n";
	return true;
}

bool Parser::OptArgs()
{
	if (")" == lex) {
//		std::cout << "OptArgs => null\n";
		return true;
	}
	
	if (!("," == lex)) {
		std::cout << "Argument list not delimited by ','\n";
		return false;
	} else {
		L.getTokLex(tok,lex);
	}
	
	if (!Expr()) return false;
//++++++++++++++++++++++++++++++++++++++
//check that Expr type matches param type...
	
	if (!OptArgs()) return false;
	
//	std::cout << "OptArgs => , Expr OptArgs\n";
	return true;
}

// function is no longer used...
//void Parser::ident()
//{
//	std::cout << "ident = " << lex << std::endl;
//}

bool Parser::get_symbol_addr(std::string l, int &a)
{
	for (int i=0; i < symbols.size(); i++) {
		if (l == symbols[i].id) {
			a = symbols[i].addr;
			return true;
		}
	}
	return false;
}

void Parser::print_symbols()
{
	std::cout << "Symbol Table:\n";
	for (int i = 0; i < symbols.size(); i++) {
		std::cout << std::setw(6) << symbols[i].id
			<< "  " << symbols[i].addr << std::endl;
	}
}

void Parser::print_assembly()
{
	std::cout << "Assembly code:\n";
	for (int i = 0; i < assembly.size(); i++) {
		std::cout << std::setw(3) << std::right << assembly[i].line << " "
			<< std::setw(8) << std::left << assembly[i].instr
			<< assembly[i].addr << std::endl;
	}
}

void Parser::end_nop()
{
	for (int i = 0; i < assembly.size(); i++) {
		if (assembly[i].instr == "JUMP" ||
			assembly[i].instr == "JMPZ") {
			if (std::to_string(a_line) == assembly[i].addr) {
				assembly.push_back(icode(a_line, "NOP"));
				return;
			}
		}
	}
}
