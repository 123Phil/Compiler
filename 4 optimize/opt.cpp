// Phillip Stewart, 891499733
// NOV 2014

// compiled on OSX 10.9.5 using g++ which invokes clang, with:
//   $ g++ opt.cpp

// can be run supplying filename as command line argument:
//   ./a.out assembly.txt
// or run and supplied filename during runtime:
//   ./a.out
//  >Enter filename: assembly.txt


#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <fstream>


class icode {
public:
	int line;
	std::string instr;
	int addr;
	bool has_addr;
	icode();
	icode(int l, std::string i, int a=0, bool h=false) {
		line = l;
		instr = i;
		addr = a;
		has_addr = h;
	}
};

class block {
public:
	std::vector< icode > instrs;
	int block_num;
	std::vector<int> pointed_by;
	block() {block_num = 0;}
};

std::vector< icode > read_assembly(std::string filename);
std::vector<int> find_blocks(std::vector< icode > a);
std::vector< block > make_blocks(std::vector< icode > assem, std::vector<int> b_nums);
void optimize(std::vector< block > &blocks);
void print(std::vector< block > blocks);


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

	std::vector< icode > assembly = read_assembly(filename);
	std::vector<int> block_nums = find_blocks(assembly);
	std::vector< block > blocks = make_blocks(assembly, block_nums);
	optimize(blocks);
	print(blocks);
	
	return 0;
}

std::vector< icode > read_assembly(std::string filename)
{
	std::vector< icode > assem;
	std::ifstream fp(filename);
	std::string line, instr;
	int line_num, addr;
	while(getline(fp, line)) {
		std::istringstream ss(line);
		ss >> line_num;
		ss >> instr;
		if (ss >> addr) {
			assem.push_back(icode(line_num, instr, addr, true));
		} else {
			assem.push_back(icode(line_num, instr));
		}
	}
	return assem;
}

std::vector<int> find_blocks(std::vector< icode > a)
{
	// Vector of Basic Block line #s
	// Add beginning of program (line 1)
	std::vector<int> bb;
	bb.push_back(1);
	
	// Add jump targets
	int k;
	for (int i=0; i < a.size(); i++) {
		if (a[i].instr == "JUMP" || a[i].instr == "JMPZ") {
			if (a[i].addr != 1) {
				bb.push_back(a[i].addr);
			}
		}
	}
	
	// Add lines after jumps if not in bb
	bool in_bb;
	for (int i=0; i < a.size(); i++) {
		if (a[i].instr == "JUMP" || a[i].instr == "JMPZ") {
			k = a[i].line + 1;
			in_bb = false;
			for (int j=0; j < bb.size(); j++) {
				if (bb[j] == k) {
					in_bb = true;
					break;
				}
			}
			if (!in_bb) {
				bb.push_back(k);
			}
		}
	}
	std::sort(bb.begin(), bb.end());
	
	return bb;
}

std::vector< block > make_blocks(std::vector< icode > assem, std::vector<int> b_nums)
{
	std::vector< block > blocks;
	b_nums.push_back(assem.size()+1);
	
	//separate & number
	int a_line = 0;
	for (int i=1; i < b_nums.size(); i++) {
		blocks.push_back(block());
		blocks[i-1].block_num = i;
		while (a_line+1 < b_nums[i]) {
			blocks[i-1].instrs.push_back(assem[a_line]);
			++a_line;
		}
	}

	//fix jumps & points
	blocks[0].pointed_by.push_back(0);
	for (int i=0; i < blocks.size(); i++) {
		int target = -1;
		if ("JUMP" == blocks[i].instrs[blocks[i].instrs.size()-1].instr
		 || "JMPZ" == blocks[i].instrs[blocks[i].instrs.size()-1].instr) {
			for (int j=0; j < blocks.size(); j++) {
				if (blocks[i].instrs[blocks[i].instrs.size()-1].addr
						== blocks[j].instrs[0].line) {
					target = blocks[j].block_num;
					break;
				}
//				if (j==blocks.size()-1) {
//					//error, didn't find target
//				}
			}
			blocks[i].instrs[blocks[i].instrs.size()-1].addr = target;
			blocks[target-1].pointed_by.push_back(i+1);
		}
		if (!("JUMP" == blocks[i].instrs[blocks[i].instrs.size()-1].instr)
		 && i != blocks.size()-1) {
			blocks[i+1].pointed_by.push_back(i+1);
		}
	}
	
	return blocks;
}

void optimize(std::vector< block > &blocks)
{
	int i, j, k;
	
	//remove unreachable blocks
	for (i = 0; i < blocks.size(); i++) {
		if (blocks[i].pointed_by.size() == 0) {
			for (j = 0; j < blocks.size(); j++) {
				if (i == j) continue;
				for (k = 0; k < blocks[j].pointed_by.size(); k++) {
					if (blocks[j].pointed_by[k] > i) {
						--blocks[j].pointed_by[k];
					}
				}
				if (blocks[j].instrs[blocks[j].instrs.size()-1].instr == "JUMP"
				 || blocks[j].instrs[blocks[j].instrs.size()-1].instr == "JMPZ") {
					if (blocks[j].instrs[blocks[j].instrs.size()-1].addr > i) {
						--blocks[j].instrs[blocks[j].instrs.size()-1].addr;
					}
				}
				if (j > i) {
					--blocks[j].block_num;
				}
			}
			blocks.erase(blocks.begin() + i);
			break;
		}
	}

	// optimize assembly in each block
	bool changed;
	for (i = 0; i < blocks.size(); i++) {
		changed = true;
//I set the body of this while loop against the margin.
//It's a mess either way...		
		while (changed) {
			changed = false;
			
//try to constant fold in block
if (blocks[i].instrs.size() >= 3) {
	for (j = 2; j < blocks[i].instrs.size(); j++) {
		if (blocks[i].instrs[j-2].instr == "PUSHV"
		 && blocks[i].instrs[j-1].instr == "PUSHV") {
			if (blocks[i].instrs[j].instr == "ADD") {
				blocks[i].instrs[j-2].addr = blocks[i].instrs[j-2].addr + blocks[i].instrs[j-1].addr;
				blocks[i].instrs.erase(blocks[i].instrs.begin() + (j));
				blocks[i].instrs.erase(blocks[i].instrs.begin() + (j-1));
				changed = true;
				break;
			}
			else if (blocks[i].instrs[j].instr == "SUB") {
				blocks[i].instrs[j-2].addr = blocks[i].instrs[j-2].addr - blocks[i].instrs[j-1].addr;
				blocks[i].instrs.erase(blocks[i].instrs.begin() + (j));
				blocks[i].instrs.erase(blocks[i].instrs.begin() + (j-1));
				changed = true;
				break;
			}
			else if (blocks[i].instrs[j].instr == "MUL") {
				blocks[i].instrs[j-2].addr = blocks[i].instrs[j-2].addr * blocks[i].instrs[j-1].addr;
				blocks[i].instrs.erase(blocks[i].instrs.begin() + (j));
				blocks[i].instrs.erase(blocks[i].instrs.begin() + (j-1));
				changed = true;
				break;
			}
			else if (blocks[i].instrs[j].instr == "DIV") {
				blocks[i].instrs[j-2].addr = blocks[i].instrs[j-2].addr / blocks[i].instrs[j-1].addr;
				blocks[i].instrs.erase(blocks[i].instrs.begin() + (j));
				blocks[i].instrs.erase(blocks[i].instrs.begin() + (j-1));
				changed = true;
				break;
			}
		}
	}
}

//try to remove +0,-0,*1,/1
if (blocks[i].instrs.size() >= 2) {
	for (j = 1; j < blocks[i].instrs.size(); j++) {
		if (blocks[i].instrs[j-1].instr == "PUSHV") {
			if (blocks[i].instrs[j-1].addr == 0) {
				if (blocks[i].instrs[j].instr == "SUB"
				 || blocks[i].instrs[j].instr == "ADD") {
					blocks[i].instrs.erase(blocks[i].instrs.begin() + (j));
					blocks[i].instrs.erase(blocks[i].instrs.begin() + (j-1));
					changed = true;
					break;
				}
			} else if (blocks[i].instrs[j-1].addr == 1) {
				if (blocks[i].instrs[j].instr == "MUL"
				 || blocks[i].instrs[j].instr == "DIV") {
					blocks[i].instrs.erase(blocks[i].instrs.begin() + (j));
					blocks[i].instrs.erase(blocks[i].instrs.begin() + (j-1));
					changed = true;
					break;
				}
			}
		}
	}
}
if (blocks[i].instrs.size() >= 3) {
	for (j = 2; j < blocks[i].instrs.size(); j++) {
		if (blocks[i].instrs[j-2].instr == "PUSHV"
			&& (blocks[i].instrs[j-1].instr == "PUSHV" 
				|| blocks[i].instrs[j-1].instr == "PUSHM") ) {
			if (blocks[i].instrs[j-2].addr == 0) {
				if (blocks[i].instrs[j].instr == "ADD") {
					blocks[i].instrs.erase(blocks[i].instrs.begin() + (j));
					blocks[i].instrs.erase(blocks[i].instrs.begin() + (j-2));
					changed = true;
					break;
				}
			} else if (blocks[i].instrs[j-2].addr == 1) {
				if (blocks[i].instrs[j].instr == "MUL") {
					blocks[i].instrs.erase(blocks[i].instrs.begin() + (j));
					blocks[i].instrs.erase(blocks[i].instrs.begin() + (j-2));
					changed = true;
					break;
				}
			}
		}
	}
}
//try to remove unnec stack use
if (blocks[i].instrs.size() >= 2) {
	for (j = 1; j < blocks[i].instrs.size(); j++) {
		if (blocks[i].instrs[j-1].instr == "PUSHM"
		 && blocks[i].instrs[j].instr == "POPM"
		 && blocks[i].instrs[j-1].addr == blocks[i].instrs[j].addr) {
			blocks[i].instrs.erase(blocks[i].instrs.begin() + (j));
			blocks[i].instrs.erase(blocks[i].instrs.begin() + (j-1));
			changed = true;
			break;
		}
	}
}
		}
	}// end of while loop.
	
	// remove empty blocks
	for (i = 0; i < blocks.size(); i++) {
		if (blocks[i].instrs.size() == 0) {
			for (j = 0; j < blocks.size(); j++) {
				if (i == j) continue;
// 				for (k = 0; k < blocks[j].pointed_by.size(); k++) {
// 					if (blocks[j].pointed_by[k] > i) {
// 						--blocks[j].pointed_by[k];
// 					}
// 				}
				if (blocks[j].instrs[blocks[j].instrs.size()-1].instr == "JUMP"
				 || blocks[j].instrs[blocks[j].instrs.size()-1].instr == "JMPZ") {
					if (blocks[j].instrs[blocks[j].instrs.size()-1].addr > i) {
						--blocks[j].instrs[blocks[j].instrs.size()-1].addr;
					}
				}
				if (j > i) {
					--blocks[j].block_num;
				}
			}
			blocks.erase(blocks.begin() + i);
			break;
		}
	}
	
	// fix line #s
	k = 1;
	for (i = 0; i < blocks.size(); i++) {
		for (j = 0; j < blocks[i].instrs.size(); j++) {
			blocks[i].instrs[j].line = k;
			++k;
		}
	}
	
	// fix jumps
	for (i = 0; i < blocks.size(); i++) {
		if ("JUMP" == blocks[i].instrs[blocks[i].instrs.size()-1].instr
		 || "JMPZ" == blocks[i].instrs[blocks[i].instrs.size()-1].instr) {
			blocks[i].instrs[blocks[i].instrs.size()-1].addr
			 = blocks[blocks[i].instrs[blocks[i].instrs.size()-1].addr-1].instrs[0].line;
		}
	}
	
}

void print(std::vector< block > blocks)
{
	for (int i=0; i < blocks.size(); i++) {
		std::cout << "Block " << i+1 << std::endl;
		for (int j=0; j < blocks[i].instrs.size(); j++) {
			std::cout << blocks[i].instrs[j].line << ' ' << blocks[i].instrs[j].instr;
			if (blocks[i].instrs[j].has_addr) {
				std::cout << ' ' << blocks[i].instrs[j].addr << std::endl;
			} else {
				std::cout << std::endl;
			}
		}
		if (i != blocks.size()-1) std::cout << "\n";
	}
}
