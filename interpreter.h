//
// Created by Reed on 5/4/2024.
//
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <stack>

#include "nodeStruc.h"
#include "parser.h"
#include "structures.h"

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

    int evaluateExpression(Nodes* node) {
        std::stack<int> operandStack;

        // Traverse the expression in postfix order
        for (Nodes* current = node; current != nullptr; current = current->rightSibling) {
            if (current->data.type == TokenType::INTEGER) {
                // Push integer literals directly onto the stack
                operandStack.push(std::stoi(current->data.value));
            } else if (current->data.type == TokenType::IDENTIFIER) {
                // Look up the value of identifiers in the symbol table and push onto the stack
                operandStack.push(symbolTable.get(current->data.value));
            } else {
                // For operators, pop the necessary number of operands from the stack, perform the operation, and push the result back onto the stack
                int rightOperand = operandStack.top();
                operandStack.pop();
                int leftOperand = operandStack.top();
                operandStack.pop();

                int result;
                if (current->data.value == "+") {
                    result = leftOperand + rightOperand;
                } else if (current->data.value == "-") {
                    result = leftOperand - rightOperand;
                } else if (current->data.value == "*") {
                    result = leftOperand * rightOperand;
                } else if (current->data.value == "/") {
                    result = leftOperand / rightOperand;
                } else if (current->data.value == "%") {
                    result = leftOperand % rightOperand;
                } else if (current->data.value == "<") {
                    result = leftOperand < rightOperand;
                } else if (current->data.value == ">") {
                    result = leftOperand > rightOperand;
                } else if (current->data.value == "==") {
                    result = leftOperand == rightOperand;
                } else if (current->data.value == "&&") {
                    result = leftOperand && rightOperand;
                } else {
                    // Handle other operators as needed
                }

                operandStack.push(result);
            }
        }

        // The result of the expression is the final value left on the stack
        return operandStack.top();
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
