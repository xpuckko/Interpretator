#pragma once
#include "Token.hpp"
#include <fstream>
#include <vector>

class Lexer {
public:
	Lexer() : current(0), line(1) {}

	void readFromString(const std::string& source);
	void readFromStream(std::istream& input);

	const std::vector<Token>& getTokens() const { return tokens; }
private:
	void scanTokens();

	char advance();
	char peek() const;
	char peekNext() const;
	bool isAtEnd() const;
	void addToken(TokenType type, const std::string& word, double value = 0.0);

	void number();
	void identifierOrKeyword();
	void operatorOrSymbol();

	void error(int line, const std::string& text);

	bool isNewLine(char c) const;
	bool isOperatorOrSymbol(char c) const;
	bool isWhiteSpace(char c) const;
private:
	std::string source;
	std::vector<Token> tokens;
	int current;
	int line;
};