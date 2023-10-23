#include "lib/ParserB.h"

int main() {
#if DEBUG == 0
    std::string input = "";
    std::string line;
    
    while(!std::cin.eof()) {
        char c;
        std::cin.get(c);
        line = std::string(1, c);
        input += line;
    }
    input = input.substr(0, input.size()-1);
    // std::cout << "LOLOLOLOLOLOLOLOLO" << std::endl;
    // std::cout << input << std::endl;
    // std::cout << "LOLOLOLOLOLOLOLOLO" << std::endl;
#endif
#if DEBUG == 1
    
    //std::string input = "x = y = 0 + 1 + 2 * 3 - 4 / (5 + 6)";
    //std::string input = "b=13\n(7 - (b = (b + 5)))";
    std::string input = "b=13\n(7 - (b = (b + 5)))\n1 % 114514\n\n1=a\n1+1\n";
#endif

    if (input.size() == 0) { return 0; }
    std::vector<std::string> expressions;
    std::stringstream ss(input);
    std::string s;

    while (std::getline(ss, s, '\n')) {
        expressions.push_back(s);
    }

    for (std::string expression : expressions)
    {
        if (expression.length() == 0)
        {
            continue;
        }

        // Lexer
        std::vector<Token> TokenVector;
        std::pair<int, int> errorPair = Token::GenTokenVector(expression, TokenVector);
        if (errorPair.first != -1)
        {
            std::cout << "Syntax error on line " << errorPair.first << " column " << errorPair.second << "." << std::endl;
            exit(1);
        }

        // ParserB
        Node root;
        std::pair<std::pair<int, int>, std::string> errorResult = ParserB::MakeTreeInfix(TokenVector, 0, TokenVector.size() - 2, root);
        if (errorResult.first.first != -1) 
        {
            std::cout << "Unexpected token at line " << errorResult.first.first << " column " << errorResult.first.second << ": " << errorResult.second << std::endl;
            exit(2);
        }

        ParserB::print(root);
        std::cout << std::endl;
        //std::cout << ParserB::calculate(root) << std::endl;
    }
    return 0;
}