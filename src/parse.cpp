#include "lib/parse.h"


int main() {
    std::string input = "";
    std::string line;
    
    while(!std::cin.eof()) {
        char c;
        std::cin.get(c);
        line = std::string(1, c);
        input += line;
    }
    input = input.substr(0, input.size()-1);

    //std::cout << input << std::endl;
    
    //std::string input = "(* (+ 1 2) 3 (/ 4 5 (- 6 7)))";
    //std::string input = "(/ 1 0)";
    //std::string input = "7";
    //std::string input = "(1 * 2 * 3)";

    
    if (input.size() == 0) 
    {
        std::cout << "Unexpected token at line 1 column 1: END" << std::endl;
        return 2; 
        
    }
    bool empty = true;
    int index = 1;
    int l = 1;
    for (int i=0; i < (int)input.size(); i++)
    {
        if (input.at(i) == '\n') { l += 1; index = 1; }
        else if (input.at(i) == '\t' || input.at(i) == ' ') { index = +1; }
        else { empty = false; break; }
    }
    if (empty) {
        std::cout << "Unexpected token at line " << l << " column " << index << ": END" << std::endl;
        return 2; 
    }
    //if (input.at(0) != '(') 
    //{
    //    std::cout << "Unexpected token at line " << 1 << "  column  " << 1 << ": " << input.at(0) << std::endl;
//
    //    return 2; 
   //     
   // }
    
   // if (input.at(input.size()-1) != ')') 
    //{
     //   std::cout << "Unexpected token at line " << Parser::ErrorToken.line << "  column  " << Parser::ErrorToken.index << ": " << Parser::ErrorToken.content << std::endl;

     //   return 2; 
        
    //}
    
    
    
    
    std::vector<Token> TokenVector = Token::GenTokenVector(input);
    // Check ERROR
    Token lastToken = TokenVector.back();
    if (lastToken.type == TokenType::error)
    {
        std::cout << "Syntax error on line " << lastToken.line << " column " << lastToken.index << "." << std::endl;
        return 1;
    }

    // Multiple expression
    int right = findRightParenthesis(TokenVector, 1, (int)TokenVector.size()-2);
    // std::cout << right << std::endl;
    // std::cout << (int)TokenVector.size()-2 << std::endl;
    if (TokenVector[0].type == TokenType::leftParenthesis) {
        if (right != (int)TokenVector.size()-2) 
        {  
            std::cout << "Unexpected token at line " << TokenVector[right+1].line << " column " << TokenVector[right+1].index << ": " << TokenVector[right+1].content << std::endl;
            return 2;
        }
    }
    else {
        // not started with (. must be a single number
        
        // not a number
        //if (TokenVector[0].type != TokenType::number) 
        //{
            //std::cout << "Unexpected token at line " << TokenVector[0].line << " column " << TokenVector[0].index << ": " << TokenVector[0].content << std::endl;
        //}
        
        // multiple numbers
        if (TokenVector.size() > 2 && TokenVector[0].type == TokenType::number)
        {  
            std::cout << "Unexpected token at line " << TokenVector[1].line << " column " << TokenVector[1].index << ": " << TokenVector[1].content << std::endl;
            return 2;
        }
    }


    for (int i=0; i < (int)TokenVector.size(); i++)
    {
        if (TokenVector[i].type == TokenType::rightParenthesis)
        {
            int left = findLeftParenthesis(TokenVector, 0, i-1);
            if (left < 0)
            {
                std::cout << "Unexpected token at line " << TokenVector[0].line << " column " << TokenVector[0].index << ": " << TokenVector[0].content << std::endl;
                return 2;
            }
            
        }
    }
    

    
    Node root = Parser::MakeTree(TokenVector, 0, TokenVector.size() - 2);
    if (Parser::ErrorToken.type != TokenType::none)
    {
        std::cout << "Unexpected token at line " << Parser::ErrorToken.line << " column " << Parser::ErrorToken.index << ": " << Parser::ErrorToken.content << std::endl;
        return 2;
    }
    Parser::print(root);
    std::cout << std::endl;
    
    double result= Parser::calculate(root);
    if (Parser::ErrorToken.type != TokenType::none)
    {
        std::cout << "Runtime error: division by zero." << std::endl;
        return 3;
    }

    std::cout << result << std::endl;
    
    
    return 0;
}
