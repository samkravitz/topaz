#pragma once

#include <list>
#include <sstream>
#include <string>

#include <FlexLexer.h>

#include "token.h"

class Scanner
{
public:
	Scanner(const char *);
	~Scanner();
	void scan();
	std::string to_string();
	Token next();

private:
	std::list<Token> tokens;
	std::istringstream istream;
	FlexLexer *lexer;
};
