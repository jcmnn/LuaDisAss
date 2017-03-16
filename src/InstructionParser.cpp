#include "InstructionParser.h"
#include "opcodes.h"
#include "Function.h"

#include <iostream>
#include <algorithm>

//#define IHINTS // show instruction hints as a comment


InstructionParser::InstructionParser(Function *function, const std::vector<Instruction> &code) : function_(function), code_(code) {

}

InstructionParser::InstructionParser(Function *function, std::vector<Instruction> &&code) : function_(function), code_(std::move(code)) {

}

Util::BoolRes InstructionParser::parse() {
	std::stringstream opout;

	std::vector<std::string> lines;
	std::vector<int> locations;

	for (auto it = code_.begin(); it != code_.end(); it++) {
		opout.str("");
		opout << luaP_opnames[GET_OPCODE(*it)];

		short a = GETARG_A(*it);

		switch (GET_OPCODE(*it)) {
			case OP_MOVE:
				opout << " %" << a;
				opout << " %" << GETARG_B(*it);

				#ifdef IHINTS
				opout << "\t\t\t ; dst, src";
				#endif
				break;
			case OP_LOADK:
				opout << " %" << a;
				opout << " const " << function_->constant(INDEXK(GETARG_Bx(*it)))->str();

				#ifdef IHINTS
				opout << "\t\t\t ; dst, const";
				#endif
				break;
			case OP_LOADKX:
				opout << " %" << a;
				if (GET_OPCODE(*it++) != OP_EXTRAARG) {
					return Util::BoolRes(false, "OP_LOADKK needs to be proceded by an OP_EXTRAARG");
				}
				opout << " const " << function_->constant(INDEXK(GETARG_Ax(*it)))->str();

				#ifdef IHINTS
				opout << "\t\t\t ; (load extended: uses OP_EXTRAARG) dst, const";
				#endif
				break;
			case OP_LOADBOOL: {
				opout << " %" << a;
				int b = GETARG_B(*it);
				opout << " " << (b == 0 ? "false" : "true");
				opout << " " << GETARG_C(*it);

				#ifdef IHINTS
				opout << "\t\t\t ; dst, src, skip (if skip != 0, skip next instruction)";
				#endif
				break;
			}
			case OP_LOADNIL:
				opout << " %" << a;
				opout << " " << GETARG_B(*it);

				#ifdef IHINTS
				opout << "\t\t\t ; dst, amount (amount = amount of bytes to set i.e. dst...dst+amount";
				#endif
				break;
			case OP_GETUPVAL: {
				opout << " %" << a;
				opout << " @" << GETARG_B(*it);
				Upvalue *upval = function_->upvalue(GETARG_B(*it));
				if (upval != nullptr) {
					opout << " ; debug name: " << upval->name;
				}

				#ifdef IHINTS
				opout << "\t\t\t ; dst, upidx";
				#endif
			}
			case OP_GETTABUP: {
				opout << " %" << a;
				opout << " @" << GETARG_B(*it);
				if (ISK(GETARG_C(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_C(*it)))->str();
				} else {
					opout << " %" << GETARG_C(*it);
				}
				Upvalue *upval = function_->upvalue(GETARG_B(*it));
				if (upval != nullptr) {
					opout << " ; debug name: " << upval->name;
				}

				#ifdef IHINTS
				opout << "\t\t\t ; dst, upidx, key (upidx must be the index of a table, key can be a stack index or constant)";
				#endif
				break;
			}
			case OP_GETTABLE: {
				opout << " %" << a;
				opout << " %" << GETARG_B(*it);
				if (ISK(GETARG_C(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_C(*it)))->str();
				} else {
					opout << " %" << GETARG_C(*it);
				}

				#ifdef IHINTS
				opout << "\t\t\t ; dst, tabidx, key (key can be a stack index or constant)";
				#endif
				break;
			}
			case OP_SETTABUP: {
				opout << " @" << a;
				if (ISK(GETARG_B(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_B(*it)))->str();
				} else {
					opout << " %" << GETARG_B(*it);
				}

				if (ISK(GETARG_C(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_C(*it)))->str();
				} else {
					opout << " %" << GETARG_C(*it);
				}


				Upvalue *upval = function_->upvalue(GETARG_A(*it));
				if (upval != nullptr) {
					opout << " ; debug name: " << upval->name;
				}

				#ifdef IHINTS
				opout << "\t\t\t ; upidx, key, val (upidx must index a table, key and val can be stack indexes or consants)";
				#endif
				break;
			}
			case OP_SETUPVAL: {
				opout << " @" << GETARG_B(*it);
				opout << " %" << a;

				Upvalue *upval = function_->upvalue(GETARG_B(*it));
				if (upval != nullptr) {
					opout << " ; debug name: " << upval->name;
				}

				#ifdef IHINTS
				opout << "\t\t\t ; upidx, src";
				#endif
				break;
			}
			case OP_SETTABLE: {
				opout << " %" << a;

				if (ISK(GETARG_B(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_B(*it)))->str();
				} else {
					opout << " %" << GETARG_B(*it);
				}

				if (ISK(GETARG_C(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_C(*it)))->str();
				} else {
					opout << " %" << GETARG_C(*it);
				}

				#ifdef IHINTS
				opout << "\t\t\t ; tab, key, val (key and val can be indexes or constants)";
				#endif
				break;
			}
			case OP_NEWTABLE: {
				opout << " %" << a;
				opout << " " << GETARG_B(*it);
				opout << " " << GETARG_C(*it);

				#ifdef IHINTS
				opout << "\t\t\t ; dst, narr, nrec (narr is a hint for how many elements the table will have as a sequence; nrec is a hint for how many other elements the table will have)";
				#endif
				break;
			}
			case OP_SELF: {
				opout << " %" << a;
				opout << " %" << GETARG_B(*it);

				if (ISK(GETARG_C(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_C(*it)))->str();
				} else {
					opout << " %" << GETARG_C(*it);
				}

				#ifdef IHINTS
				opout << "\t\t\t ; dst, tabidx, key (key must be a string)";
				#endif
				break;
			}
			/*case OP_ADD: {
				opout << " %" << a;

				if (ISK(GETARG_B(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_B(*it)))->str();
				} else {
					opout << " %" << GETARG_B(*it);
				}

				if (ISK(GETARG_C(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_C(*it)))->str();
				} else {
					opout << " %" << GETARG_C(*it);
				}

				#ifdef IHINTS
				opout << "\t\t\t ; dst, a, b (a and b can be stack indexes or constants)";
				#endif
				break;
			}
			case OP_SUB: {
				opout << " %" << a;

				if (ISK(GETARG_B(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_B(*it)))->str();
				} else {
					opout << " %" << GETARG_B(*it);
				}

				if (ISK(GETARG_C(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_C(*it)))->str();
				} else {
					opout << " %" << GETARG_C(*it);
				}

				#ifdef IHINTS
				opout << "\t\t\t ; dst, a, b (a and b can be stack indexes or constants)";
				#endif
				break;
			}
			case OP_MUL: {
				opout << " %" << a;

				if (ISK(GETARG_B(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_B(*it)))->str();
				} else {
					opout << " %" << GETARG_B(*it);
				}

				if (ISK(GETARG_C(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_C(*it)))->str();
				} else {
					opout << " %" << GETARG_C(*it);
				}

				#ifdef IHINTS
				opout << "\t\t\t ; dst, a, b (a and b can be stack indexes or constants)";
				#endif
				break;
			}
			case OP_DIV: {
				opout << " %" << a;

				if (ISK(GETARG_B(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_B(*it)))->str();
				} else {
					opout << " %" << GETARG_B(*it);
				}

				if (ISK(GETARG_C(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_C(*it)))->str();
				} else {
					opout << " %" << GETARG_C(*it);
				}

				#ifdef IHINTS
				opout << "\t\t\t ; dst, a, b (a and b can be stack indexes or constants)";
				#endif
				break;
			}
			case OP_BAND: {
				opout << " %" << a;

				if (ISK(GETARG_B(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_B(*it)))->str();
				} else {
					opout << " %" << GETARG_B(*it);
				}

				if (ISK(GETARG_C(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_C(*it)))->str();
				} else {
					opout << " %" << GETARG_C(*it);
				}

				#ifdef IHINTS
				opout << "\t\t\t ; dst, a, b (a and b can be stack indexes or constants)";
				#endif
				break;
			}
			case OP_BOR: {
				opout << " " << a;

				if (ISK(GETARG_B(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_B(*it)))->str();
				} else {
					opout << " " << GETARG_B(*it);
				}

				if (ISK(GETARG_C(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_C(*it)))->str();
				} else {
					opout << " " << GETARG_C(*it);
				}
				break;
			}
			case OP_BXOR: {
				opout << " " << a;

				if (ISK(GETARG_B(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_B(*it)))->str();
				} else {
					opout << " " << GETARG_B(*it);
				}

				if (ISK(GETARG_C(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_C(*it)))->str();
				} else {
					opout << " " << GETARG_C(*it);
				}
				break;
			}
			case OP_SHL: {
				opout << " " << a;

				if (ISK(GETARG_B(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_B(*it)))->str();
				} else {
					opout << " " << GETARG_B(*it);
				}

				if (ISK(GETARG_C(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_C(*it)))->str();
				} else {
					opout << " " << GETARG_C(*it);
				}
				break;
			}
			case OP_SHR: {
				opout << " " << a;

				if (ISK(GETARG_B(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_B(*it)))->str();
				} else {
					opout << " " << GETARG_B(*it);
				}

				if (ISK(GETARG_C(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_C(*it)))->str();
				} else {
					opout << " " << GETARG_C(*it);
				}
				break;
			}
			case OP_MOD: {
				opout << " " << a;

				if (ISK(GETARG_B(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_B(*it)))->str();
				} else {
					opout << " " << GETARG_B(*it);
				}

				if (ISK(GETARG_C(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_C(*it)))->str();
				} else {
					opout << " " << GETARG_C(*it);
				}
				break;
			}
			case OP_IDIV: {
				opout << " " << a;

				if (ISK(GETARG_B(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_B(*it)))->str();
				} else {
					opout << " " << GETARG_B(*it);
				}

				if (ISK(GETARG_C(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_C(*it)))->str();
				} else {
					opout << " " << GETARG_C(*it);
				}
				break;
			}*/
			case OP_ADD:
			case OP_SUB:
			case OP_MUL:
			case OP_DIV:
			case OP_BAND:
			case OP_BOR:
			case OP_BXOR:
			case OP_SHL:
			case OP_SHR:
			case OP_MOD:
			case OP_IDIV:
			case OP_POW: {
				opout << " %" << a;

				if (ISK(GETARG_B(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_B(*it)))->str();
				} else {
					opout << " %" << GETARG_B(*it);
				}

				if (ISK(GETARG_C(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_C(*it)))->str();
				} else {
					opout << " %" << GETARG_C(*it);
				}

				#ifdef IHINTS
				opout << "\t\t\t ; dst, a, b (a and be can be stack indexes or constants)";
				#endif
				break;
			}
			case OP_UNM: {
				opout << " %" << a;
				opout << " %" << GETARG_B(*it);

				#ifdef IHINTS
				opout << "\t\t\t ; dst, a (a can be a stack index, dst = -a)";
				#endif
				break;
			}
			case OP_BNOT: {
				opout << " %" << a;
				opout << " %" << GETARG_B(*it);

				#ifdef IHINTS
				opout << "\t\t\t ; dst, src";
				#endif
				break;
			}
			case OP_NOT: {
				opout << " %" << a;
				opout << " %" << GETARG_B(*it);

				#ifdef IHINTS
				opout << "\t\t\t ; dst, src (dst = not src)";
				#endif
				break;
			}
			case OP_LEN: {
				opout << " %" << a;
				opout << " %" << GETARG_B(*it);

				#ifdef IHINTS
				opout << "\t\t\t ; dst, src";
				#endif
				break;
			}
			case OP_CONCAT: {
				opout << " %" << a;
				opout << " %" << GETARG_B(*it);
				opout << " %" << GETARG_C(*it);

				#ifdef IHINTS
				opout << "\t\t\t ; dst, a, b (concat values from %a..%b and store result is dst)";
				#endif
				break;
			}
			case OP_JMP: {
				opout << " " << a;
				int loc = (it - code_.begin()) + GETARG_sBx(*it) + 1;
				opout << " $location_" << loc;

				locations.push_back(loc);

				#ifdef IHINTS
				opout << "\t\t\t ; a, j (jump j instructions ahead, if a != 0 close upvalues at level base+a-1)";
				#endif
				break;
			}
			case OP_LT:
			case OP_LE:
			case OP_EQ: {
				opout << " " << (a == 0 ? "false" : "true");

				if (ISK(GETARG_B(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_B(*it)))->str();
				} else {
					opout << " %" << GETARG_B(*it);
				}

				if (ISK(GETARG_C(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_C(*it)))->str();
				} else {
					opout << " %" << GETARG_C(*it);
				}

				#ifdef IHINTS
				opout << "\t\t\t ; invert, a, b (a and b can be stack indexes or constants)";
				#endif
				break;
			}
			/*case OP_LT: {
				opout << " " << a;

				if (ISK(GETARG_B(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_B(*it)))->str();
				} else {
					opout << " " << GETARG_B(*it);
				}

				if (ISK(GETARG_C(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_C(*it)))->str();
				} else {
					opout << " " << GETARG_C(*it);
				}
				break;
			}
			case OP_LE: {
				opout << " " << a;

				if (ISK(GETARG_B(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_B(*it)))->str();
				} else {
					opout << " " << GETARG_B(*it);
				}

				if (ISK(GETARG_C(*it))) {
					opout << " const " << function_->constant(INDEXK(GETARG_C(*it)))->str();
				} else {
					opout << " " << GETARG_C(*it);
				}
				break;
			}*/
			case OP_TEST: {
				opout << " %" << a;
				opout << " " << (GETARG_C(*it) == 0 ? "false" : "true");

				#ifdef IHINTS
				opout << "\t\t\t ; a, invert (skips next instruction if a is not false. If invert then skips if a is false)";
				#endif
				break;
			}
			case OP_TESTSET: {
				opout << " %" << a;
				opout << " %" << GETARG_B(*it);
				opout << " " << (GETARG_C(*it) == 0 ? "false" : "true");
				#ifdef IHINTS
				opout << "\t\t\t ; a, b, invert (skips next instruction if a is not false. If invert then skips if a is false. sets a to b when not skipping)";
				#endif
				break;
			}
			case OP_CALL: {
				opout << " %" << a;
				opout << " " << GETARG_B(*it);
				opout << " " << GETARG_C(*it);
				#ifdef IHINTS
				opout  << "\t\t\t ; func, nargs+1, nresults+1";
				#endif
				break;
			}
			case OP_TAILCALL: {
				opout << " %" << a;
				opout << " " << GETARG_B(*it);
				opout << " " << GETARG_C(*it);

				#ifdef IHINTS
				opout  << "\t\t\t ; func, nargs, nresults+1(0) (nresults MUST be LUA_MULTRET (-1))";
				#endif
				break;
			}
			case OP_RETURN: {
				opout << " %" << a;
				opout << " " << GETARG_B(*it);

				#ifdef IHINTS
				opout  << "\t\t\t ; firstRes, nres+1";
				#endif
				break;
			}
			case OP_FORLOOP: {
				opout << " %" << a;
				int loc = (it - code_.begin()) + GETARG_sBx(*it) + 1;
				opout << " $location_" << loc;

				locations.push_back(loc);

				#ifdef IHINTS
				opout  << "\t\t\t ; idx, j (j = amount to jump. must be on stack: idx (increment index), limit, step)";
				#endif
				break;
			}
			case OP_FORPREP: {
				opout << " %" << a;
				int loc = (it - code_.begin()) + GETARG_sBx(*it) + 1;
				opout << " $location_" << loc;

				locations.push_back(loc);

				#ifdef IHINTS
				opout  << "\t\t\t ; init, j (j = amount to jump. must be on stack: init (initial value), limit, step)";
				#endif
				break;
			}
			case OP_TFORCALL: {
				opout << " %" << a;
				opout << " " << GETARG_C(*it);

				#ifdef IHINTS
				opout  << "\t\t\t ; func, nresults (OP_TFORLOOP must be the next instruction)";
				#endif
				break;
			}
			case OP_TFORLOOP: {
				opout << " %" << a;
				int loc = (it - code_.begin()) + GETARG_sBx(*it) + 1;
				opout << " $location_" << loc;

				locations.push_back(loc);

				#ifdef IHINTS
				opout  << "\t\t\t ; func?, j (j = amount to jump)";
				#endif
				break;
			}
			case OP_SETLIST: {
				opout << " %" << a;
				opout << " " << GETARG_B(*it);

				if (GETARG_C(*it) == 0) {
					if (GET_OPCODE(*it++) != OP_EXTRAARG) {
						return Util::BoolRes(false, "OP_SETLIST C=0 needs to be proceded by an OP_EXTRAARG");
					}
					opout << " " << GETARG_Ax(*it);
				} else {
					opout << " " << GETARG_C(*it);
				}

				#ifdef IHINTS
				opout  << "\t\t\t ; table, amount, key (sets table[key] to table+n, where key++ and n++ for amount times. If amount == 0 set all above table in stack)";
				#endif
				break;
			}
			case OP_CLOSURE: {
				opout << " %" << a;
				int index = GETARG_Bx(*it);

				FunctionPtr f = function_->proto(index);
				if (f) {
					opout << " " << f->label();
				} else {
					opout << " " << index << " (invalid)";
				}

				#ifdef IHINTS
				opout  << "\t\t\t ; dst, proto (pushes proto to dst)";
				#endif
				break;
			}
			case OP_VARARG: {
				opout << " %" << a;
				opout << " " << GETARG_B(*it);

				#ifdef IHINTS
				opout  << "\t\t\t ; base, rres+1";
				#endif
				break;
			}
			case OP_EXTRAARG: {
				return Util::BoolRes(false, "OP_EXTRAARG is not a valid opcode");
				break;
			}
		}

		lines.push_back(opout.str());
	}

	for (int i = 0; i < lines.size(); i++) {
		if (!locations.empty()) {
			std::vector<int>::iterator newEnd;
			if (locations.end() != (newEnd = std::remove(locations.begin(), locations.end(), i))) {
				decomp_ << "location_" << i << ":\n";
				locations.erase(newEnd, locations.end());
			}
		}
		decomp_ << "   " << lines[i] << "\n";
	}

	if (!locations.empty()) {
		decomp_ << "; invalid locations:";
		for (int l : locations) {
			decomp_ << " location_" << l;
		}
		decomp_ << "\n";
	}

	return Util::BoolRes(true, "");
}