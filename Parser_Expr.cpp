#include "Parser.hpp"

Expr* Parser::parseLogicalOr()
{
    Expr* left = parseLogicalAnd();

    while (check(TokenType::OR)) {
        BinaryOp op = mapBinary(peek().type);
        advance();

        Expr* right = parseLogicalAnd();
        left = new BinaryExpr(left, op, right);
    }

    return left;
}

Expr* Parser::parseLogicalAnd()
{
    Expr* left = parseEquality();

    while (check(TokenType::AND)) {
        BinaryOp op = mapBinary(peek().type);
        advance();

        Expr* right = parseEquality();
        left = new BinaryExpr(left, op, right);
    }

    return left;
}

Expr* Parser::parseEquality()
{
    Expr* left = parseRelation();

    while (check(TokenType::EQUAL) || check(TokenType::NOT_EQUAL)) {
        BinaryOp op = mapBinary(peek().type);
        advance();

        Expr* right = parseRelation();
        left = new BinaryExpr(left, op, right);
    }

    return left;
}

Expr* Parser::parseRelation()
{
    Expr* left = parseTerm();

    while (check(TokenType::GREATER)|| check(TokenType::GREATER_EQUAL) || check(TokenType::LESS) || check(TokenType::LESS_EQUAL)) {
        BinaryOp op = mapBinary(peek().type);
        advance();

        Expr* right = parseTerm();
        left = new BinaryExpr(left, op, right);
    }

    return left;
}

Expr* Parser::parseTerm()
{
    Expr* left = parseFactor();

    while (check(TokenType::PLUS) || check(TokenType::MINUS)) {
        BinaryOp op = mapBinary(peek().type);
        advance();

        Expr* right = parseFactor();
        left = new BinaryExpr(left, op, right);
    }
    return left;
}

Expr* Parser::parseFactor()
{
    Expr* left = parseUnary();

    while (check(TokenType::STAR) || check(TokenType::SLASH) || check(TokenType::MOD)) {
        BinaryOp op = mapBinary(peek().type);
        advance();

        Expr* right = parseUnary(); 
        left = new BinaryExpr(left, op, right);
    }

    return left;
}

Expr* Parser::parsePrimary()
{
    if (isAtEnd())
        throw std::runtime_error("Unexpected end of input\n");

    const Token& t = advance();

    if (t.type == TokenType::NUMBER) {
        return new NumberExpr(t.numberValue);
    }

    if (t.type == TokenType::IDENTIFIER) {

        if(check(TokenType::LEFT_BRACKET)) {
            advance();

            Expr* index = nullptr;
            try {
                index = parseExpression();

                if (isAtEnd() || peek().type != TokenType::RIGHT_BRACKET)
                    throw std::runtime_error("']' expected\n");

                advance();
                return new IndexExpr(t.word, index);
            }
            catch (...) {
                delete index;
                throw;
            }
        }
        return new VariableExpr(t.word);
    }

    if (t.type == TokenType::LEFT_PAREN) {
        Expr* expr = parseExpression();

        if (isAtEnd() || peek().type != TokenType::RIGHT_PAREN)
            throw std::runtime_error("')' expected\n");

        advance();
        return expr;
    }

    if (t.type == TokenType::QUESTION) {

    }

    throw std::runtime_error("Unexpected expression\n");
}

Expr* Parser::parseUnary()
{
    if (check(TokenType::NOT) || check(TokenType::MINUS)) {
        UnaryOp op = mapUnary(peek().type);
        advance();

        Expr* right = parseUnary();
        return new UnaryExpr(op, right);
    }

    return parsePostFix();
}

Expr* Parser::parsePostFix()
{
    Expr* expr = parsePrimary();

    while (!isAtEnd()) {
        try {
            if (peek().type == TokenType::LEFT_PAREN) {
                expr = parseParen(expr);
                continue;
            }

            if (peek().type == TokenType::LEFT_BRACKET) {
                expr = parseBrackets(expr);
                continue;
            }
            break;
        }
        catch (...) {
            delete expr;
            throw;
        }
    }

    return expr;
}


Expr* Parser::parseBrackets(Expr* expr)
{
    advance();

    Expr* index = nullptr;
    try {
        index = parseExpression();

        if (isAtEnd() || peek().type != TokenType::RIGHT_BRACKET)
            throw std::runtime_error("']' expected\n");

        advance();
    }
    catch (...) {
        delete index;
        throw;
    }

    VariableExpr* v = dynamic_cast<VariableExpr*>(expr);
    if (!v) {
        delete index;
        throw std::runtime_error("Indexing must be on identifier\n");
    }

    std::string arr = v->name;
    delete v;
    try {
        return new IndexExpr(arr, index);
    }
    catch (...) {
        delete index;
        throw;
    }
}

Expr* Parser::parseParen(Expr* expr)
{
    advance();

    std::vector<Expr*> arguments;

    try {
        if (!isAtEnd() && peek().type != TokenType::RIGHT_PAREN) {
            arguments.push_back(parseExpression());

            while (check(TokenType::COMMA)) {
                advance();
                arguments.push_back(parseExpression());
            }
        }

        if (isAtEnd() || peek().type != TokenType::RIGHT_PAREN) {
            throw std::runtime_error("')' expected after function arguments");
        }
        advance();

    }
    catch (...) {
        for (Expr* a : arguments) delete a;
        throw;
    }

    VariableExpr* v = dynamic_cast<VariableExpr*>(expr);
    if (!v) {
        for (Expr* a : arguments) delete a;
        throw std::runtime_error("Function call must be on identifier");
    }

    std::string funcName = v->name;
    delete v;
    try {
        return new FunctionExpr(funcName, arguments);
    }
    catch (...) {
        for (Expr* a : arguments) delete a;
        throw;
    }
}