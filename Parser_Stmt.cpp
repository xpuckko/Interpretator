#include "Parser.hpp"


BlockStmt* Parser::parseBlock(TokenType end)
{
    std::vector<Stmt*> stmts;

    try {
        while (!isAtEnd()) {
            skipNewLine();
            if (isAtEnd()) break;
            if (end != TokenType::UNKNOWN && check(end))
                break;

            Stmt* s = parseStatement();
            if (!s) throw std::runtime_error("Internal parser error: null statement");
            stmts.push_back(s);
            skipNewLine();
        }

        if (end != TokenType::UNKNOWN) {
            consume(end, "Expected block terminator");
            skipNewLine();
        }

        return new BlockStmt(stmts);
    }
    catch (...) {
        for (Stmt* s : stmts) delete s;
        throw;
    }
}


Stmt* Parser::parseLET()
{
    Token token = consume(TokenType::IDENTIFIER, "Expected identifier after let");
    std::string name = token.word;

    bool isArray = false;
    int size = 0;

    if (match(TokenType::LEFT_BRACKET)) {
        isArray = true;
        Token n = consume(TokenType::NUMBER, "Expected array size");
        size = n.numberValue;
        consume(TokenType::RIGHT_BRACKET, "Expected ']'");
    }

        consume(TokenType::NEWLINE, "Expected newline after statement");
        return new LetStmt(name, isArray, size);
}

Stmt* Parser::parsePRINT()
{
    Expr* e = nullptr;
    try {
        e = parseExpression();
        consume(TokenType::NEWLINE, "Expected newline after print");
        return new PrintStmt(e);
    }
    catch (...) {
        delete e;
        throw;
    }
}

Stmt* Parser::parseREAD()
{
    Token nameTok = consume(TokenType::IDENTIFIER, "Expected identifier after read");
    std::string name = nameTok.word;

    Expr* index = nullptr;
    try {
        if (match(TokenType::LEFT_BRACKET)) {
            index = parseExpression();
            consume(TokenType::RIGHT_BRACKET, "Expected ']'");
        }

        consume(TokenType::NEWLINE, "Expected end of line after read");
        while (match(TokenType::NEWLINE)) {}

        return new ReadStmt(name, index);
    }
    catch (...) {
        delete index;
        throw;
    }
}

Stmt* Parser::parseIF()
{
    consume(TokenType::LEFT_PAREN, "Expected '(' after if");

    Expr* condition = nullptr;
    BlockStmt* then = nullptr;
    BlockStmt* elseB = nullptr;
    std::vector<Stmt*> thenStmts;
    std::vector<Stmt*> elseStmts;

    try {
        condition = parseExpression();
        consume(TokenType::RIGHT_PAREN, "Expected '(' after condition");

        consume(TokenType::NEWLINE, "Expected newline after if condition");
        skipNewLine();

        while (!isAtEnd()) {
            skipNewLine();

            if (check(TokenType::ELSE) || check(TokenType::ENDIF))
                break;

            thenStmts.push_back(parseStatement());
        }
        then = new BlockStmt(thenStmts);

        if (match(TokenType::ELSE)) {
            consume(TokenType::NEWLINE, "Expected newline after else");
            skipNewLine();

            while (!isAtEnd()) {
                skipNewLine();

                if (check(TokenType::ENDIF))
                    break;

                elseStmts.push_back(parseStatement());
            }
            elseB = new BlockStmt(elseStmts);
        }

        consume(TokenType::ENDIF, "Expected 'endif'");
        skipNewLine();

        return new IfStmt(condition, then, elseB);
    }
    catch (...) {
        delete condition;
        delete then;
        delete elseB;

        throw;
    }
}

Stmt* Parser::parseWHILE()
{
    consume(TokenType::LEFT_PAREN, "Expected '(' after while");
    Expr* condition = nullptr;
    BlockStmt* block = nullptr;

    try {
        condition = parseExpression();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after while");

        consume(TokenType::NEWLINE, "Expected NEWLINE after while condition");
        skipNewLine();

        block = parseBlock(TokenType::DONE);

        return new WhileStmt(condition, block);
    }
    catch (...) {
        delete condition;
        delete block;
        throw;
    }
}


Stmt* Parser::parseRETURN()
{
    Expr* val = nullptr;

    try {
        val = parseExpression();



        consume(TokenType::NEWLINE, "Expected newline after return");
        skipNewLine();

        return new ReturnStmt(val);
    }
    catch (...) {
        delete val;
        throw;
    }
}

Stmt* Parser::parseFUNC()
{
    inFunction = true;

    Token token = consume(TokenType::IDENTIFIER, "Expected function name");
    std::string name = token.word;

    consume(TokenType::LEFT_PAREN, "Expected '(' after function name");
    std::vector<Parameter> parameters;

    if (!check(TokenType::RIGHT_PAREN)) {
        while (true) {
            Token param = consume(TokenType::IDENTIFIER, "Expected parameter name");

            bool isArray = false;
            if (match(TokenType::LEFT_BRACKET)) {
                consume(TokenType::RIGHT_BRACKET, "Expected '] in parameter");
                isArray = true;
            }

            parameters.push_back(Parameter(param.word, isArray));

            if (!match(TokenType::COMMA)) break;
        }
    }
    consume(TokenType::RIGHT_PAREN, "Expected ')' after parameters");
    consume(TokenType::NEWLINE, "Expecteed newline after function header");
    skipNewLine();

    BlockStmt* body = nullptr;
    try {
        body = parseBlock(TokenType::DONE);
        inFunction = false;
        return new FunctionStmt(name, parameters, body);
    }
    catch (...) {
        inFunction = false;
        delete body;
        throw;
    }
}

Stmt* Parser::parseAssign()
{
    Token tok = consume(TokenType::IDENTIFIER, "Expected identifier in assign");
    std::string name = tok.word;

    Expr* index = nullptr;
    Expr* val = nullptr;

    try {
        if (match(TokenType::LEFT_BRACKET)) {
            index = parseExpression();
            consume(TokenType::RIGHT_BRACKET, "Expected ']'");
        }

        consume(TokenType::ASSIGN, "Expected '=' in assignment");

        val = parseExpression();

        consume(TokenType::NEWLINE, "Expected newline after assignment");
        skipNewLine();

        return new AssignStmt(name, index, val);
    }
    catch (...) {
        delete index;
        delete val;
        throw;
    }
}

Stmt* Parser::parseMax()
{
    Token tok = consume(TokenType::MAX, "Expected MAX");
    consume(TokenType::LEFT_PAREN, "Expected '(' after MAX");
    
    Expr* left;
    Expr* right;

    try {
        left = parseExpression();
        if (check(TokenType::RIGHT_PAREN)) {
            return new MAXStmt(left, right);
        }
        
        consume(TokenType::COMMA, "Comma expected!");

        right = parseExpression();
        consume(TokenType::RIGHT_BRACKET, "Right Bracket expected");

        return new MAXStmt(left, right);
    }
    catch (...) {
        delete left;
        delete right;
    }
}
