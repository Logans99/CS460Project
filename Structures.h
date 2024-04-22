//
// Created by chrisxo on 2/14/24.
//

#ifndef REMOVE_COMMENTS_STRUCTS_H
#define REMOVE_COMMENTS_STRUCTS_H

// the different states the buffer may rest on
enum class State {
    CODE,
    STRING,
    SLASH,
    LINE_COMMENT,
    START_BLOCK,
    END_BLOCK,
    RISK,
    ERROR
};


enum class TokenType {
    IDENTIFIER,
    PROCEDUR,
    FUNCTION,
    FOR,
    WHILE,
    IF,
    RETURN,
    ELSE,
    PrintF,
    DATATYPE_SPECIFIER,
    INTEGER,
    LEFT_PARENTHESIS,
    RIGHT_PARENTHESIS,
    LEFT_BRACE,
    RIGHT_BRACE,
    SEMICOLON,
    ASSIGNMENT,
    PLUS,
    ASTERISK,
    MODULO,
    EQUAL,
    DOUBLE_QUOTE,
    STRING,
    COMMA,
    SINGLE_QUOTE,
    GREATER_THAN_OR_EQUAL,
    LESS_THAN_OR_EQUAL,
    BOOLEAN_AND,
    LESS_THAN,
    GREATER_THAN,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    SUBTRACTION,
    DIVIDE,
    END
    // Add other token types as needed
};


struct Token {
    TokenType type;
    std::string value;
    int linefound;
};

// stores the cumulative output of uncommented code in str
struct StringData {
    std::vector<char> str;
    int currentLine;
    int debugLine;
    int numAllocated;
    char openingString;
};

#endif //REMOVE_COMMENTS_STRUCTS_H
