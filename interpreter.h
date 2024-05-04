//
// Created by Reed on 5/4/2024.
//
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <stack>

#include "SymbolTable.h"
#include "nodeStruc.h"

#ifndef ABSTRACT_SYNTAX_TREE_INTERPRETER_HPP
#define ABSTRACT_SYNTAX_TREE_INTERPRETER_HPP

class Interpreter {
public:
    Interpreter(Nodes* root) : root(root) {}

    void interpret() {
        executeStatement(root);
    }

private:
    Nodes* root;
    SymbolTable symbolTable;
    std::stack<int> callStack;
    int programCounter = 0;

    void executeStatement(Nodes* node) {
        if (node->data.type == TokenType::IF) {
            executeIfStatement(node);
        } else if (node->data.type == TokenType::FOR) {
            executeForStatement(node);
        } else if (node->data.type == TokenType::WHILE) {
            executeWhileStatement(node);
        } else if (node->data.type == TokenType::PrintF) {
            executePrintStatement(node);
        } else if (node->data.type == TokenType::RETURN) {
            executeReturnStatement(node);
        } else if (node->data.type == TokenType::ASSIGNMENT) {
            executeAssignmentStatement(node);
        } else if (node->data.type == TokenType::IDENTIFIER) {
            executeIdentifier(node);
        } else if (node->data.type == TokenType::INTEGER) {
            executeInteger(node);
        } else if (node->data.type == TokenType::STRING) {
            executeString(node);
        } else if (node->data.type == TokenType::DATATYPE_SPECIFIER) {
            executeDeclarationStatement(node);
        } else {
            // handle other types of statements
        }
    }

    void executeIfStatement(Nodes* node) {
        // Evaluate the condition
        bool condition = evaluateExpression(node->leftChild);

        // if the condition is true, execute the then branch
        if (condition) {
            executeStatement(node->rightSibling->leftChild);
        }
            // Otherwise, if theres an else branch, execute it
        else if (node->rightSibling->rightSibling != nullptr) {
            executeStatement(node->rightSibling->rightSibling->leftChild);
        }
    }

    void executeForStatement(Nodes* node) {
        // The initialization, condition, and increment parts of the for loop
        Nodes* initialization = node->leftChild;
        Nodes* condition = initialization->rightSibling;
        Nodes* increment = condition->rightSibling;

        // Execute the initialization statement
        executeStatement(initialization);

        // While the condition is true, execute the body and the increment statement
        while (evaluateExpression(condition)) {
            executeStatement(node->rightSibling);
            executeStatement(increment);
        }
    }


    void executeWhileStatement(Nodes* node) {
        // While the condition is true, execute the body
        while (evaluateExpression(node->leftChild)) {
            executeStatement(node->rightSibling);
        }
    }

    void executePrintStatement(Nodes* node) {
        // Evaluate the arguments
        Nodes* arg = node->leftChild;
        while (arg != nullptr) {
            std::cout << evaluateExpression(arg) << " ";
            arg = arg->rightSibling;
        }
        std::cout << std::endl;
    }

    int executeReturnStatement(Nodes* node) {
        return evaluateExpression(node->leftChild);
    }

    void executeAssignmentStatement(Nodes* node) {
        int value = evaluateExpression(node->rightSibling);
        symbolTable.set(node->leftChild->data.value, value);
    }

    int executeIdentifier(Nodes* node) {
        return symbolTable.get(node->data.value);
    }

    int executeInteger(Nodes* node) {
        return std::stoi(node->data.value);
    }

    int executeString(Nodes* node) {
        // Handle string values as needed
    }

    void executeDeclarationStatement(Nodes* node) {
        symbolTable.set(node->leftChild->data.value, 0);  // Initialize the variable to 0
    }


};

#endif //ABSTRACT_SYNTAX_TREE_INTERPRETER_HPP
