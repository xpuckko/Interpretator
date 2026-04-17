#include <iostream>
#include <fstream>
#include <sstream>

#include "Lexer.hpp"
#include "Parser.hpp"
#include "Statement.hpp"
#include "Table.hpp"
void runSource(const std::string& source)
{
    Lexer lexer;
    lexer.readFromString(source);

    Parser parser(lexer.getTokens());

    Table global(nullptr);
    Stmt* program = nullptr;

    try {
        program = parser.parseProgram();
        std::cout << "\nOUTPUT:\n";
        program->exec(global);
    }
    catch (const ReturnSignal& r) {
        std::cout << "\nProgram exited with code: " << r.value << "\n";
    }
    catch (...) {
        delete program;
        throw;
    }

    delete program;
}

void runFile(const std::string& filename)
{
    std::ifstream input(filename);
    if (!input.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    std::stringstream buffer;
    buffer << input.rdbuf();
    runSource(buffer.str());
}

int main()
{
    std::string mode;

    while (true) {
        std::cout << "\nChoose mode (example / write / exit): ";
        std::getline(std::cin, mode);

        if (mode == "exit") {
            break;
        }
        else if (mode == "example") {
            while (true) {
                std::cout << "Enter example file name: ";
                std::string filename;
                std::getline(std::cin, filename);

                try {
                    runFile(filename);
                    break;
                }
                catch (const std::exception& e) {
                    std::cout << "Error: " << e.what() << "\n";
                    std::cout << "Try again.\n";
                }
            }
        }
        else if (mode == "write") {
            std::cout << "Enter your code. Finish with empty line:\n\n";

            std::string source;
            std::string line;

            while (true) {
                std::getline(std::cin, line);
                if (line.empty()) break;
                source += line + "\n";
            }

            try {
                runSource(source);
            }
            catch (const std::exception& e) {
                std::cout << "Error: " << e.what() << "\n";
            }
        }
        else {
            std::cout << "Invalid option.\n";
        }
    }

    return 0;
}