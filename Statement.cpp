#include "Statement.hpp"
#include "Table.hpp"

void LetStmt::exec(Table& table) const
{
	if (isArray) {
		table.declareArray(name, size);
		return;
	}

	table.declareVar(name);
}

Stmt* LetStmt::clone() const
{
	return new LetStmt(name, isArray, size);
}

void PrintStmt::exec(Table& table) const
{
	std::cout << expr->eval(table) << '\n';
}

Stmt* PrintStmt::clone() const
{
	Expr* newExpr = expr->clone();
	try {
		return new PrintStmt(newExpr);
	}
	catch (...) {
		delete newExpr;
		throw;
	}
}

void ReadStmt::exec(Table& table) const
{
	double val;

	if (!(std::cin >> val)) {
		throw std::runtime_error("READ: Invalid number input");
	}

	if (!index)
		table.assign(name, val);
	else {
		double raw = index->eval(table);
		int i = static_cast<int>(raw);
		if (raw != static_cast<double>(i)) throw std::logic_error("READ: Index must be an integer");
		table.setArrayElement(name, i, val);
	}
}

Stmt* ReadStmt::clone() const
{
	Expr* newIndex = (index) ? index->clone() : nullptr;
	try {
		return new ReadStmt(name, newIndex);
	}
	catch (const std::bad_alloc& ie)
	{
		delete newIndex;
		throw ie;
	}
}

void AssignStmt::exec(Table& table) const
{
	double val = value->eval(table);

	if (!index) {
		table.assign(name, val);
		return;
	}
	else {
		double raw = index->eval(table);
		int i = static_cast<int>(raw);
		if (raw != static_cast<double>(i)) throw std::logic_error("ASSIGN: Index must be an integer");
		table.setArrayElement(name, i, val);
	}
}

Stmt* AssignStmt::clone() const
{
	Expr* newIndex = (index) ? index->clone() : nullptr;
	Expr* newValue = nullptr;
	try {
		newValue = value->clone();
		return new AssignStmt(name, newIndex, newValue);
	}
	catch (...) {
		delete newIndex;
		delete newValue;
		throw;
	}
}

void BlockStmt::exec(Table& table) const
{
	Table newTable(&table);

	for (Stmt* s : stmts) {
		if (!s) throw std::runtime_error("null stmt in BlockStmt");
		s->exec(newTable);
	}
}

Stmt* BlockStmt::clone() const
{
	std::vector<Stmt*> copy;
	try {
		for (Stmt* s : stmts)
			copy.push_back(s->clone());

		return new BlockStmt(copy);
	}
	catch (...) {
		for (Stmt* s : copy) delete s;
		throw;
	}
}

void IfStmt::exec(Table& table) const
{
	double cond = condition->eval(table);

	if (cond != 0) {
		thenBlock->exec(table);
	}
	else if (elseBlock) {
		elseBlock->exec(table);
	}
}

Stmt* IfStmt::clone() const
{
	Expr* newCondition = nullptr;
	BlockStmt* newThen = nullptr;
	BlockStmt* newElse = nullptr;

	try {
		newCondition = condition->clone();
		newThen = static_cast<BlockStmt*>(thenBlock->clone());
		newElse = (elseBlock) ? static_cast<BlockStmt*>(elseBlock->clone()) : nullptr;


		return new IfStmt(newCondition, newThen, newElse);
	}
	catch (...) {
		delete newCondition;
		delete newThen;
		delete newElse;

		throw;
	}
}

void WhileStmt::exec(Table& table) const
{
	while (condition->eval(table) != 0) {
		block->exec(table);
	}
}

Stmt* WhileStmt::clone() const
{
	Expr* newCondition = nullptr;
	BlockStmt* newBlock = nullptr;
	try {
		newCondition = condition->clone();
		newBlock = static_cast<BlockStmt*>(block->clone());

		return new WhileStmt(newCondition, newBlock);
	}
	catch (...)
	{
		delete newCondition;
		delete newBlock;

		throw;
	}
}

void ReturnStmt::exec(Table& table) const
{
	if (!value) {
		throw ReturnSignal{ 0.0 };
	}

	double v = value->eval(table);
	throw ReturnSignal{ v };
}

Stmt* ReturnStmt::clone() const
{
	Expr* newVal = value->clone();
	try {
		return new ReturnStmt(newVal);
	}
	catch (...)
	{
		delete newVal;
		throw;
	}
}

void FunctionStmt::exec(Table& table) const
{
	Function fn;
	fn.name = name;
	fn.params = parameters;
	fn.body = body;
	fn.scope = &table;
	table.declareFunction(name, fn);
}

Stmt* FunctionStmt::clone() const
{
	BlockStmt* newBody = static_cast<BlockStmt*>(body->clone());
	try {
		return new FunctionStmt(name, parameters, newBody);
	}
	catch (const std::bad_alloc& ie) {
		delete newBody;
		throw ie;
	}
}

void ExprStmt::exec(Table& table) const
{
	if (!expr) throw std::runtime_error("ExprStmt: null expr");
	expr->eval(table);
}

Stmt* ExprStmt::clone() const
{
	return new ExprStmt(expr->clone());
}

MAXStmt::MAXStmt(Expr* left, Expr* right)
{
	if (!left || !right) throw std::runtime_error("Arguments for MAX statement are required");

	result = new MAXExpr(left, right);
}

void MAXStmt::exec(Table& table) const
{
	double res = result->eval(table);

	throw ReturnSignal{ res };
}

Stmt* MAXStmt::clone() const
{
	
}
