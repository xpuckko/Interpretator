#pragma once
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <vector>
#include "Expression.hpp"
#include "Statement.hpp"

struct Function {
	std::string name;
	std::vector<Parameter> params;
	BlockStmt* body;
	Table* scope;
};

struct Array {
	std::vector<double>* data;
	bool owned;
};

class Table {

public:
	explicit Table(Table* parent = nullptr) : parent(parent) {}
	~Table();


	double get(const std::string& name) const;
	void assign(const std::string& name, double val);
	const Function& getFunction(const std::string& name) const;

	void declareVar(const std::string& name);
	void declareArray(const std::string& name, size_t size);
	void declareFunction(const std::string& name, const Function& fn);

	double getArrayElement(const std::string& name, int index) const;
	void setArrayElement(const std::string& name, size_t index, double value);
	void bindArray(const std::string& localName, std::vector<double>* arr);
	std::vector<double>* getArrayPtr(const std::string& name);

private:
	bool isVarDeclared(const std::string& name) const;
private:
	Table* parent;
	std::unordered_map<std::string, Function> functions;
	std::unordered_map<std::string, double> vars;
	std::unordered_map<std::string, Array> arrays;
};