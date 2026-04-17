#include "Parser.hpp"
#include "Expression.hpp"

BlockStmt* Parser::parseProgram()
{
    std::vector<Stmt*> stmts;

    try {
        while (!isAtEnd()) {
            if (match(TokenType::NEWLINE)) continue;

            stmts.push_back(parseStatement());
        }

        return new BlockStmt(stmts);

    }
    catch (...) {
        for (Stmt* s : stmts) delete s;
        throw;
    }
}

Expr* Parser::parseExpression()
{
    Expr* e = parseLogicalOr();
    return e;
}

Stmt* Parser::parseStatement() {
 
    skipNewLine();


    if (match(TokenType::LET))       return parseLET(); 
    if (match(TokenType::PRINT))     return parsePRINT();
    if (match(TokenType::READ))      return parseREAD();
    if (match(TokenType::IF))        return parseIF();
    if (match(TokenType::WHILE))     return parseWHILE();
    if (match(TokenType::FUNCTION)) { if (inFunction) throw std::logic_error("Parser: nested function");  return parseFUNC(); }
    if (match(TokenType::RETURN))    return parseRETURN();
    if (match(TokenType::MAX))       return parseMax();

    if (check(TokenType::IDENTIFIER) && (peekNext().type == TokenType::ASSIGN || peekNext().type == TokenType::LEFT_BRACKET))
        return parseAssign();

    if (check(TokenType::IDENTIFIER) && peekNext().type == TokenType::LEFT_PAREN) {
        Expr* call = parseExpression();
        skipNewLine();
        try {
            return new ExprStmt(call);
        }
        catch (...) {
            delete call;
            throw;
        }
    }

    throw std::runtime_error("Unexpected token in statement");
}



BinaryOp Parser::mapBinary(TokenType t) const
{
    switch (t) {
    case TokenType::PLUS:           return BinaryOp::Plus;
    case TokenType::MINUS:          return BinaryOp::Minus;
    case TokenType::STAR:           return BinaryOp::Mul;
    case TokenType::SLASH:          return BinaryOp::Div;
    case TokenType::MOD:            return BinaryOp::Mod;

    case TokenType::LESS:           return BinaryOp::Less;
    case TokenType::LESS_EQUAL:     return BinaryOp::LessEqual;
    case TokenType::GREATER:        return BinaryOp::Greater;
    case TokenType::GREATER_EQUAL:  return BinaryOp::GreaterEqual;

    case TokenType::EQUAL:          return BinaryOp::Equal;
    case TokenType::NOT_EQUAL:      return BinaryOp::NotEqual;

    case TokenType::AND:            return BinaryOp::And;
    case TokenType::OR:             return BinaryOp::Or;

    default:
        throw std::runtime_error("Invalid binary operator token");
    }
}

UnaryOp Parser::mapUnary(TokenType t) const
{
    switch (t) {
    case TokenType::MINUS: return UnaryOp::Negate;
    case TokenType::NOT:   return UnaryOp::Not;

    default:
        throw std::runtime_error("Invalid unary operator token");
    }
}


bool Parser::check(TokenType t) const {
    if (isAtEnd()) return false;
    return peek().type == t;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

const Token& Parser::consume(TokenType t, const std::string& msg) {
    if (check(t)) return advance();
    throw std::runtime_error(msg);
}

void Parser::skipNewLine() {
    while (!isAtEnd() && check(TokenType::NEWLINE)) advance();
}

bool Parser::isAtEnd() const {
    return current >= tokens.size();
}

const Token& Parser::peek() const {
    return tokens[current];
}

const Token& Parser::peekNext() const {
    if (current + 1 < tokens.size()) return tokens[current + 1];
    throw std::overflow_error("There is no next element");
}

const Token& Parser::advance() {
    if (!isAtEnd()) current++;
    return tokens[current - 1];
}

