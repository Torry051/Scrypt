#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <cmath>


enum class TokenType {
    none,               //
    variable,           // a b c
    number,             // 1 2 3

    leftParenthesis,    // (
    rightParenthesis,   // )

    multiply,           // *
    divide,             // /
    mod,                // %

    plus,               // +
    minus,              // -

    smaller,            // <
    bigger,             // >
    smaller_equal,      // <=
    bigger_equal,       // >=

    equality,           // ==
    inequality,         // !=

    And,                // &

    exclusive_or,       // ^

    inclusive_or,       // |

    equals,             // =

    end,                // END
    error,              // ERROR



    leftBracket,    // {
    rightBracket,   // }
    True,           
    False,
    wWile,
    If,
    Print
};

class Token {

public:
    Token(TokenType itype, std::string icontent, int iline, int iindex, double ivalue = -1);
    ~Token() = default;

    operator double() const {
        return value;
    }

public:
    static std::pair<int, int> GenTokenVector(const std::string& input, std::vector<Token>& res);  // returns a vector of tokens created from a string.
    static void printLexer(std::vector<Token> TokenVector);
    static void printLexer(const std::string input);
     
public:
    TokenType type;
    std::string content;    // value as a string
    int line = -1;
    int index = -1;
    double value = -1;       // a number has its own value. otherwise -1

};
