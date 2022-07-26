#include "compiler.h"

#include <functional>

struct ParseRule
{
	std::function<void(Compiler *, bool)> prefix;
	std::function<void(Compiler *, bool)> infix;
	Precedence precedence;
};

ParseRule rules[] = {
	[0] = { 0, 0, PREC_NONE },
	[LEFT_PAREN]      = { &Compiler::grouping, &Compiler::call, PREC_CALL },
	[RIGHT_PAREN]     = { nullptr, nullptr, PREC_NONE },
	[LEFT_BRACE]      = { nullptr, nullptr, PREC_NONE },
	[RIGHT_BRACE]     = { nullptr, nullptr, PREC_NONE },
	[LEFT_BRACKET]    = { &Compiler::array, &Compiler::subscript, PREC_SUBSCRIPT },
	[RIGHT_BRACKET]   = { nullptr, nullptr, PREC_NONE },
	[COMMA]           = { nullptr, nullptr, PREC_NONE },
	[DOT]             = { nullptr, &Compiler::dot, PREC_CALL },
	[MINUS]           = { &Compiler::unary, &Compiler::binary, PREC_TERM },
	[PLUS]            = { nullptr, &Compiler::binary, PREC_TERM },
	[SLASH]           = { nullptr, &Compiler::binary, PREC_FACTOR },
	[STAR]            = { nullptr, &Compiler::binary, PREC_FACTOR },
	[MOD]             = { nullptr, &Compiler::binary, PREC_FACTOR },
	[NOT]             = { nullptr, nullptr, PREC_NONE },
	[EQUAL]           = { nullptr, nullptr, PREC_NONE },
	[GREATER]         = { nullptr, &Compiler::binary, PREC_COMPARISON },
	[LESS]            = { nullptr, &Compiler::binary, PREC_COMPARISON },
	[AND]             = { nullptr, &Compiler::binary, PREC_TERM },
	[PIPE]            = { nullptr, &Compiler::binary, PREC_TERM },
	[BANG]            = { &Compiler::unary, nullptr, PREC_NONE },
	[NEWLINE]         = { nullptr, nullptr, PREC_NONE },
	[NOT_EQUAL]       = { nullptr, &Compiler::binary, PREC_EQUALITY },
	[EQUAL_EQUAL]     = { nullptr, &Compiler::binary, PREC_COMPARISON },
	[GREATER_EQUAL]   = { nullptr, &Compiler::binary, PREC_COMPARISON },
	[LESS_EQUAL]      = { nullptr, &Compiler::binary, PREC_COMPARISON },
	[LESS_LESS]       = { nullptr, nullptr, PREC_NONE },
	[GREATER_GREATER] = { nullptr, nullptr, PREC_NONE },
	[AND_AND]         = { nullptr, &Compiler::binary, PREC_AND },
	[PIPE_PIPE]       = { nullptr, &Compiler::binary, PREC_OR },
	[IDENTIFIER]      = { &Compiler::variable, nullptr, PREC_NONE },
	[STRING]          = { &Compiler::string, nullptr, PREC_NONE },
	[NUMBER]          = { &Compiler::number, nullptr, PREC_NONE },
	[KEY_CLASS]       = { nullptr, nullptr, PREC_NONE },
	[KEY_ELSE]        = { nullptr, nullptr, PREC_NONE },
	[KEY_FALSE]       = { &Compiler::literal, nullptr, PREC_NONE },
	[KEY_FOR]         = { nullptr, nullptr, PREC_NONE },
	[KEY_FN]          = { nullptr, nullptr, PREC_NONE },
	[KEY_IF]          = { nullptr, nullptr, PREC_NONE },
	[KEY_NIL]         = { &Compiler::literal, nullptr, PREC_NONE },
	[KEY_PRINT]       = { nullptr, nullptr, PREC_NONE },
	[KEY_RETURN]      = { nullptr, nullptr, PREC_NONE },
	[KEY_SELF]        = { nullptr, nullptr, PREC_NONE },
	[KEY_SUPER]       = { nullptr, nullptr, PREC_NONE },
	[KEY_TRUE]        = { &Compiler::literal, nullptr, PREC_NONE },
	[KEY_WHILE]       = { nullptr, nullptr, PREC_NONE },
	[TOKEN_EOF]       = { nullptr, nullptr, PREC_NONE },
	[ERROR]           = { nullptr, nullptr, PREC_NONE },
};

static ParseRule *get_rule(TokenType type)
{
	return &rules[type];
}

Compiler::Compiler(const char *src)
	: scanner(src)
{

}

Function Compiler::compile()
{
	functions.push(Function {""});
	advance();

	while (!match(TOKEN_EOF))
		declaration();

	emit_byte(OP_RETURN);
	return functions.top();
}

void Compiler::advance()
{
	previous = current;
	current = scanner.next();
}

void Compiler::consume(TokenType type, const char *msg)
{
	if (current.type() != type)
		error_at_current(msg);
	
	advance();
}

bool Compiler::match(TokenType type)
{
	if (current.type() == type)
	{
		advance();
		return true;
	}

	return false;
}

void Compiler::parse_precedence(Precedence precedence)
{
	advance();
	auto prefix = get_rule(previous.type())->prefix;
	if (!prefix)
	{
		error("Expect expression");
		return;
	}

	auto can_assign = precedence <= PREC_ASSIGNMENT;
	prefix(this, can_assign);

	while (precedence <= get_rule(current.type())->precedence)
	{
		advance();
		auto infix = get_rule(previous.type())->infix;
		infix(this, can_assign);

		if (can_assign && match(EQUAL))
			error("Invalid assignment target");
	}
}

void Compiler::array(bool can_assign)
{
	auto num_elements = 0;

	if (current.type() != RIGHT_BRACKET)
	{
		do
		{
			num_elements += 1;
			expression();
		} while (match(COMMA));
	}

	consume(RIGHT_BRACKET, "Expect ']' after array literal");
	emit_bytes(OP_BUILD_ARRAY, num_elements);
}

void Compiler::binary(bool can_assign)
{
	auto op = previous.type();
	auto precedence = get_rule(op)->precedence;
	parse_precedence(precedence);

	switch (op)
	{
		case PLUS:
			emit_byte(OP_ADD);
			break;
		case MINUS:
			emit_byte(OP_SUBTRACT);
			break;
		case STAR:
			emit_byte(OP_MULTIPLY);
			break;
		case SLASH:
			emit_byte(OP_DIVIDE);
			break;
		case MOD:
			emit_byte(OP_MOD);
			break;
		case NOT_EQUAL:
			emit_bytes(OP_EQUAL, OP_NOT);
			break;
		case EQUAL_EQUAL:
			emit_byte(OP_EQUAL);
			break;
		case GREATER:
			emit_byte(OP_GREATER);
			break;
		case GREATER_EQUAL:
			emit_bytes(OP_LESS, OP_NOT);
			break;
		case LESS:
			emit_byte(OP_LESS);
			break;
		case LESS_EQUAL:
			emit_bytes(OP_GREATER, OP_NOT);
			break;
		case AND:
			emit_byte(OP_BITWISE_AND);
			break;
		case PIPE:
			emit_byte(OP_BITWISE_OR);
			break;
		case AND_AND:
			emit_byte(OP_LOGICAL_AND);
			break;
		case PIPE_PIPE:
			emit_byte(OP_LOGICAL_OR);
			break;
	}
}

void Compiler::call(bool can_assign)
{
	auto num_arguments = 0;

	if (current.type() != RIGHT_PAREN)
	{
		do
		{
			num_arguments += 1;
			expression();
		} while (match(COMMA));
	}

	consume(RIGHT_PAREN, "Expect ')' after arguments");
	emit_bytes(OP_CALL, num_arguments);
}

void Compiler::dot(bool can_assign)
{
	consume(IDENTIFIER, "Expect identifier after '.'");
	auto constant = make_constant(Value(previous.value()));

	if (can_assign && match(EQUAL))
	{
		expression();
		emit_bytes(OP_SET_PROPERTY, constant);
	}
	
	else
	{
		emit_bytes(OP_GET_PROPERTY, constant);
	}
}

void Compiler::grouping(bool can_assign)
{
	expression();
	consume(RIGHT_PAREN, "Expect ')' after expression");
}

void Compiler::literal(bool can_assign)
{
	switch (previous.type())
	{
		case KEY_FALSE:
			emit_byte(OP_FALSE);
			break;
		case KEY_NIL:
			emit_byte(OP_NIL);
			break;
		case KEY_TRUE:
			emit_byte(OP_TRUE);
			break;
	}
}

void Compiler::number(bool can_assign)
{
	auto d = std::stod(previous.value());
	emit_constant(Value(d));
}

void Compiler::string(bool can_assign)
{
	auto str = previous.value();
	emit_constant(Value(str.substr(1, str.size() - 2)));
}

void Compiler::subscript(bool can_assign)
{
	expression();
	consume(RIGHT_BRACKET, "Expect ']' after index");

	if (can_assign && match(EQUAL))
	{
		expression();
		emit_byte(OP_SET_SUBSCRIPT);
	}
	
	else
	{
		emit_byte(OP_GET_SUBSCRIPT);
	}
}

void Compiler::unary(bool can_assign)
{
	auto op = previous.type();
	parse_precedence(PREC_UNARY);

	switch (op)
	{
		case MINUS:
			emit_byte(OP_NEGATE);
			break;
		case BANG:
			emit_byte(OP_NOT);
			break;
	}
}

void Compiler::variable(bool can_assign)
{
	auto identifier = previous;

	Opcode get_op, set_op;
	int value = resolve_local(identifier);

	if (value == -1)
	{
		get_op = OP_GET_GLOBAL;
		set_op = OP_SET_GLOBAL;
		value = make_constant(identifier.value());
	}

	else
	{
		get_op = OP_GET_LOCAL;
		set_op = OP_SET_LOCAL;
	}


	if (can_assign && match(EQUAL))
	{
		expression();
		emit_bytes(set_op, value);
	}

	else
	{
		emit_bytes(get_op, value);
	}
}


void Compiler::expression()
{
	parse_precedence(PREC_ASSIGNMENT);
}

void Compiler::declaration()
{
	statement();
}

void Compiler::statement()
{
	switch (current.type())
	{
		case KEY_PRINT:
			advance();
			print_statement();
			break;
		case LEFT_BRACE:
			advance();
			begin_scope();
			block();
			end_scope();
			break;
		case KEY_IF:
			advance();
			if_statement();
			break;
		case KEY_WHILE:
			advance();
			while_statement();
			break;
		case KEY_FN:
			advance();
			function_definition();
			break;
		case KEY_RETURN:
			advance();
			return_statement();
			break;
		case KEY_CLASS:
			advance();
			class_declaration();
			break;
		default:
			expression_statement();
	}
}

void Compiler::expression_statement()
{
	expression();
}

void Compiler::block()
{
	while (current.type() != RIGHT_BRACE)
		declaration();
	
	consume(RIGHT_BRACE, "Expect '}' after block");
}

void Compiler::print_statement()
{
	expression();
	emit_byte(OP_PRINT);
}

void Compiler::if_statement()
{
	expression();
	auto then_offset = emit_jump(OP_JUMP_IF_FALSE);
	emit_byte(OP_POP);
	statement();

	auto else_offset = emit_jump(OP_JUMP);
	emit_byte(OP_POP);
	patch_jump(then_offset);

	if (match(KEY_ELSE))
		statement();
	
	patch_jump(else_offset);
}

void Compiler::while_statement()
{
	auto loop_start = functions.top().chunk.size();
	expression();
	auto exit_offset = emit_jump(OP_JUMP_IF_FALSE);
	emit_byte(OP_POP);
	statement();
	emit_loop(loop_start);

	patch_jump(exit_offset);
	emit_byte(OP_POP);
}

void Compiler::return_statement()
{
    // TODO: don't parse expression if return is followed immediately by \n
	expression();
	emit_byte(OP_RETURN);
}

void Compiler::function_definition()
{
	auto name = current.value();

	auto fn = Function { name };
	functions.push(fn);
	advance();

	begin_scope();
	consume(LEFT_PAREN, "Expect '(' after function name");

	int num_params = 0;
	while (!match(RIGHT_PAREN))
	{
		num_params += 1;
		add_local(current);
		consume(IDENTIFIER, "Expect identifier");
	}

	functions.top().num_params = num_params;
	consume(LEFT_BRACE, "Expect '{' before function body");
	block();

	if (functions.top().chunk.code.empty())
		emit_constant(Value(nullptr));
	
	emit_byte(OP_RETURN);
	fn = functions.top();

	#ifdef DEBUG
	fn.chunk.disassemble(fn.name.c_str());
	#endif

	functions.pop();
	auto global = make_constant(Value(std::make_shared<Function>(fn)));
	emit_bytes(OP_SET_GLOBAL, global);
}

void Compiler::class_declaration()
{
	consume(IDENTIFIER, "Expect class name");
	auto klass = make_constant(Value(previous.value()));

	emit_bytes(OP_CLASS, klass);
	emit_bytes(OP_SET_GLOBAL, klass);

	consume(LEFT_BRACE, "Expect '{' before class body");
	consume(RIGHT_BRACE, "Expect '}' after class body");
}

size_t Compiler::make_constant(Value value)
{
	auto constant = functions.top().chunk.add_constant(value);
	if (constant > 0xff)
	{
		error("Too many constants in this chunk");
		return 0;
	}

	return constant;
}

void Compiler::emit_byte(u8 byte)
{
	functions.top().chunk.write(byte, previous.line());
}

void Compiler::emit_bytes(u8 a, u8 b)
{
	emit_byte(a);
	emit_byte(b);
}

void Compiler::emit_constant(Value value)
{
	auto constant = make_constant(value);
	emit_bytes(OP_CONSTANT, constant);
}

size_t Compiler::emit_jump(Opcode op)
{
	emit_byte(op);
	emit_bytes(0xff, 0xff);
	return functions.top().chunk.size() - 2;
}

void Compiler::patch_jump(size_t offset)
{
	auto jump = functions.top().chunk.size() - offset - 2;
	if (jump > 0xffff)
		error("Jump is out of bounds");
	
	functions.top().chunk.code[offset] = (jump >> 8) & 0xff;
	functions.top().chunk.code[offset + 1] = jump & 0xff;
}

void Compiler::emit_loop(size_t loop_start)
{
	emit_byte(OP_LOOP);
	auto offset = functions.top().chunk.size() - loop_start + 2;
	if (offset > 0xffff)
		error("Loop offset is out of bounds");

	emit_byte((offset >> 8) & 0xff);
	emit_byte(offset & 0xff);
}

void Compiler::error(const char *msg)
{
	error_at(previous, msg);
}

void Compiler::error_at_current(const char *msg)
{
	error_at(current, msg);
}

void Compiler::error_at(Token t, const char *msg)
{
	std::cout << "[line " << t.line() << " ] Error: " << msg << "\n";
}

void Compiler::add_local(Token t)
{
	auto local = Local { t, -1 };
	functions.top().locals.push_back(local);
}

int Compiler::resolve_local(Token t)
{
	auto local_count = functions.top().locals.size();
	for (int i = local_count - 1; i >= 0; i--)
	{
		auto local = functions.top().locals[i];
		if (t.value() == local.name.value())
			return i + 1;
	}

	return -1;
}

void Compiler::begin_scope()
{
	functions.top().scope_depth += 1;
}

void Compiler::end_scope()
{
	functions.top().scope_depth -= 1;
	auto local_count = functions.top().locals.size();

	while (local_count > 0 && functions.top().locals[local_count - 1].depth > functions.top().scope_depth)
	{
		emit_byte(OP_POP);
		local_count -= 1;
	}
}
