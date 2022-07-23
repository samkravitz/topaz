#pragma once

#include <stack>
#include <vector>

#include "chunk.h"
#include "function.h"
#include "opcode.h"
#include "scanner.h"
#include "token_type.h"
#include "value.h"

enum Precedence : int
{
	PREC_NONE,
	PREC_ASSIGNMENT,
	PREC_OR,
	PREC_AND,
	PREC_EQUALITY,
	PREC_COMPARISON,
	PREC_TERM,
	PREC_FACTOR,
	PREC_UNARY,
	PREC_CALL,
	PREC_SUBSCRIPT,
	PREC_PRIMARY
};

// represents a local variable
struct Local
{
	Token name;
	uint depth;
};

class Compiler
{
public:
	Compiler(const char *);

	Function compile();

	void array(bool);
	void binary(bool);
	void call(bool);
	void grouping(bool);
	void literal(bool);
	void number(bool);
	void string(bool);
	void subscript(bool);
	void unary(bool);
	void variable(bool);

private:
	Scanner scanner;
	Token current;
	Token previous;
	std::stack<Function> functions;
	std::vector<Local> locals;
	uint local_count = 0;
	uint scope_depth = 0;

	void advance();
	void consume(TokenType, const char *);
	bool match(TokenType);

	void parse_precedence(Precedence);

	void expression();
	void declaration();
	void statement();
	void expression_statement();
	void block();
	void print_statement();
	void if_statement();
	void while_statement();
	void return_statement();

	size_t make_constant(Value);
	void emit_byte(u8);
	void emit_bytes(u8, u8);
	void emit_constant(Value);
	size_t emit_jump(Opcode);
	void patch_jump(size_t);
	void emit_loop(size_t);

	void error(const char *);
	void error_at_current(const char *);
	void error_at(Token, const char *);

	void add_local(std::string);
	void resolve_local(Token);
	void begin_scope();
	void end_scope();
};