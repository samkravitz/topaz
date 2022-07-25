%option noyywrap
%option yylineno

%{
	#include <iostream>
	#include <string>

	#include "token_type.h"
%}

%%

%{
	// Keywords
%}

"class"                     { return KEY_CLASS; }
"else"                      { return KEY_ELSE; }
"false"                     { return KEY_FALSE; }
"for"                       { return KEY_FOR; }
"fn"                        { return KEY_FN; }
"if"                        { return KEY_IF; }
"nil"                       { return KEY_NIL; }
"print"                     { return KEY_PRINT; }
"return"                    { return KEY_RETURN; }
"self"                      { return KEY_SELF; }
"super"                     { return KEY_SUPER; }
"true"                      { return KEY_TRUE; }
"while"                     { return KEY_WHILE; }

%{
	// One character tokens
%}

"("                         { return LEFT_PAREN; }
")"                         { return RIGHT_PAREN; }
"{"                         { return LEFT_BRACE; }
"}"                         { return RIGHT_BRACE; }
"["                         { return LEFT_BRACKET; }
"]"                         { return RIGHT_BRACKET; }
","                         { return COMMA; }
"."                         { return DOT; }
"-"                         { return MINUS; }
"+"                         { return PLUS; }
"/"                         { return SLASH; }
"*"                         { return STAR; }
"%"                         { return MOD; }
"!"                         { return NOT; }
"="                         { return EQUAL; }
">"                         { return GREATER; }
"<"                         { return LESS; }
"&"                         { return AND; }
"!"                         { return BANG; }
"|"                         { return PIPE; }

%{
	// Two+ character tokens
%}

"!="                        { return NOT_EQUAL; }
"=="                        { return EQUAL_EQUAL; }
">="                        { return GREATER_EQUAL; }
"<="                        { return LESS_EQUAL; }
"<<"                        { return LESS_LESS; }
">>"                        { return GREATER_GREATER; }
"&&"                        { return AND_AND; }
"||"                        { return PIPE_PIPE; }

%{
	// Literals
%}

\'([^\\\']|\\n|\\t)*\'      { return STRING; }
\"([^\\\"]|\\n|\\t)*\"      { return STRING; }
-?([0-9]+|[0-9]*\.[0-9]+)   { return NUMBER; }

%{
	// Whitespace / comment
%}

#.*                      { /* Comment */ }
[ \t\n]                  { }

%{
	// Identifier
%}

[a-zA-Z_][a-zA-Z0-9_]*      { return IDENTIFIER; }

%{
	// Unknown Token
%}

.                           { std::cout << "Unknown token: " << yytext << "\n"; }

%%
