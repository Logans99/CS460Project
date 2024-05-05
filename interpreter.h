//
// Created by Reed on 5/4/2024.
//
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <stack>

#include "nodestruc.h"
#include "parser.h"
#include "Structures.h"

#ifndef ABSTRACT_SYNTAX_TREE_INTERPRETER_HPP
#define ABSTRACT_SYNTAX_TREE_INTERPRETER_HPP

class Interpreter {
public:
    Interpreter(Nodes* root, SymbolTable& symbolTable) : root(root), symbolTable(symbolTable) {}

    void interpret() {
        executeStatement(root);
    }

private:
    Nodes* root;
    SymbolTable symbolTable;
    std::stack<int> callStack;
    int programCounter = 0;

    int evaluateExpression(Nodes* node) {
        std::stack<int> operandStack;

        // Traverse the expression in postfix order
        for (Nodes* current = node; current != nullptr; current = current->rightSibling) {
            if (current->data.type == TokenType::INTEGER) {
                // Push integer literals directly onto the stack
                operandStack.push(std::stoi(current->data.value));
            } else if (current->data.type == TokenType::IDENTIFIER) {
                // Look up the value of identifiers in the symbol table and push onto the stack
                //operandStack.push(symbolTable.get(current->data.value));
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
            //executeDeclarationStatement(node);
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
        // evaluate the arguments
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
        symbolTable.update(node->leftChild->data.value, value);
    }

    int executeIdentifier(Nodes* node) {
        return symbolTable.get(node->data.value);
    }

    int executeInteger(Nodes* node) {
        return std::stoi(node->data.value);
    }

    std::string executeString(Nodes* node) {
        return node->data.value;
    }

    // declarations probably not necessary
//    void executeDeclarationStatement(Nodes* node) {
//        if (symbolTable.exists(node->leftChild->data.value)) {
//            throw std::runtime_error("Error: Identifier already declared");
//        }
//        Symbol symbol;
//        symbol.identifier_name = node->leftChild->data.value;
//        symbol.value = 0;  // Initialize the variable to 0
//        symbolTable.insert(symbol);
//    }

//    void executeProcedureStatement(Nodes* node) {
//        // Push the current program counter onto the call stack
//        callStack.push(programCounter);
//
//        // Set the program counter to the start of the procedure
//        programCounter = /* the start of the procedure */;
//
//        // Execute the procedure's statements
//        while (/* the procedure has not returned */) {
//            executeStatement(/* the current statement */);
//        }
//
//        // Pop the program counter from the call stack
//        programCounter = callStack.top();
//        callStack.pop();
//    }
//
//    int executeFunctionStatement(Nodes* node) {
//        // Push the current program counter onto the call stack
//        callStack.push(programCounter);
//
//        // Set the program counter to the start of the function
//        programCounter = /* the start of the function */;
//
//        // Execute the function's statements
//        int returnValue;
//        while (/* the function has not returned */) {
//            returnValue = executeStatement(/* the current statement */);
//        }
//
//        // Pop the program counter from the call stack
//        programCounter = callStack.top();
//        callStack.pop();
//
//        // Return the function's return value
//        return returnValue;
//    }


};

#endif //ABSTRACT_SYNTAX_TREE_INTERPRETER_HPP
