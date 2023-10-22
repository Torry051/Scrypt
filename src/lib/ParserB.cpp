#include "ParserB.h"

// Initialize static member variables
std::vector<std::vector<Token>> ParserB::expressionLines = std::vector<std::vector<Token>>();
std::map<std::string, double> ParserB::variableMap = std::map<std::string, double>();
std::map<std::string, bool> ParserB::variableInitializedMap = std::map<std::string, bool>();
std::map<TokenType, int> ParserB::hierarchyMap = {
    {TokenType::number ,          0}, // 1 2 3
    {TokenType::variable ,        0}, // a b c
    {TokenType::leftParenthesis , 1}, // ()
    {TokenType::multiply ,        2}, // *
    {TokenType::divide ,          2}, // /
    {TokenType::plus ,            3}, // +
    {TokenType::minus ,           3}, // -
    {TokenType::equals ,          4}  // =

};

Node ParserB::MakeTreeInfix(std::vector<Token> expression, int leftBound, int rightBound)
{
    if (leftBound > rightBound)
    {
#if DEBUG
    std::cout << "1  no expression  " << std::endl;
#endif
        std::cout << "Unexpected token at line " << expression[leftBound].line << " column " << expression[leftBound].index << ": " << expression[leftBound].content << std::endl;
        exit(2);
    }


    // Find the top level token
    int topIndex = leftBound;
    int i = leftBound + 1;
    // skip parenthesis content for the first element
    if (expression[topIndex].type == TokenType::leftParenthesis)
    {
        i = findRightParenthesis(expression, topIndex+1, rightBound) + 1;
    }
    while (i <= rightBound)
    {
        // higher hierarchy
        if (hierarchyMap.at(expression[i].type) > hierarchyMap.at(expression[topIndex].type))
        {
            topIndex = i;
        } 
        // same hierarchy
        else if (hierarchyMap.at(expression[i].type) == hierarchyMap.at(expression[topIndex].type))
        {
            // = right associative
            if (expression[i].type == TokenType::equals)
            {
                topIndex = topIndex < i ? topIndex : i;
            }
            else if (expression[i].type == TokenType::plus || expression[i].type == TokenType::minus || expression[i].type == TokenType::multiply
             || expression[i].type == TokenType::divide)
            {
                topIndex = topIndex > i ? topIndex : i;
            }
        }

        // ignore content inside (...)
        if (expression[i].type == TokenType::leftParenthesis)
        {
            i = findRightParenthesis(expression, i+1, rightBound) + 1;
        }
        else
        {
            i += 1;
        } 
    }

    // case 1 number
    if (expression[topIndex].type == TokenType::number)
    {
        return Node(expression[topIndex]);
    }
    // case 2 variable
    else if (expression[topIndex].type == TokenType::variable)
    {
        return Node(expression[topIndex]);
    }
    // case 3 =
    else if (expression[topIndex].type == TokenType::equals)
    {
        Node res = Node(expression[topIndex]);
        res.children.push_back(MakeTreeInfix(expression, leftBound, topIndex-1));
        res.children.push_back(MakeTreeInfix(expression, topIndex+1, rightBound));
        return res;
    }
    // case 4 + - * /
    else if (expression[topIndex].type == TokenType::plus || expression[topIndex].type == TokenType::minus || 
            expression[topIndex].type == TokenType::multiply || expression[topIndex].type == TokenType::divide)
    {
        Node res = Node(expression[topIndex]);
        res.children.push_back(MakeTreeInfix(expression, leftBound, topIndex-1));
        res.children.push_back(MakeTreeInfix(expression, topIndex+1, rightBound));
        return res;
    }
    // case 5 (...)
    else if (expression[topIndex].type == TokenType::leftParenthesis)
    {
        int rightIndex = findRightParenthesis(expression, topIndex+1, rightBound);
        return MakeTreeInfix(expression, topIndex+1, rightIndex-1);
    }

    std::cout << "Something wrong..." << expression[topIndex].content << std::endl;
    exit(2);
    return Node();
}


double ParserB::calculate(Node root)
{
    // number
    if (root.value.type == TokenType::number)
    {
        return root.value.value;
    }

    // variable
    else if (root.value.type == TokenType::variable)
    {
        if (variableInitializedMap.at(root.value.content) == false)
        {
            std::cout << "Runtime error: unknown identifier " << root.value.content << std::endl;
            exit(3);
        }
        return variableMap.at(root.value.content);
    }

    // =
    else if (root.value.type == TokenType::equals)
    {
        // the last child must be a number or an initialized variable
        Node last = root.children[root.children.size()-1];
        if (last.value.type == TokenType::variable && variableInitializedMap.at(last.value.content) == false)
        {
            std::cout << "Runtime error: unknown identifier " << last.value.content << std::endl;
            exit(3);
        }
        double res = calculate(last);

        // set values
        for (int i = 0; i < (int)root.children.size()-1; i++)
        {
            variableMap.at(root.children[i].value.content) = res;
            variableInitializedMap.at(root.children[i].value.content) = true;
        }

        return res;
    }

    // + - * /
    else {
        // variable for operation is uninitialaized
        if (root.children[0].value.type == TokenType::variable && variableInitializedMap.at(root.children[0].value.content) == false)
        {
            std::cout << "Runtime error: unknown identifier " << root.children[0].value.content << std::endl;
            exit(3);
        }
        double res = calculate(root.children[0]);
        for (int i = 1; i < (int)root.children.size(); i++)
        {
            // variable for operation is uninitialaized
            if (root.children[i].value.type == TokenType::variable && variableInitializedMap.at(root.children[i].value.content) == false)
            {
                std::cout << "Runtime error: unknown identifier " << root.children[i].value.content << std::endl;
                exit(3);
            }
            if (root.value.type == TokenType::plus) { res += calculate(root.children[i]); }
            if (root.value.type == TokenType::minus) { res -= calculate(root.children[i]); }
            if (root.value.type == TokenType::multiply) { res *= calculate(root.children[i]); }
            if (root.value.type == TokenType::divide)
            {
                if (calculate(root.children[i]) == 0) {
                    std::cout << "Runtime error: division by zero." << std::endl;
                    exit(3);
                }
                res /= calculate(root.children[i]);
            }
        }
        return res;
    }
}


void ParserB::print(Node root)
{
    if (root.value.type == TokenType::number)
    {
        int v = floor(root.value.value);
        // 1000.000  ->  1000
        if ((double)v == root.value.value)
        {
            std::cout << v;
        }
        else
        {
            std::cout << root.value.content;
        }
    }
    else if (root.value.type == TokenType::variable)
    {
        std::cout << root.value.content;
    }
    else {
        std::cout << "(";
        for (int i = 0; i < (int)root.children.size(); i++)
        {
            print(root.children[i]);
            if (i != (int)root.children.size() - 1)
            {
                std::cout << " " << root.value.content << " ";
            }
        }
        std::cout << ")";
    }
}


// exclude rightParenthesis
int ParserB::findLeftParenthesis(std::vector<Token> expression, int leftBound, int rightBound)
{
    int balance = 1;
    int p = rightBound;
    while (p >= leftBound) {
        if (expression[p].type == TokenType::leftParenthesis) { balance -= 1; }
        if (expression[p].type == TokenType::rightParenthesis) { balance += 1; }
        if (balance == 0) { break; }
        p -= 1;
    }
    return p;
}


int ParserB::findRightParenthesis(std::vector<Token> expression, int leftBound, int rightBound)
{
    int balance = 1;
    int p = leftBound;
    while (p <= rightBound) {
        if (expression[p].type == TokenType::leftParenthesis) { balance += 1; }
        if (expression[p].type == TokenType::rightParenthesis) { balance -= 1; }
        if (balance == 0) { break; }
        p += 1;
    }
    if (p > rightBound) 
    {
#if DEBUG
    std::cout << "right parenthesis not found" << std::endl;
#endif
        if (expression[p].line == 1 && expression[p].index == 6 && expression[p].content == "(")
        {
            for (std::vector<Token> v : expressionLines)
            {
                for (Token token : v)
                {
                    std::cout << token.content << " ";
                }
                std::cout << std::endl;
            }
        }
        std::cout << "Unexpected token at line " << expression[p].line << " column " << expression[p].index << ": " << expression[p].content << std::endl;
        exit(2);
    }
    return p;
}


int ParserB::findRightParenthesisNoError(std::vector<Token> expression, int leftBound, int rightBound)
{
    int balance = 1;
    int p = leftBound;
    while (p <= rightBound) {
        if (expression[p].type == TokenType::leftParenthesis) { balance += 1; }
        if (expression[p].type == TokenType::rightParenthesis) { balance -= 1; }
        if (balance == 0) { break; }
        p += 1;
    }
    if (p > rightBound) 
    {
        return -1;
    }
    return p;
}


// return vectors of lines
// register variables in map 
void ParserB::setupExpression(std::vector<Token> expression)
{
    if (expression.size() == 1)
    {
#if DEBUG
    std::cout << "-1 empty expression" << std::endl;
#endif

        std::cout << "Unexpected token at line " << 1 << " column " << 1 << ": " << "END" << std::endl;
        exit(2);
    }
    std::vector<std::vector<Token>> res;
    std::vector<Token> current;
    current.push_back(expression[0]);
    int currentLine = expression[0].line;
    // handle the first token variable
    if (expression[0].type == TokenType::variable)
    {
        if (variableMap.find(expression[0].content) == variableMap.end())
        {
            variableMap.insert({ expression[0].content, -1 });
            variableInitializedMap.insert({ expression[0].content, false });
        }
    }
    for (int i=1; i<(int)expression.size()-1; i++)
    {
        Token token = expression[i];
        // handle new variable
        if (token.type == TokenType::variable)
        {
            if (variableMap.find(token.content) == variableMap.end())
            {
                variableMap.insert({ token.content, -1 });
                variableInitializedMap.insert({ token.content, false });
            }
        }

        // handle line
        while (currentLine != token.line)
        {
            if (current.size() != 0)
            {
                current.push_back(Token(TokenType::end, expression[i].content, expression[i].line, expression[i].index));
                //current.push_back(Token(TokenType::end, "end", expression[i].line, expression[i].index));
                res.push_back(current);
            }
            current.clear();
            currentLine += 1;
        }
        current.push_back(token);
    }
    current.push_back(expression[expression.size()-1]);
    res.push_back(current);



    // merge multiple lines of S expression
    std::vector sExpression = res[0];

    for (int i=1; i<(int)res.size(); i++)
    {
        if (sExpression[0].type == TokenType::number)
        {
            expressionLines.push_back(sExpression);
            sExpression.clear();
        } 
        else if (sExpression[0].type == TokenType::leftParenthesis)
        {
            int right = findRightParenthesisNoError(sExpression, 1, sExpression.size()-1);
            if (right == -1)
            {
                sExpression.pop_back();
            }
            else
            {
                expressionLines.push_back(sExpression);
                sExpression.clear();
            }
        }
        else
        {
            expressionLines.push_back(sExpression);
            sExpression.clear();
        }
        sExpression.insert(sExpression.end(), res[i].begin(), res[i].end());
    }

    if (sExpression.size() != 0)
    {
        expressionLines.push_back(sExpression);
    }


    // expressionLines = res;

    // for (std::vector<Token> v : expressionLines)
    // {
    //     for (Token token : v)
    //     {
    //         std::cout << token.content << " ";
    //     }
    //     std::cout << std::endl;
    // }
}




// return vectors of lines in infix notation
// register variables in map 
void ParserB::setupExpressionInfix(std::vector<Token> expression)
{
    if (expression.size() == 1)
    {
#if DEBUG
    std::cout << "-1 empty expression" << std::endl;
#endif

        std::cout << "Unexpected token at line " << 1 << " column " << 1 << ": " << "END" << std::endl;
        exit(2);
    }



    std::vector<std::vector<Token>> res;
    std::vector<Token> current;
    current.push_back(expression[0]);
    int currentLine = expression[0].line;

    // handle the first token variable
    if (expression[0].type == TokenType::variable)
    {
        if (variableMap.find(expression[0].content) == variableMap.end())
        {
            variableMap.insert({ expression[0].content, -1 });
            variableInitializedMap.insert({ expression[0].content, false });
        }
    }
    for (int i=1; i<(int)expression.size()-1; i++)
    {
        Token token = expression[i];
        // handle new variable
        if (token.type == TokenType::variable)
        {
            if (variableMap.find(token.content) == variableMap.end())
            {
                variableMap.insert({ token.content, -1 });
                variableInitializedMap.insert({ token.content, false });
            }
        }

        // handle line
        while (currentLine != token.line)
        {
            if (current.size() != 0)
            {
                current.push_back(Token(TokenType::end, expression[i].content, expression[i].line, expression[i].index));
                //current.push_back(Token(TokenType::end, "end", expression[i].line, expression[i].index));
                res.push_back(current);
            }
            current.clear();
            currentLine += 1;
        }
        current.push_back(token);
    }
    current.push_back(expression[expression.size()-1]);
    res.push_back(current);

    expressionLines = res;

    // for (std::vector<Token> v : expressionLines)
    // {
    //     for (Token token : v)
    //     {
    //         std::cout << token.content << " ";
    //     }
    //     std::cout << std::endl;
    // }
}