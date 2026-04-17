#pragma once
#include <vector>
#include <string>
#include <exception>
#include <stdexcept>
#include "Expression.hpp"
#include <iostream>

struct Stmt {
	virtual ~Stmt() = default;
	virtual Stmt* clone() const = 0;

	virtual void exec(Table& table) const = 0;

	explicit Stmt() = default;
	Stmt(const Stmt&) = delete;
	Stmt& operator=(const Stmt&) = delete;
};

struct BlockStmt : public Stmt {
	std::vector<Stmt*> stmts;

	explicit BlockStmt() = default;
	explicit BlockStmt(const std::vector<Stmt*>& s) : stmts(s) {}
	virtual void exec(Table& table) const override;
	virtual ~BlockStmt() override {
		for (Stmt* s : stmts) {
			delete s;
		}
	}

	virtual Stmt* clone() const override;

	BlockStmt(const BlockStmt&) = delete;
	BlockStmt& operator=(const BlockStmt&) = delete;
};

struct LetStmt: public Stmt {
	std::string name;
	bool isArray;
	size_t size;


	explicit LetStmt(const std::string& name, bool isArray = false, size_t size = 0) :
		name(name), isArray(isArray), size(size) {}

	virtual ~LetStmt() override = default;

	virtual void exec(Table& table) const override;

	virtual Stmt* clone() const override;

	LetStmt(const LetStmt&) = delete;
	LetStmt& operator=(const LetStmt&) = delete;

};

struct PrintStmt : public Stmt {
	Expr* expr;

	explicit PrintStmt(Expr* expr): expr(expr) {
		if (!expr) throw std::runtime_error("PrintStmt: Invalid nullptr!");
	}

	virtual ~PrintStmt() override {
		delete expr;
	}

	virtual void exec(Table& table) const override;

	virtual Stmt* clone() const override;

	PrintStmt(const PrintStmt&) = delete;
	PrintStmt& operator=(const PrintStmt&) = delete;
};

struct ReadStmt : public Stmt {
	std::string name;
	Expr* index;

	explicit ReadStmt(const std::string& name, Expr* index = nullptr) :name(name), index(index) {}

	virtual ~ReadStmt() override {
		delete index;
	}

	virtual void exec(Table& table) const override;

	virtual Stmt* clone() const override;

	ReadStmt(const ReadStmt&) = delete;
	ReadStmt& operator=(const ReadStmt&) = delete;
};

struct AssignStmt : public Stmt {
	std::string name;
	Expr* index;
	Expr* value;

	explicit AssignStmt(const std::string& name, Expr* index, Expr* value) : name(name), index(index), value(value) {
		if (!value) throw std::runtime_error("AssignStmt: invalid null value");
	}

	virtual ~AssignStmt() override {
		delete index;
		delete value;
	}

	virtual void exec(Table& table) const override;

	virtual Stmt* clone() const override;

	AssignStmt(const AssignStmt&) = delete;
	AssignStmt& operator=(const AssignStmt&) = delete;
};

struct IfStmt : public Stmt {
	Expr* condition;
	BlockStmt* thenBlock;
	BlockStmt* elseBlock;

	explicit IfStmt(Expr* condition, BlockStmt* thenBlock, BlockStmt* elseBlock = nullptr) :condition(condition), thenBlock(thenBlock), elseBlock(elseBlock) {
		if (!condition || !thenBlock) throw std::runtime_error("IfStmt: null condition/block");
	}

	virtual ~IfStmt() override {
		delete condition;
		delete thenBlock;
		delete elseBlock;
	}

	virtual void exec(Table& table) const override;

	virtual Stmt* clone() const override;

	IfStmt(const IfStmt&) = delete;
	IfStmt& operator=(const IfStmt&) = delete;
};

struct WhileStmt : public Stmt {
	Expr* condition;
	BlockStmt* block;

	explicit WhileStmt(Expr* condition, BlockStmt* block) :condition(condition), block(block) {
		if (!condition || !block) throw std::runtime_error("WhileStmt: null condition/block");
	}

	virtual ~WhileStmt() override{
		delete condition;
		delete block;
	}

	virtual void exec(Table& table) const override;

	virtual Stmt* clone() const override;

	WhileStmt(const WhileStmt&) = delete;
	WhileStmt& operator=(const WhileStmt&) = delete;
};

struct Parameter {
	std::string name;
	bool isArray = false;

	Parameter(const std::string& name, bool isArray = false):name(name), isArray(isArray) {}
};

struct FunctionStmt : public Stmt {
	std::string name;
	std::vector<Parameter> parameters;
	BlockStmt* body;

	explicit FunctionStmt(const std::string& n, const std::vector<Parameter>& p, BlockStmt* b) :name(n), parameters(p), body(b)
	{
		if (!body) throw std::runtime_error("FunctionStmt: null body");
	}

	virtual ~FunctionStmt() override { delete body; }

	virtual void exec(Table& table) const override;

	virtual Stmt* clone() const override;

	FunctionStmt(const FunctionStmt&) = delete;
	FunctionStmt& operator=(const FunctionStmt&) = delete;
};


struct ReturnSignal {
	double value;
};

struct ReturnStmt : public Stmt {
	Expr* value;

	explicit ReturnStmt(Expr* v) : value(v) {
		if (!value) throw std::runtime_error("ReturnStmt: null value");
	}

	~ReturnStmt() override { delete value; }

	virtual void exec(Table& table) const override;

	virtual Stmt* clone() const override;

	ReturnStmt(const ReturnStmt&) = delete;
	ReturnStmt& operator=(const ReturnStmt&) = delete;
};

struct ExprStmt : public Stmt {
	Expr* expr;

	explicit ExprStmt(Expr* e) : expr(e) {}

	void exec(Table& table) const override;

	Stmt* clone() const override;

	~ExprStmt() override {
		delete expr;
	}

	ExprStmt(const ExprStmt&) = delete;
	ExprStmt& operator=(const ExprStmt&) = delete;
};

struct MAXStmt: public Stmt {
	Expr* result;
	
	explicit MAXStmt(Expr* left, Expr* right);

	void exec(Table& table) const override;
	Stmt* clone()const override;

	~MAXStmt() override {
		delete result;
	}

	MAXStmt(const MAXStmt&) = delete;
	MAXStmt& operator=(const MAXStmt&) = delete;
};