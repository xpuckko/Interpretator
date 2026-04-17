#include "Lexer.hpp"
#include <cctype>
#include <iostream>


void Lexer::readFromString(const std::string& source)
{
    this->source = source;
    current = 0;
    line = 1;
    tokens.clear();

    scanTokens();
}

void Lexer::readFromStream(std::istream& input)
{
    std::string src;
    std::string line;
    while (std::getline(input, line)) {
        src += line + "\n";
    }

    readFromString(src);
}

void Lexer::scanTokens()
{
	while (!isAtEnd()) {
		char c = advance();
		
		if (isWhiteSpace(c)) continue;
        else if (isNewLine(c)) { addToken(TokenType::NEWLINE, "\\n"); ++line; }
		else if (std::isdigit(c)) number();
		else if (std::isalpha(c) || c=='_') identifierOrKeyword();
		else if (isOperatorOrSymbol(c)) operatorOrSymbol();
		else error(line, "Unexpected character" + c);
	}
}

char Lexer::advance()
{
	if (isAtEnd()) return '\0';
	return source[current++];
}

char Lexer::peek() const
{
	if (isAtEnd()) return '\0';
	return source[current];
}

char Lexer::peekNext() const
{
	if (current + 1 >= source.size()) return'\0';
	return source[current+1];
}

bool Lexer::isAtEnd() const
{
	return current>=source.size();
}

void Lexer::addToken(TokenType type, const std::string& word, double value)
{
	Token t(type, line, word, value);
	tokens.push_back(t);
}

void Lexer::number()
{
	int start = current - 1;

	while (!isAtEnd() && (std::isdigit(peek()) || peek() == '.')) advance();

	std::string num = source.substr(start, current - start);

	double val = std::stod(num);

	addToken(TokenType::NUMBER, num, val);
}

void Lexer::identifierOrKeyword()
{
	int start = current - 1;

	while (!isAtEnd() && (std::isalnum(peek()) || peek() == '_'))
		advance();

	std::string text = source.substr(start, current - start);

	TokenType type = TokenType::IDENTIFIER;

	if (text == "LET") type = TokenType::LET;
	else if (text == "PRINT") type = TokenType::PRINT;
	else if (text == "READ") type = TokenType::READ;
	else if (text == "IF") type = TokenType::IF;
	else if (text == "ENDIF") type = TokenType::ENDIF;
	else if (text == "ELSE") type = TokenType::ELSE;
	else if (text == "WHILE") type = TokenType::WHILE;
	else if (text == "DONE") type = TokenType::DONE;
	else if (text == "FUNCTION") type = TokenType::FUNCTION;
	else if (text == "RETURN") type = TokenType::RETURN;
	
	addToken(type, text);
}

void Lexer::operatorOrSymbol()
{
    char c = source[current - 1];

    std::string op;
    TokenType type=TokenType::UNKNOWN;

    switch (c)
    {
    case '=':
        if (peek() == '=') {
            advance();
            op = "==";
            type = TokenType::EQUAL;
        }
        else {
            op = "=";
            type = TokenType::ASSIGN;
        }
        break;

    case '!':
        if (peek() == '=') {
            advance();
            op = "!=";
            type = TokenType::NOT_EQUAL;
        }
        else {
            op = "!";
            type = TokenType::NOT;
        }
        break;

    case '<':
        if (peek() == '=') {
            advance();
            op = "<=";
            type = TokenType::LESS_EQUAL;
        }
        else {
            op = "<";
            type = TokenType::LESS;
        }
        break;

    case '>':
        if (peek() == '=') {
            advance();
            op = ">=";
            type = TokenType::GREATER_EQUAL;
        }
        else {
            op = ">";
            type = TokenType::GREATER;
        }
        break;

    case '&':
        if (peek() == '&') {
            advance();
            op = "&&";
            type = TokenType::AND;
        }
        break;

    case '|':
        if (peek() == '|') {
            advance();
            op = "||";
            type = TokenType::OR;
        }
        break;

    case '+': op = "+"; type = TokenType::PLUS; break;
    case '-': op = "-"; type = TokenType::MINUS; break;
    case '*': op = "*"; type = TokenType::STAR; break;
    case '/': op = "/"; type = TokenType::SLASH; break;
    case '%': op = "%"; type = TokenType::MOD; break;
    case '(': op = "("; type = TokenType::LEFT_PAREN; break;
    case ')': op = ")"; type = TokenType::RIGHT_PAREN; break;
    case '[': op = "["; type = TokenType::LEFT_BRACKET; break;
    case ']': op = "]"; type = TokenType::RIGHT_BRACKET; break;
    case ',': op = ","; type = TokenType::COMMA; break;

    default:
        error(line, "Unknown operator: " + std::string(1, c));
        return;
    }

    addToken(type, op);
}

void Lexer::error(int line, const std::string& text)
{
    throw std::logic_error("Line " + std::to_string(line) + ": " + text);
}


bool Lexer::isNewLine(char c) const
{
	return c=='\n';
}

bool Lexer::isOperatorOrSymbol(char c) const
{
	return c=='=' || c=='<' || c=='>' || c==',' ||
		c=='+' || c=='*' || c=='-' || c == '/' || c == '%' ||
		c=='&' || c=='|' || c=='!' ||
		c=='(' || c== ')' || c=='[' || c==']';
}

bool Lexer::isWhiteSpace(char c) const
{
    return c==' ' || c=='\r' || c=='\t';
}
