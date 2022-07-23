#pragma once

enum TokenType : int
{
	// One character tokens
	LEFT_PAREN = 1,    // (
	RIGHT_PAREN,       // )
	LEFT_BRACE,        // {
	RIGHT_BRACE,       // }
	LEFT_BRACKET,      // [
	RIGHT_BRACKET,     // ]
	COMMA,             // ,
	DOT,               // .
	MINUS,             // -
	PLUS,              // +
	SLASH,             // /
	STAR,              // *
	MOD,               // %
	NOT,               // !
	EQUAL,             // =
	GREATER,           // >
	LESS,              // <
	AND,               // &
	PIPE,              // |
	BANG,              // !
	NEWLINE,

	// Two+ character tokens
	NOT_EQUAL,         // !=
	EQUAL_EQUAL,       // ==
	GREATER_EQUAL,     // >=
	LESS_EQUAL,        // <=
	LESS_LESS,         // <<
	GREATER_GREATER,   // >>
	AND_AND,           // &&
	PIPE_PIPE,         // ||

	// Literals
	IDENTIFIER,
	STRING,            // e.g. 'Hello World'
	NUMBER,            // e.g. 3.4, -1

	// Keywords
	KEY_CLASS,
	KEY_ELSE,
	KEY_FALSE,
	KEY_FOR,
	KEY_FN,
	KEY_IF,
	KEY_NIL,
	KEY_PRINT,
	KEY_RETURN,
	KEY_SELF,
	KEY_SUPER,
	KEY_TRUE,
	KEY_WHILE,

	TOKEN_EOF,

	ERROR,
};
