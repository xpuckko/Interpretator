#include "Expression.hpp"
#include "Table.hpp"

bool isValidBinaryOp(BinaryOp t) {
	switch (t) {
	case BinaryOp::Plus:
	case BinaryOp::Minus:
	case BinaryOp::Mul:
	case BinaryOp::Div:
	case BinaryOp::Mod:

	case BinaryOp::Equal:
	case BinaryOp::NotEqual:
	case BinaryOp::Less:
	case BinaryOp::LessEqual:
	case BinaryOp::Greater:
	case BinaryOp::GreaterEqual:

	case BinaryOp::And:
	case BinaryOp::Or:
		return true;
	default:
		return false;
	}
}

bool isValidUnaryOp(UnaryOp t)
{
	switch (t) {
	case UnaryOp::Not:
	case UnaryOp::Negate:
		return true;
	default:
		return false;
	}
}

double NumberExpr::eval(Table& table) const
{
	return value;
}

double VariableExpr::eval(Table& table) const
{
	return table.get(name);
}

BinaryExpr::BinaryExpr(Expr* first, BinaryOp op, Expr* second)
{
	if (!first || !second) throw std::runtime_error("Invalid expressions in binary operator");

	if (!isValidBinaryOp(op)) throw std::runtime_error("Invalid binary operator");

	left = first;
	right = second;
	this->op = op;
}

double BinaryExpr::eval(Table& table) const
{
	double left = this->left->eval(table);
	double right = this->right->eval(table);

	switch (op) {
	case BinaryOp::Plus: return left + right;
	case BinaryOp::Minus: return left - right;
	case BinaryOp::Mul: return left * right;
	case BinaryOp::Div:
	{
		if (right == 0) throw std::runtime_error("Cannot devide by zero");

		return left / right;
	}
	case BinaryOp::Mod: 
	{
		if (right == 0) throw std::runtime_error("Cannot module by zero");

		return static_cast<int>(left) % static_cast<int>(right);
	}

	case BinaryOp::Less: return left < right;
	case BinaryOp::LessEqual: return left <= right;
	case BinaryOp::Greater: return left > right;
	case BinaryOp::GreaterEqual: return left >= right;
	case BinaryOp::Equal: return left == right;
	case BinaryOp::NotEqual: return left != right;
	case BinaryOp::Or: return (left != 0) || (right != 0);
	case BinaryOp::And: return (left != 0) && (right != 0);
	default:
		throw std::runtime_error("Unknown binary operator");

	}
}

Expr* BinaryExpr::clone() const
{
	Expr* newLeft = nullptr;
	Expr* newRight = nullptr;

	try {
		newLeft = left->clone();
		newRight = right->clone();

		return new BinaryExpr(newLeft, op, newRight);
	}
	catch (...) {
		delete newLeft;
		delete newRight;
		throw;
	}
}

UnaryExpr::UnaryExpr(UnaryOp o, Expr* e):op(o)
{
	if (!e) throw std::runtime_error("Invalid unary expression");

	if (!isValidUnaryOp(op)) throw std::runtime_error("Invalid unary operator");

	expr = e;
}

double UnaryExpr::eval(Table& table) const
{
	double expr = this->expr->eval(table);

	switch (op) {
	case UnaryOp::Negate: return -expr;
	case UnaryOp::Not: return expr==0;
	default:
		throw std::runtime_error("Unknown unary operator");
	}
}

Expr* UnaryExpr::clone() const
{
	Expr* newExpr = expr->clone();
	try {
		return new UnaryExpr(op, newExpr);
	}
	catch (...) {
		delete newExpr;
		throw;
	}
}

double IndexExpr::eval(Table& table) const
{
	double i = static_cast<int>(index->eval(table));

	return table.getArrayElement(name,static_cast<int>(i));
}

Expr* IndexExpr::clone() const
{
	Expr* newIdx = nullptr;
	try {
		newIdx = index->clone();
		return new IndexExpr(name, newIdx);
	}
	catch (...) {
		delete newIdx;
		throw;
	}
}

double FunctionExpr::eval(Table& table) const
{
	const Function& fn = table.getFunction(name);

	if (args.size() != fn.params.size())
		throw std::logic_error("FunctionExpr: wrong number of arguments for" + name);

	Table callBlock(fn.scope);

	size_t paramsSize = fn.params.size();

	for (size_t i = 0; i < paramsSize; i++)
	{
		const Parameter& p = fn.params[i];

		if (!p.isArray) {
			double v = args[i]->eval(table);
			callBlock.declareVar(p.name);
			callBlock.assign(p.name, v);
		}
		else {
			const VariableExpr* v = dynamic_cast<const VariableExpr*>(args[i]);
			if (!v) {
				throw std::logic_error("FunctionExpr: array argument bust be an array name for param" + p.name);
			}

			const std::string& arrName = v->name;

			std::vector<double>* ptr = table.getArrayPtr(arrName);
			callBlock.bindArray(p.name, ptr);
		}
	}

	try {
		fn.body->exec(callBlock);
	}
	catch (const ReturnSignal& r) {
		return r.value;
	}

	throw std::logic_error("Function ended without RETURN: " + name);
}

Expr* FunctionExpr::clone() const
{
	std::vector<Expr*> clonedArgs;

	try {
		for (Expr* e : args) {
			clonedArgs.push_back(e->clone());
		}

		return new FunctionExpr(name, clonedArgs);
	}
	catch (...) {
		for (Expr* e : clonedArgs) {
			delete e;
		}
		throw;
	}
}

double MAXExpr::eval(Table& table) const
{
	double l = left->eval(table);
	if (!right) return l;

	double r = right->eval(table);
	return std::max(l, r);
}

Expr* MAXExpr::clone() const
{
	Expr* left = nullptr;
	Expr* right = nullptr;

	try {
		left = this->left->clone();
		right = (this->right) ? this->right->clone() : nullptr;

		return new MAXExpr(left, right);
	}
	catch (...) {
		delete left;
		delete right;
		throw;
	}
}
