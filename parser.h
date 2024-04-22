#include "SymbolTable.h"
#include <stack>
class Parser {
public:
    Parser(std::vector<Token>& tokens) : tokens(tokens), index(0) {}

    void parse() {
        while (index < tokens.size()) {

            parseProgram();

        }
        std::cout<<"done"<<std::endl;
        return_root();

    }
    Nodes * return_root(){
        return root;
    }

    SymbolTable& getSymbolTable() {
        return symbolTable;
    }

private:
    Nodes* root = nullptr;
    bool inforloop =false;
    std::vector<Token>& tokens;
    SymbolTable symbolTable;
    int scope = 0;
    bool globalScope = true; // Indicates whether the parser is in the global scope
//    std::stack<int> scopeStack;

    size_t index;

    void advance() {
        index++;
    }

    const Token& getCurrentToken() {
        return tokens[index];
    }

    void match(TokenType expectedType) {
        Nodes* newnode = makenode(getCurrentToken());
        static bool childflag = false;
        if(root == nullptr){

            root = newnode;

        }else if (getCurrentToken().value != "{" && getCurrentToken().value != "}" && getCurrentToken().value != ";") {
            if(childflag  ){
                insertchild(root,newnode);
                childflag=false;
            }else{
                insertsibiling(root,newnode);

            }

        }else if(getCurrentToken().value == ";"){
            if(inforloop){
                insertsibiling(root,newnode);
            }else{
                childflag=true;
                insertsibiling(root,newnode);
            }
        }else{
            childflag=true;
            insertchild(root,newnode);
        }

        if (index < tokens.size() && getCurrentToken().type == expectedType) {
            advance();
        }
        else {
            throw std::runtime_error("Syntax 1 error on line " + std::to_string(getCurrentToken().linefound) + ": Reserved word \"" + getCurrentToken().value + "\" cannot be used for name of a variable");

        }
    }

    void parseProgram() {

        parseMainProcedureorfunction();
        return;


    }

    void parseMainProcedureorfunction(){
        if (getCurrentToken().type == TokenType::PROCEDUR) {
            globalScope = false;
            parseMainProcedure();
            globalScope = true;
        } else if (getCurrentToken().type == TokenType::FUNCTION) {
            globalScope = false;
            parseFunction();
            globalScope = true;
        } else if(getCurrentToken().type == TokenType::DATATYPE_SPECIFIER){
            parseDeclarationStatement();
            match(TokenType::SEMICOLON);
        } else {
            throw std::runtime_error("Syntax error on line " + std::to_string(getCurrentToken().linefound) + ": Reserved word \"" + getCurrentToken().value + "\" cannot be used for name of a function");
        }
    }

    void parseParameterList(){
        parseDeclarationStatement();
    }

    void parseFunction() {
        scope++;
        match(TokenType::FUNCTION);

        Symbol functionSymbol;
        functionSymbol.scope = scope;
        functionSymbol.identifier_type = "function";
        functionSymbol.datatype = getCurrentToken().value; // Return type of the function
        match(TokenType::DATATYPE_SPECIFIER);

        functionSymbol.identifier_name = getCurrentToken().value; // Name of the function
        match(TokenType::IDENTIFIER);

        match(TokenType::LEFT_PARENTHESIS);
        int linefound = getCurrentToken().linefound;
        // If the next token is 'void', match it and skip the parameter processing loop
        if (getCurrentToken().value == "void") {
            match(TokenType::IDENTIFIER); // Match 'void' as an identifier
        } else {
            // Start of parameter processing loop
            while (getCurrentToken().type != TokenType::RIGHT_PARENTHESIS) {
                Symbol parameterSymbol;
                parameterSymbol.identifier_type = "parameter";
                parameterSymbol.datatype = getCurrentToken().value; // Parameter type
                match(TokenType::DATATYPE_SPECIFIER);

                parameterSymbol.identifier_name = getCurrentToken().value; // Parameter name
                match(TokenType::IDENTIFIER);

                // Check if the parameter is an array
                if (getCurrentToken().type == TokenType::LEFT_BRACKET) {
                    match(TokenType::LEFT_BRACKET);
                    parameterSymbol.datatype_is_array = true;
                    parameterSymbol.datatype_array_size = std::stoi(getCurrentToken().value); // Array size
                    match(TokenType::INTEGER);
                    match(TokenType::RIGHT_BRACKET);
                } else {
                    parameterSymbol.datatype_is_array = false;
                    parameterSymbol.datatype_array_size = 0; // No array size for non-array parameters
                }

                parameterSymbol.scope = scope; // Sets scope

                functionSymbol.parameters.push_back(parameterSymbol); // Add the parameter to the function symbol

                // If the next token is a comma, skip it and continue with the next parameter
                if (getCurrentToken().type == TokenType::COMMA) {
                    match(TokenType::COMMA);
                }
            }
            // End of parameter processing loop
        }

        match(TokenType::RIGHT_PARENTHESIS);

        symbolTable.insert(functionSymbol,linefound); // Add the function symbol to the symbol table

        parseBlockStatement(); // Parse the function body
    }

    void parseMainProcedure() {
        scope++;
        match(TokenType::PROCEDUR);

        Symbol procedureSymbol;
        procedureSymbol.scope = scope; // Scope of the procedure
        procedureSymbol.identifier_type = "procedure";
        procedureSymbol.identifier_name = getCurrentToken().value; // Name of the procedure
        procedureSymbol.datatype = "NOT APPLICABLE"; // Set the datatype for the procedure
        match(TokenType::IDENTIFIER);
        int linefound = getCurrentToken().linefound;

        match(TokenType::LEFT_PARENTHESIS);
        // If the next token is 'void', match it as a DATATYPE_SPECIFIER and skip the parameter processing loop
        if (getCurrentToken().value == "void") {
            match(TokenType::DATATYPE_SPECIFIER); // Match 'void' as a DATATYPE_SPECIFIER
        } else {
            // Start of parameter processing loop
            while (getCurrentToken().type != TokenType::RIGHT_PARENTHESIS) {
                Symbol parameterSymbol;
                parameterSymbol.identifier_type = "parameter";
                parameterSymbol.datatype = getCurrentToken().value; // Parameter type
                match(TokenType::DATATYPE_SPECIFIER);

                parameterSymbol.identifier_name = getCurrentToken().value; // Parameter name
                match(TokenType::IDENTIFIER);

                // Check if the parameter is an array
                if (getCurrentToken().type == TokenType::LEFT_BRACKET) {
                    match(TokenType::LEFT_BRACKET);
                    parameterSymbol.datatype_is_array = true;
                    parameterSymbol.datatype_array_size = std::stoi(getCurrentToken().value); // Array size
                    match(TokenType::INTEGER);
                    match(TokenType::RIGHT_BRACKET);
                } else {
                    parameterSymbol.datatype_is_array = false;
                    parameterSymbol.datatype_array_size = 0; // No array size for non-array parameters
                }

                parameterSymbol.scope = scope; // Sets scope

                procedureSymbol.parameters.push_back(parameterSymbol); // Add the parameter to the procedure symbol

                // If the next token is a comma, skip it and continue with the next parameter
                if (getCurrentToken().type == TokenType::COMMA) {
                    match(TokenType::COMMA);
                }
            }
            // End of parameter processing loop
        }

        match(TokenType::RIGHT_PARENTHESIS);
        symbolTable.insert(procedureSymbol,linefound); // Add the procedure symbol to the symbol table
        parseBlockStatement();
//        scope--;
    }

    void parseBlockStatement() {

        match(TokenType::LEFT_BRACE);
        while (getCurrentToken().type != TokenType::RIGHT_BRACE) {

            parseStatement();
        }

        match(TokenType::RIGHT_BRACE);
    }
    void parsesingalquote(){

        match(TokenType::SINGLE_QUOTE);

        match(TokenType::STRING);

        match(TokenType::SINGLE_QUOTE);

    }
    void parsedoublequote() {
        if (getCurrentToken().type != TokenType::DOUBLE_QUOTE) {
            throw std::runtime_error("Syntax error on line " + std::to_string(getCurrentToken().linefound) + ": expected double quote.");
        }

        match(TokenType::DOUBLE_QUOTE); // Match the opening double quote

        // Check if the next token is a string
        if (getCurrentToken().type != TokenType::STRING) {
            throw std::runtime_error("Syntax error on line " + std::to_string(getCurrentToken().linefound) + ": expected string after double quote.");
        }

        match(TokenType::STRING); // Match the string token

        // Check if the closing double quote is present
        if (getCurrentToken().type != TokenType::DOUBLE_QUOTE) {
            throw std::runtime_error("Syntax error on line " + std::to_string(getCurrentToken().linefound) + ": expected closing double quote.");
        }

        match(TokenType::DOUBLE_QUOTE); // Match the closing double quote
    }
    void parseExpression() {

        if (getCurrentToken().type == TokenType::LEFT_PARENTHESIS) {

            match(TokenType::LEFT_PARENTHESIS);
            parseExpression();

            if (getCurrentToken().type == TokenType::RIGHT_PARENTHESIS) {

                match(TokenType::RIGHT_PARENTHESIS);
            }


        }
        else if (getCurrentToken().type == TokenType::IDENTIFIER) {

            match(TokenType::IDENTIFIER);
            if(getCurrentToken().type==TokenType::LEFT_PARENTHESIS){
                parseExpression();
            }
            if(getCurrentToken().type==TokenType::LEFT_BRACKET){

                match(TokenType::LEFT_BRACKET);
                if(getCurrentToken().type==TokenType::IDENTIFIER) {

                    match(TokenType::IDENTIFIER);

                }else if(getCurrentToken().type==TokenType::INTEGER) {

                    match(TokenType::INTEGER);

                }

                match(TokenType::RIGHT_BRACKET);
            }


        } else if (getCurrentToken().type == TokenType::INTEGER) {

            match(TokenType::INTEGER);

        } else if (getCurrentToken().type == TokenType::DOUBLE_QUOTE) {
            parsedoublequote();

        } else{
            parsesingalquote();
        }
        if (getCurrentToken().type == TokenType::BOOLEAN_AND) {

            match(TokenType::BOOLEAN_AND);
            parseExpression();
        }
        if (getCurrentToken().type == TokenType::GREATER_THAN) {

            match(TokenType::GREATER_THAN);
            parseExpression();
        }
        if (getCurrentToken().type == TokenType::LESS_THAN) {

            match(TokenType::LESS_THAN);

            parseExpression();
        }
        if (getCurrentToken().type == TokenType::LESS_THAN_OR_EQUAL) {

            match(TokenType::LESS_THAN_OR_EQUAL);
            parseExpression();
        }
        if (getCurrentToken().type == TokenType::GREATER_THAN_OR_EQUAL) {

            match(TokenType::GREATER_THAN_OR_EQUAL);
            parseExpression();
        }
        if (getCurrentToken().type == TokenType::DIVIDE) {
            match(TokenType::DIVIDE);
            parseExpression();
        }
        if (getCurrentToken().type == TokenType::SUBTRACTION) {
            match(TokenType::SUBTRACTION);
            parseExpression();
        }
        if (getCurrentToken().type == TokenType::MODULO) {
            match(TokenType::MODULO);
            parseExpression();
        }
        if (getCurrentToken().type == TokenType::ASTERISK) {
            match(TokenType::ASTERISK);
            parseExpression();
        }
        if (getCurrentToken().type == TokenType::PLUS) {
            match(TokenType::PLUS);
            parseExpression();
        }
        if (getCurrentToken().type == TokenType::ASSIGNMENT) {
            match(TokenType::ASSIGNMENT);
            parseExpression();
        }
        if (getCurrentToken().type == TokenType::EQUAL) {
            match(TokenType::EQUAL);
            parseExpression();
        }
        if (getCurrentToken().type == TokenType::COMMA) {
            match(TokenType::COMMA);
            parseExpression();
        }
    }
    void parseIfStatement(){
        match(TokenType::IF);
        match(TokenType::LEFT_PARENTHESIS);
        parseExpression();
        match(TokenType::RIGHT_PARENTHESIS);

        parseBlockStatement();
    }
    void parseelsestatment(){
        match(TokenType::ELSE);
        parseBlockStatement();
    }
    void parseDeclarationStatement() {
        Symbol variableSymbol;
        variableSymbol.identifier_type = "datatype";
        variableSymbol.scope = globalScope ? 0 : scope;
        int linefound = getCurrentToken().linefound;
        if(getCurrentToken().type == TokenType::DATATYPE_SPECIFIER){
            variableSymbol.datatype = getCurrentToken().value; // Variable type
            match(TokenType::DATATYPE_SPECIFIER);

            while (true) { // Loop to handle multiple declarations
                variableSymbol.identifier_name = getCurrentToken().value; // Variable name
                match(TokenType::IDENTIFIER);
                if (getCurrentToken().type == TokenType::LEFT_BRACKET) {
                    match(TokenType::LEFT_BRACKET);
                    variableSymbol.datatype_is_array = true;
                    variableSymbol.datatype_array_size = std::stoi(getCurrentToken().value); // Array size
                    match(TokenType::INTEGER);
                    match(TokenType::RIGHT_BRACKET);
                } else {
                    variableSymbol.datatype_is_array = false;
                    variableSymbol.datatype_array_size = 0; // No array size for non-array variables
                }
                symbolTable.insert(variableSymbol,linefound); // Add the variable symbol to the symbol table

                if (getCurrentToken().type != TokenType::COMMA) {
                    break; // Exit the loop if there's no comma
                }
                match(TokenType::COMMA); // Match the comma and continue with the next variable
            }
        } else if (getCurrentToken().type == TokenType::ASSIGNMENT){
            match(TokenType::ASSIGNMENT);
            parseExpression();
        }
    }

    void parseAssignmentStatement(){
        match(TokenType::IDENTIFIER);

        if(getCurrentToken().type ==TokenType::LEFT_PARENTHESIS){

            match(TokenType::LEFT_PARENTHESIS);
            parseExpression();
            match(TokenType::RIGHT_PARENTHESIS);
        }else if(getCurrentToken().type ==TokenType::LEFT_BRACKET){

            match(TokenType::LEFT_BRACKET);
            parseExpression();
            match(TokenType::RIGHT_BRACKET);
            match(TokenType::ASSIGNMENT);
            parseExpression();
        }
        else {
            match(TokenType::ASSIGNMENT);
            parseExpression();
        }
    }
    void parseForStatement(){
        parseExpression();
        match(TokenType::SEMICOLON);
        parseExpression();
        match(TokenType::SEMICOLON);


        parseExpression();
        match(TokenType::RIGHT_PARENTHESIS);
        inforloop=false;
        parseBlockStatement();


    }
    void parseprintf(){
        match(TokenType::PrintF);
        match(TokenType::LEFT_PARENTHESIS); //
        match(TokenType::DOUBLE_QUOTE);

        if (getCurrentToken().type == TokenType::STRING) {
            match(TokenType::STRING);
        } else {

            throw std::runtime_error("Syntax error: Expected string literal in printf statement");
        }
        match(TokenType::DOUBLE_QUOTE);
        while (getCurrentToken().type == TokenType::COMMA) {
            match(TokenType::COMMA);
            parseExpression();
        }


        match(TokenType::RIGHT_PARENTHESIS);
        match(TokenType::SEMICOLON);

    }
    void parseWhileStatement(){
        match(TokenType::WHILE);
        match(TokenType::LEFT_PARENTHESIS);
        parseExpression();
        match(TokenType::RIGHT_PARENTHESIS);

        parseBlockStatement();

    }
    void parsreturn(){
        match(TokenType::RETURN);
        parseExpression();
    }
    void parseStatement() {
        TokenType currentTokenType = getCurrentToken().type;

        switch (currentTokenType) {
            case TokenType::IF:
                parseIfStatement();

                break;
            case TokenType::FOR:
                match(TokenType::FOR);
                inforloop=true;
                match(TokenType::LEFT_PARENTHESIS);
                parseForStatement();

                break;
            case TokenType::WHILE:
                parseWhileStatement();
                break;
            case TokenType::ELSE:

                parseelsestatment();

                break;
            case TokenType::DATATYPE_SPECIFIER:
                parseDeclarationStatement();
                match(TokenType::SEMICOLON);
                break;
            case TokenType::IDENTIFIER:

                parseAssignmentStatement();

                match(TokenType::SEMICOLON);

                break;

          case TokenType::PrintF:
               parseprintf();
               break;

            case TokenType::RETURN:
                 parsreturn();
                 match(TokenType::SEMICOLON);
                 break;

        }
    }

};

