#include "Table.hpp"


Table::~Table()
{
    std::unordered_map<std::string, Array>::iterator it =
        arrays.begin();
  
    while (it != arrays.end()) {
        if (it->second.owned) {
            delete it->second.data;
            it->second.data = nullptr;
        }
        ++it;
    }
}

double Table::get(const std::string& name) const
{
    std::unordered_map<std::string, double>::const_iterator it = vars.find(name);

    if (it != vars.end()) {
        return it->second;
    }

    if (parent) {
        return parent->get(name);
    }

    throw std::runtime_error("There isn't a variable with that name");
}

void Table::assign(const std::string& name, double val)
{
    std::unordered_map<std::string,double>::iterator it = vars.find(name);
    if (it != vars.end()) {
        it->second = val;
        return;
    }

    if (parent) {
        parent->assign(name, val);
        return;
    }

    throw std::logic_error("ASSIGN: variable not declared: " + name);
}

const Function& Table::getFunction(const std::string& name) const
{
    std::unordered_map<std::string, Function>::const_iterator it = functions.find(name);

    if (it != functions.end()) {
        return it->second;
    }

    if (parent != nullptr) {
        return parent->getFunction(name);
    }

    throw std::logic_error("Undefined function: " + name);
}

void Table::declareVar(const std::string& name)
{
    if (vars.find(name) != vars.end() || arrays.find(name)!=arrays.end())
        throw std::logic_error("LET: name already declared in this scope");

    vars.insert(std::pair<std::string,double>(name,0.0));
}


void Table::declareArray(const std::string& name, size_t size)
{
    if (size <= 0) throw std::logic_error("Invalid array size" + std::to_string(size));

    if (vars.find(name) != vars.end() || arrays.find(name) != arrays.end()) {
        throw std::runtime_error("LET: Name already defined " + name);
    }

    arrays.insert(std::pair<std::string,Array>(name,Array{new std::vector<double>(size,0.0), true }));
}

void Table::declareFunction(const std::string& name, const Function& fn)
{
    std::unordered_map<std::string, Function>::iterator it = functions.find(name);

    if (it != functions.end()) {
        throw std::logic_error("FUNCTION already declared in this scope: " + name);
    }

    functions.insert(std::pair<std::string, Function>(name, fn));
}

double Table::getArrayElement(const std::string& name, int index) const
{
    std::unordered_map<std::string, Array>::const_iterator it = arrays.find(name);

    if (it != arrays.end()) {
        const std::vector<double>& v = *(it->second.data);
        if (index >= v.size() || index < 0) 
            throw std::out_of_range("Index out of range" + name);

        return v[static_cast<size_t>(index)];
    }

    if (parent)
        return parent->getArrayElement(name, index);

    throw std::runtime_error("Undefined array: " + name);
}

void Table::setArrayElement(const std::string& name, size_t index, double value)
{
    std::unordered_map<std::string, Array>::iterator it = arrays.find(name);

    if (it != arrays.end()) {
        std::vector<double>& v = *(it->second.data);
        if (index >= v.size() || index < 0) 
            throw std::out_of_range("Index out of range" + name);
        
        
        v[index] = value;
        return;
    }

    if (parent) {
        parent->setArrayElement(name, index, value);
        return;
    }

    throw std::runtime_error("Undefined array: " + name);
}

void Table::bindArray(const std::string& localName, std::vector<double>* arr)
{
    if (!arr) throw std::logic_error("Null array alies");

    if (vars.find(localName) != vars.end() || arrays.find(localName) != arrays.end()) {
        throw std::runtime_error("LET: Name already defined " + localName);
    }

    arrays[localName] = Array{ arr, false };
}

std::vector<double>* Table::getArrayPtr(const std::string& name)
{
    std::unordered_map<std::string, Array>::const_iterator it = arrays.find(name);
    if (it != arrays.end()) return it->second.data;

    if (parent) return parent->getArrayPtr(name);

    throw std::logic_error("Undefined array: " + name);
}

bool Table::isVarDeclared(const std::string& name) const
{
    if (vars.find(name) != vars.end() || arrays.find(name) != arrays.end()) {
        return true;
    }

    if (parent) {
        return parent->isVarDeclared(name);
    }

    return false;
}
