#pragma once
#include <string>
#include <stdexcept>
#include <vector>

class Table;

struct Expr {
	virtual ~Expr() = default;
	virtual Expr* clone() const = 0;
	virtual double eval(Table& table) const = 0;

	Expr() = default;
};

struct NumberExpr : public Expr {
	double value;
	explicit NumberExpr(double v) : value(v) {}
	virtual double eval(Table& table) const override;

	virtual Expr* clone() const override {
		return new NumberExpr(*this);
	}
};

struct VariableExpr : public Expr {
	std::string name;
	explicit VariableExpr(const std::string& n) : name(n) {}
	virtual double eval(Table& table) const override;

	virtual Expr* clone() const override {
		return new VariableExpr(*this);
	}
};

enum class BinaryOp {
	Plus, Minus, Mul, Div, Mod,
	Less, LessEqual, Greater, GreaterEqual,
	Equal, NotEqual,
	And, Or
};

enum class UnaryOp {
	Negate,
	Not
};

bool isValidBinaryOp(BinaryOp t);

bool isValidUnaryOp(UnaryOp t);


struct BinaryExpr : public Expr {
	BinaryOp op;
	Expr* left;
	Expr* right;
	explicit BinaryExpr(Expr* first, BinaryOp op, Expr* second);

	virtual double eval(Table& table) const override;

	virtual Expr* clone() const override;

	virtual ~BinaryExpr() override{
		delete left;
		delete right;
	}

	BinaryExpr(const BinaryExpr&) = delete;
	BinaryExpr& operator=(const BinaryExpr&) = delete;
};

struct UnaryExpr : public Expr {
	UnaryOp op;
	Expr* expr;
	explicit UnaryExpr(UnaryOp o, Expr* e);

	virtual double eval(Table& table) const override;

	virtual ~UnaryExpr() override {
		delete expr;
	}

	virtual Expr* clone() const override;
	UnaryExpr(const UnaryExpr&) = delete;
	UnaryExpr& operator=(const UnaryExpr&) = delete;
};

struct FunctionExpr : public Expr {
	std::string name;
	std::vector<Expr*> args;

	explicit FunctionExpr(const std::string& n, const std::vector<Expr*>& a) :name(n), args(a) {
		for (Expr* e : args) {
			if (!e)
				throw std::runtime_error("FunctionExpr: null argument");
		}
	}

	virtual ~FunctionExpr() override {
		for (Expr* e : args) delete e;
	}

	virtual double eval(Table& table) const override;

	virtual Expr* clone() const override;

	FunctionExpr(const FunctionExpr&) = delete;
	FunctionExpr& operator=(const FunctionExpr&) = delete;
};

struct IndexExpr :public Expr {
	std::string name;
	Expr* index;

	explicit IndexExpr(const std::string& n, Expr* i) : name(n), index(i) {
		if (!index) throw std::runtime_error("IndexExpr: null index");
	}

	virtual ~IndexExpr() override { delete index; }

	virtual double eval(Table& table) const override;

	virtual Expr* clone() const override;

	IndexExpr(const IndexExpr&) = delete;
	IndexExpr& operator=(const IndexExpr&) = delete;
};


struct MAXExpr : public Expr{
	Expr* left;
	Expr* right;

	explicit MAXExpr(Expr* left, Expr* right) : left(left), right(right) {
	}

	virtual ~MAXExpr() override{
		delete left;
		delete right;
	}

	virtual double eval(Table& table) const override;
	virtual Expr* clone() const override;

	MAXExpr(const MAXExpr&) = delete;
	MAXExpr& operator=(const MAXExpr&) = delete;

};