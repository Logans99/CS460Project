/** ************************************************************************
 * -C++ program to remove all C/C++ based comments within a program        *
 *  and tokenize remaining code                                            *
 *                                                                         *
 * -Processing is based on a DFA                                           *
 *    by Chris Bartha * Reed LeBlanc * Logan Santos * Raul Gallardo        *
 **************************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>

#include "SymbolTable.h"
#include "Transition.h"
#include "Structures.h"
#include "nodestruc.h"
#include "parser.h"
#include <stack>


std::vector<Token> tokenize(const std::string& code) {
    std::vector<Token> tokens;
    std::string currentToken;
    TokenType currentType = TokenType::IDENTIFIER; // Default type
    bool inString = false;
    bool inChar = false;
    int lineNumber = 1;

    for (size_t i = 0; i < code.length(); ++i) {
        char c = code[i];
        if (c == '\n') {
            lineNumber++;
        }
        if (c == '\"' && !inChar && (i == 0 || code[i - 1] != '\\')) { // Start or end of a string
            if (inString) {
                // End of string, add string token
                tokens.push_back({currentType, currentToken,lineNumber});
                currentToken.clear();
            }
            // Add double quote token
            tokens.push_back({TokenType::DOUBLE_QUOTE, "\"",lineNumber});
            inString = !inString;
            if (inString) {
                // Start of string
                currentType = TokenType::STRING;
            }
        } else if (c == '\''&& !inString) { // Start or end of a char
            if (inChar) {
                // End of char, add char token
                tokens.push_back({currentType, currentToken,lineNumber});
                currentToken.clear();
            }
            // Add single quote token
            tokens.push_back({TokenType::SINGLE_QUOTE, "\'",lineNumber});
            inChar = !inChar;

            if (inChar) {
                // Start of char
                currentType = TokenType::STRING;
            }
        } else if (inString || inChar) { // Inside a string
            currentToken += c;

        } else if (isalpha(c) || c == '_') { // Start of an identifier
//            if (!currentToken.empty() && currentType == TokenType::INTEGER) {
//                throw std::runtime_error("Syntax error on line " + std::to_string(lineNumber) + ": invalid integer");
//            }
            currentToken += c;
            currentType = TokenType::IDENTIFIER;
        } else if (isdigit(c)) { // Start of an integer
            currentToken += c;
            currentType = TokenType::INTEGER;
        } else if (isdigit(c) || (c == '-' && i < code.length() - 1 && isdigit(code[i + 1]))) { // Start of an integer
            currentToken += c;
            currentType = TokenType::INTEGER;
            // Check if the integer is followed by an invalid character
            if (i < code.length() - 1 && !isspace(code[i + 1]) && !ispunct(code[i + 1]) && isdigit(code[i + 2])) {
                throw std::runtime_error("Syntax error on line " + std::to_string(lineNumber) + ": invalid integer");
            }
        } else if (isspace(c) || ispunct(c)) { // End of a token
            if (!currentToken.empty()) {
                if (currentType == TokenType::IDENTIFIER && isdigit(currentToken[0])) {
                    throw std::runtime_error("Syntax error on line " + std::to_string(lineNumber) + ": invalid integer");
                }

                if (currentToken == "int") {
                    tokens.push_back({TokenType::DATATYPE_SPECIFIER, currentToken,lineNumber});
                    currentToken.clear();
                } else if (currentToken == "bool") {
                    tokens.push_back({TokenType::DATATYPE_SPECIFIER, currentToken,lineNumber});
                    currentToken.clear();
                } else if (currentToken == "char") {
                    tokens.push_back({TokenType::DATATYPE_SPECIFIER, currentToken,lineNumber});
                    currentToken.clear();
                } else if (currentToken == "void") {
                    tokens.push_back({TokenType::DATATYPE_SPECIFIER, currentToken,lineNumber});
                    currentToken.clear();
                } else if (currentToken == "if") {
                    tokens.push_back({TokenType::IF, currentToken,lineNumber});
                    currentToken.clear();
                } else if (currentToken == "procedure") {
                    tokens.push_back({TokenType::PROCEDUR, currentToken,lineNumber});
                    currentToken.clear();
                } else if (currentToken == "function") {
                    tokens.push_back({TokenType::FUNCTION, currentToken,lineNumber});
                    currentToken.clear();
                } else if (currentToken == "for") {
                    tokens.push_back({TokenType::FOR, currentToken,lineNumber});
                    currentToken.clear();
                } else if (currentToken == "else") {
                    tokens.push_back({TokenType::ELSE, currentToken,lineNumber});
                    currentToken.clear();
                } else if (currentToken == "printf") {
                    tokens.push_back({TokenType::PrintF, currentToken,lineNumber});
                    currentToken.clear();
                } else if (currentToken == "while") {
                    tokens.push_back({TokenType::WHILE, currentToken,lineNumber});
                    currentToken.clear();
                } else if (currentToken == "return") {
                    tokens.push_back({TokenType::RETURN, currentToken,lineNumber});
                    currentToken.clear();
                } else {
                    tokens.push_back({currentType, currentToken,lineNumber});
                    currentToken.clear();
                }

            }
            // Then handle punctuation and whitespace as before
            switch (c) {
                case '-':
                    tokens.push_back({TokenType::SUBTRACTION, "-",lineNumber});
                    break;
                case '(':
                    tokens.push_back({TokenType::LEFT_PARENTHESIS, "(",lineNumber});
                    break;
                case ')':
                    tokens.push_back({TokenType::RIGHT_PARENTHESIS, ")",lineNumber});
                    break;
                case '{':
                    tokens.push_back({TokenType::LEFT_BRACE, "{",lineNumber});
                    break;
                case '}':
                    tokens.push_back({TokenType::RIGHT_BRACE, "}",lineNumber});
                    break;
                case ';':
                    tokens.push_back({TokenType::SEMICOLON, ";",lineNumber});
                    break;
                case '+':
                    tokens.push_back({TokenType::PLUS, "+",lineNumber});
                    break;
                case '*':
                    tokens.push_back({TokenType::ASTERISK, "*",lineNumber});
                    break;
                case '/':
                    tokens.push_back({TokenType::DIVIDE, "/",lineNumber});
                    break;
                case '%':
                    tokens.push_back({TokenType::MODULO, "%",lineNumber});
                    break;
                case ',':
                    tokens.push_back({TokenType::COMMA, ",",lineNumber});
                    break;
                case '[':
                    tokens.push_back({TokenType::LEFT_BRACKET, "[",lineNumber});
                    break;
                case ']':
                    tokens.push_back({TokenType::RIGHT_BRACKET, "[",lineNumber});
                    break;
                case '=':
                    if (i < code.length() - 1 && code[i + 1] == '=') {
                        tokens.push_back({TokenType::EQUAL, "==",lineNumber});
                        ++i; // Skip the next character
                    } else {
                        tokens.push_back({TokenType::ASSIGNMENT, "=",lineNumber});
                    }
                    break;
                case '<':
                    if (i < code.length() - 1 && code[i + 1] == '=') {
                        tokens.push_back({TokenType::LESS_THAN_OR_EQUAL, "<=",lineNumber});
                        ++i; // Skip the next character
                    } else {
                        tokens.push_back({TokenType::LESS_THAN, "<",lineNumber});
                    }
                    break;
                case '>':
                    if (i < code.length() - 1 && code[i + 1] == '=') {
                        tokens.push_back({TokenType::GREATER_THAN_OR_EQUAL, ">=",lineNumber});
                        ++i; // Skip the next character
                    } else {
                        tokens.push_back({TokenType::GREATER_THAN, ">",lineNumber});
                    }
                    break;
                case '&':
                    if (i < code.length() - 1 && code[i + 1] == '&') {
                        tokens.push_back({TokenType::BOOLEAN_AND, "&&",lineNumber});
                        ++i; // Skip the next character
                    }
                    break;
                    // Add other cases as needed
                default:
                    // Handle identifiers, integers, strings, etc.
                    break;
            }
        }
    }

    return tokens;
}

int precedence(const std::string& op) {
    if (op == "+" || op == "-") {
        return 1;
    } else if (op == "*" || op == "/") {
        return 2;
    } else {
        return 0;
    }
}
void infix_maker(std::vector<Nodes *> currexppresion,Nodes * root) {
    std::vector<Nodes *> postfix;
    std::stack<Nodes *> operators;

    auto it = currexppresion.begin();
    while (it != currexppresion.end()) {
        const auto& token = *it;

        if (token->data.type==TokenType::INTEGER || token->data.type==TokenType::STRING||(std::isalpha(token->data.value[0]) || token->data.value[0] == '\''|| token->data.value[0] == '\"'||token->data.value[0] == '['||token->data.value[0] == ']'||token->data.value[0] == '!')) {
            postfix.push_back(token);
            if (std::isalpha(token->data.value[0]) && (it + 1 != currexppresion.end()) && (*(it + 1))->data.value == "(") {
                // Iterate until finding the corresponding ')'

                   while((*it)->data.value != ")"){

                       ++it;
                       postfix.push_back(*it);
                   }

               }




        } else if (token->data.value == "(") {
            operators.push(token);
        } else if (token->data.value == ")") {
            while (!operators.empty() && operators.top()->data.value != "(") {
                postfix.push_back(operators.top());
                operators.pop();
            }
            operators.pop();
        } else {
            while (!operators.empty() && operators.top()->data.value != "(" && precedence(operators.top()->data.value) >= precedence(token->data.value)) {
                postfix.push_back(operators.top());
                operators.pop();
            }
            operators.push(token);
        }
        ++it; // Move to the next token
    }

    while (!operators.empty()) {
        postfix.push_back(operators.top());
        operators.pop();
    }
    int val=2;
    for (const auto& token : postfix) {
        if(token->data.value==";"){
            Token NewToken;
            NewToken.type=TokenType::FOR;
            NewToken.value = "FOR LOOP EXPRESSION " + std::to_string(val);
            NewToken.linefound=0;
            insertchild(root, makenode(NewToken));
            insertsibiling(root,makenode(token->data));
            val++;



        }else {
            insertsibiling(root, makenode(token->data));
        }
    }

    postfix.clear();
}






Nodes * Make_new_tree(Nodes *root) {
    if (root == nullptr) {
        return root;
    }

    std::stack<Nodes*> nodesStack;
    nodesStack.push(root);
    std::vector<Nodes *> fix_notation;
    Token Head;
    Head.value="TREE HEAD";
    Nodes* newroot = makenode(Head);
    while (!nodesStack.empty()) {

        Nodes *current = nodesStack.top();
        nodesStack.pop();



        if(current== nullptr){
            return newroot;
        }
        if (current->data.type == TokenType::FUNCTION||current->data.type == TokenType::PROCEDUR ||current->data.type == TokenType::DATATYPE_SPECIFIER) {


            if(current->data.type != TokenType::DATATYPE_SPECIFIER) {
                while (current->rightSibling != nullptr) {


                    current = current->rightSibling;


                }
            }else {
                while (current->rightSibling != nullptr) {
                    if(current->data.type == TokenType::COMMA){
                        Token NewToken;
                        NewToken.type=TokenType::IDENTIFIER;
                        NewToken.value="DECLERATION";
                        NewToken.linefound=0;

                        insertchild(newroot,makenode(NewToken));
                    }
                    current = current->rightSibling;
                }



            }

            Token NewToken;
            NewToken.type=TokenType::IDENTIFIER;
            NewToken.value="DECLERATION";
            NewToken.linefound=0;

            insertchild(newroot,makenode(NewToken));

            if(current->data.type == TokenType::IDENTIFIER ){
                current = current->rightSibling;
            }
            if(current->data.type == TokenType::RIGHT_PARENTHESIS ||current->data.type == TokenType::SEMICOLON){
                current = current->leftChild;
                if(current == nullptr){
                    return  newroot;
                }
                if(current->data.type == TokenType::LEFT_BRACE){
                    Token NewToken;
                    NewToken.type=TokenType::LEFT_BRACE;
                    NewToken.value="BEGINBLOCK";
                    NewToken.linefound=0;

                    insertchild(newroot, makenode(NewToken));

                    current = current->leftChild;


                }

            }
            nodesStack.push(current);


        }else{
            if(current->data.type == TokenType::IF||current->data.type == TokenType::FOR||current->data.type == TokenType::WHILE){
                if(current->data.type == TokenType::IF){
                    Token NewToken;
                    NewToken.type=TokenType::IF;
                    NewToken.value="IF";
                    NewToken.linefound=0;

                    insertsibiling(newroot,makenode(NewToken));
                }else if(current->data.type == TokenType::FOR){
                    Token NewToken;
                    NewToken.type=TokenType::FOR;
                    NewToken.value="FOR LOOP EXPERSION 1";
                    NewToken.linefound=0;
                    insertsibiling(newroot,makenode(NewToken));
                }else{
                    Token NewToken;
                    NewToken.type=TokenType::WHILE;
                    NewToken.value="WHILE";
                    NewToken.linefound=0;

                    insertsibiling(newroot,makenode(NewToken));
                }
                current = current->rightSibling;
                while (current->rightSibling != nullptr){

                    fix_notation.push_back(current);

                    current = current->rightSibling;
                }
                fix_notation.push_back(current);
                infix_maker(fix_notation,newroot);
                fix_notation.clear();
                current = current->leftChild;

                nodesStack.push(current);


            }
            else if(current->data.type == TokenType::ELSE){
                    Token NewToken;
                    NewToken.type=TokenType::ELSE;
                    NewToken.value="ELSE";
                    NewToken.linefound=0;
                    current = current->leftChild;
                    insertchild(newroot, makenode(NewToken));
                    nodesStack.push(current);
                }


            else if(current->data.type == TokenType::LEFT_BRACE){

                Token NewToken;
                NewToken.type=TokenType::LEFT_BRACE;
                NewToken.value="BEGINBLOCK";
                NewToken.linefound=0;

                insertchild(newroot, makenode(NewToken));

                current = current->leftChild;

                nodesStack.push(current);
            }else if(current->data.type == TokenType::RIGHT_BRACE){

                Token NewToken;
                NewToken.type=TokenType::RIGHT_BRACE;
                NewToken.value="ENDBLOCK";
                NewToken.linefound=0;

                insertchild(newroot, makenode(NewToken));

                current = current->leftChild;

                nodesStack.push(current);



            }else if(current->data.type == TokenType::RETURN||current->data.type == TokenType::PrintF) {
                insertchild(newroot, makenode(current->data));
                while (current->rightSibling != nullptr) {

                    current =current->rightSibling;
                    if(current->data.value!=";"&&current->data.value!="\""&&current->data.value!=","&&current->data.value!="("&&current->data.value!=")"){
                    insertsibiling(newroot, makenode(current->data));
                    }

                }
                current = current->leftChild;
                nodesStack.push(current);
            }

            else{
                if(current->data.type==TokenType::IDENTIFIER && current->rightSibling->data.type ==TokenType::LEFT_PARENTHESIS ){
                    Token NewToken;
                    NewToken.type=TokenType::IDENTIFIER;
                    NewToken.value="CALL";
                    NewToken.linefound=0;
                    insertchild(newroot, makenode(NewToken));
                }else {
                    Token NewToken;
                    NewToken.type = TokenType::ASSIGNMENT;
                    NewToken.value = "ASSIGMENT";
                    NewToken.linefound = 0;
                    insertchild(newroot, makenode(NewToken));
                }

                while (current->rightSibling != nullptr){

                    fix_notation.push_back(current);

                    current = current->rightSibling;
                }
                infix_maker(fix_notation,newroot);
                fix_notation.clear();
                if(current->data.type == TokenType::SEMICOLON){

                    nodesStack.push(current->leftChild);
                }
            }
        }
    }

  return newroot;
}



int main (int argc, char *argv[]) {
    std::string fileName;

    if (argc == 2) // input file specified
        fileName = argv[1];
    else // default input file is input3.txt4ê
        fileName = std::to_string(1);

    fileName = "input" + fileName + ".txt";

    std::ifstream inputFile(fileName);

    if (!inputFile.is_open()) {
        std::cerr << "Error opening file: " << fileName << std::endl;
        return 1;
    }

    std::string input; // the entire input
    std::string inputLine; // line-by-line variable input

    while (std::getline(inputFile, inputLine))
        input += inputLine + "\n";  // concatenate the line and a newline character

    // declare state, starting state being CODE
    State currentState = State::CODE;

    // declare StringData output and its variables
    auto output = std::make_unique<StringData>();
    output->currentLine = output->debugLine = output->numAllocated = 1;

    // transition function, running through all until after \n
    for (const char *p = input.c_str(); *p != '\0'; ++p)
        Transition(*p, currentState, output.get());

    // ending on START_BLOCK state implies unterminated comment
    // ending on ERROR implies */ without /*, or unterminated quote
    if (currentState == State::START_BLOCK || currentState == State::ERROR)
        std::cout << "ERROR: Program contains C-style, unterminated comment on line "
                  << output->debugLine
                  << std::endl;
    else // everything is fine
        //std::cout << output->str.data(); // print out uncommented code
    inputFile.close(); // close the read file

    std::string code = output->str.data();
    std::vector<Token> tokens = tokenize(code);
    Parser par(tokens);
    par.parse();

    // Print the symbol table
    printf("\n");

    Make_new_tree(par.return_root());
    print(Make_new_tree(par.return_root()));
    delete(par.return_root());

//    for (const Token& token : tokens) {
//        std::cout << "Token type: ";
//        switch (token.type) {
//            case TokenType::IDENTIFIER:
//                std::cout << "IDENTIFIER";
//                break;
//            case TokenType::DATATYPE_SPECIFIER:
//                std::cout << "DATATYPE_SPECIFIER";
//                break;
//            case TokenType::INTEGER:
//                std::cout << "INTEGER";
//                break;
//            case TokenType::LEFT_PARENTHESIS:
//                std::cout << "LEFT_PARENTHESIS";
//                break;
//            case TokenType::RIGHT_PARENTHESIS:
//                std::cout << "RIGHT_PARENTHESIS";
//                break;
//            case TokenType::LEFT_BRACE:
//                std::cout << "LEFT_BRACE";
//                break;
//            case TokenType::RIGHT_BRACE:
//                std::cout << "RIGHT_BRACE";
//                break;
//            case TokenType::SEMICOLON:
//                std::cout << "SEMICOLON";
//                break;
//            case TokenType::ASSIGNMENT:
//                std::cout << "ASSIGNMENT";
//                break;
//            case TokenType::PLUS:
//                std::cout << "PLUS";
//                break;
//            case TokenType::ASTERISK:
//                std::cout << "ASTERISK";
//                break;
//            case TokenType::MODULO:
//                std::cout << "MODULO";
//                break;
//            case TokenType::COMMA:
//                std::cout << "COMMA";
//                break;
//            case TokenType::DOUBLE_QUOTE:
//                std::cout << "DOUBLE_QUOTE";
//                break;
//            case TokenType::SINGLE_QUOTE:
//                std::cout << "SINGLE_QUOTE";
//                break;
//            case TokenType::STRING:
//                std::cout << "STRING";
//                break;
//            case TokenType::GREATER_THAN_OR_EQUAL:
//                std::cout << "GREATER_THAN_OR_EQUAL";
//                break;
//            case TokenType::LESS_THAN_OR_EQUAL:
//                std::cout << "LESS_THAN_OR_EQUAL";
//                break;
//            case TokenType::BOOLEAN_AND:
//                std::cout << "BOOLEAN_AND";
//                break;
//            case TokenType::LESS_THAN:
//                std::cout << "LESS_THAN";
//                break;
//            case TokenType::GREATER_THAN:
//                std::cout << "GREATER_THAN";
//                break;
//            case TokenType::LEFT_BRACKET:
//                std::cout << "LEFT_BRACKET";
//                break;
//            case TokenType::RIGHT_BRACKET:
//                std::cout << "RIGHT_BRACKET";
//                break;
//            case TokenType::SUBTRACTION:
//                std::cout << "SUBTRACTION";
//                break;
//            case TokenType::DIVIDE:
//                std::cout << "DIVIDE";
//                break;
//                // Add other cases as needed
//        }
//        std::cout << "\nToken: " << token.value << "\n";
//    }

    return 0;
}
