#ifndef TOKEN_H_
#define TOKEN_H_

#include <iostream>

using namespace std;

class token // Token class to tokenize the input string
{
private:
    string type;
    string val;

public:
    void setType(const string &sts);
    void setValue(const string &str);
    string getType();
    string getValue();
    bool operator!=(token t); // Overloaded operator to compare two tokens
};
// getters and setters for type and value of token
void token::setType(const string &str)
{
    type = str;
}

void token::setValue(const string &str)
{
    val = str;
}

string token::getType()
{
    return type;
}

string token::getValue()
{
    return val;
}
#endif
