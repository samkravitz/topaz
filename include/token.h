#pragma once

#include <string>

#include "token_type.h"

struct Token
{
public:
	Token() = default;
	Token(std::string, TokenType, int, int);
	std::string to_string();

	std::string value() { return m_value; }
	TokenType type() { return m_type; }
	int line() { return m_line; }
	int col() { return m_col; }

private:
	std::string m_value;
	TokenType m_type;
	int m_line;
	int m_col;
};
