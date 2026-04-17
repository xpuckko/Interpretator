#pragma once
#include "Token.hpp"
#include "Expression.hpp"
#include "Statement.hpp"
#include <iostream>
#include <vector>

class Parser {
public:
	Parser(const std::vector<Token>& t) : tokens(t), current(0), inFunction(false) {}

	BlockStmt* parseProgram();

	Expr* parseExpression();
	Stmt* parseStatement();
private:
	Expr* parseLogicalOr();
	Expr* parseLogicalAnd();
	Expr* parseEquality();
	Expr* parseRelation();
	Expr* parseTerm();
	Expr* parseFactor();
	Expr* parsePrimary();
	Expr* parseUnary();
	Expr* parsePostFix();

	Expr* parseBrackets(Expr* expr);
	Expr* parseParen(Expr* expr);

	BlockStmt* parseBlock(TokenType end=TokenType::UNKNOWN);
	Stmt* parseLET();
	Stmt* parsePRINT();
	Stmt* parseREAD();
	Stmt* parseIF();
	Stmt* parseWHILE();
	Stmt* parseRETURN();
	Stmt* parseFUNC();
	Stmt* parseAssign();
	Stmt* parseMax();


	BinaryOp mapBinary(TokenType t) const;
	UnaryOp mapUnary(TokenType t) const;

	bool check(TokenType t) const;
	bool match(TokenType t);
	const Token& consume(TokenType t, const std::string& msg);
	void skipNewLine();
	bool isAtEnd() const;
	const Token& peek() const;
	const Token& peekNext() const;
	const Token& advance();
private:
	const std::vector<Token>& tokens;
	int current;
	bool inFunction;
};