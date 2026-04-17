#pragma once
#include <string>


enum class TokenType {
	UNKNOWN,

	LEFT_PAREN, RIGHT_PAREN,
	LEFT_BRACKET, RIGHT_BRACKET,

	PLUS, MINUS, SLASH, MOD, STAR, COMMA,
	ASSIGN,
	EQUAL, NOT_EQUAL,
	LESS, LESS_EQUAL,
	GREATER, GREATER_EQUAL,
	AND, OR, NOT, QUESTION,

	IDENTIFIER,
	NUMBER,

	LET, READ, PRINT,
	IF, ELSE, ENDIF,
	WHILE, DONE,
	FUNCTION, RETURN,

	MAX, MIN,

	NEWLINE,
	END_OF_FILE
};

struct Token {
	TokenType type;
	std::string word; 
	double numberValue;
	int line;

	Token(TokenType type, int line, const std::string& word, double numberValue = 0.0) :
		type(type), word(word), numberValue(numberValue), line(line) {}
};