//
// Created by Reed on 4/2/2024.
//

#ifndef REMOVE_COMMENTS_SYMBOLTABLE_H
#define REMOVE_COMMENTS_SYMBOLTABLE_H
#include <vector>
#include <string>

struct Symbol {
    std::string identifier_name;
    std::string identifier_type;
    std::string datatype;
    bool datatype_is_array;
    int datatype_array_size;
    int scope;
    std::vector<Symbol> parameters; // Store parameters if symbol is function or procedure
};

class SymbolTable {
public:
    void insert(Symbol symbol, int linefound) {
        int foundScope = find(symbol.identifier_name, symbol.scope);
        if (foundScope != -1) {
            std::string scopeDescription = (foundScope == 0) ? "globally" : "locally";
            throw std::runtime_error("Error on line " + std::to_string(linefound) + ": variable \"" + symbol.identifier_name + "\" is already defined " + scopeDescription);
        }
        symbols.push_back(symbol);
    }

    int find(const std::string& name, int scope) {
        for (const auto& symbol : symbols) {
            if (symbol.identifier_name == name && (symbol.scope == scope || symbol.scope == 0)) {
                return symbol.scope;
            }
            if ((symbol.identifier_type == "function" || symbol.identifier_type == "procedure") && symbol.scope == scope) {
                for (const auto& parameter : symbol.parameters) {
                    if (parameter.identifier_name == name) {
                        return symbol.scope;
                    }
                }
            }
        }
        return -1; // Return -1 if the symbol was not found
    }



    void print() {
        for (const auto& symbol : symbols) {
            std::cout << "IDENTIFIER_NAME: " << symbol.identifier_name << "\n";
            std::cout << "IDENTIFIER_TYPE: " << symbol.identifier_type << "\n";
            if (symbol.identifier_type == "datatype" || symbol.identifier_type == "parameter") {
                std::cout << "DATATYPE: " << symbol.datatype << "\n";
                std::cout << "DATATYPE_IS_ARRAY: " << (symbol.datatype_is_array ? "yes" : "no") << "\n";
                if (symbol.datatype_is_array) {
                    std::cout << "DATATYPE_ARRAY_SIZE: " << symbol.datatype_array_size << "\n";
                } else {
                    std::cout << "DATATYPE_ARRAY_SIZE: 0\n";
                }
            } else if (symbol.identifier_type == "function" || symbol.identifier_type == "procedure") {
                std::cout << "DATATYPE: " << symbol.datatype << "\n";
                std::cout << "DATATYPE_IS_ARRAY: no\n";
                std::cout << "DATATYPE_ARRAY_SIZE: 0\n";
            } else {
                std::cout << "DATATYPE: NOT APPLICABLE\n";
                std::cout << "DATATYPE_IS_ARRAY: no\n";
                std::cout << "DATATYPE_ARRAY_SIZE: 0\n";
            }
            std::cout << "SCOPE: " << symbol.scope << "\n";

            // If the symbol is a function or procedure and it has parameters, print its parameters
            if ((symbol.identifier_type == "function" || symbol.identifier_type == "procedure") && !symbol.parameters.empty()) {
                std::cout << "\nPARAMETER LIST FOR: " << symbol.identifier_name << "\n";
                for (const auto& parameter : symbol.parameters) {
                    std::cout << "IDENTIFIER_NAME: " << parameter.identifier_name << "\n";
                    std::cout << "DATATYPE: " << parameter.datatype << "\n";
                    std::cout << "DATATYPE_IS_ARRAY: " << (parameter.datatype_is_array ? "yes" : "no") << "\n";
                    if (parameter.datatype_is_array) {
                        std::cout << "DATATYPE_ARRAY_SIZE: " << parameter.datatype_array_size << "\n";
                    } else {
                        std::cout << "DATATYPE_ARRAY_SIZE: 0\n";
                    }
                    std::cout << "SCOPE: " << parameter.scope << "\n";
                }
            }

            std::cout << "------------------------\n";
        }
    }



private:
    std::vector<Symbol> symbols;
};


#endif //REMOVE_COMMENTS_SYMBOLTABLE_H
