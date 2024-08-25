#include <iostream>
#include <stack>
#include <iterator>
#include <string.h>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <math.h>
#include <stdio.h>

#include "token.h"
#include "tree.h"
#include "environment.h"

using namespace std;

stack<tree *> st; // Stack for syntax tree nodes
// defining the operators and keywords
char operators[] = {'+', '-', '*', '<', '>', '&', '.', '@', '/', ':', '=', '~', '|', '$', '!', '#', '%',
                    '^', '_', '[', ']', '{', '}', '"', '`', '?'};

string binary_operators[] = {"+", "-", "*", "/", "**", "gr", "ge", "<", "<=", ">", ">=", "ls", "le", "eq",
                             "ne", "&", "or", "><"};

string keys[] = {"let", "fn", "in", "where", "aug", "or", "not", "true", "false", "nil", "dummy", "within",
                 "and", "rec", "gr", "ge", "ls", "le", "eq", "ne"};

class parser
{
public:
    token nextToken;     // Next token to be read
    char readnew[10000]; // Stores program for parsing
    int index;           // Index of character
    int sizeOfFile;      // Size of file
    int astFlag;         // Flag to check if AST or ST is to be printed

    parser(char read_array[], int i, int size, int af)
    {
        copy(read_array, read_array + 10000, readnew);
        index = i;
        sizeOfFile = size;
        astFlag = af;
    }

    bool isReservedKey(string str) // Check if the given string is a reserved keyword
    {
        int i = 0;
        for (i = 0; i < 20; i++)
        {
            if (str == keys[i])
                return true;
        }
        return false;
    }

    bool isOperator(char ch) // Check if the given character is an operator
    {
        for (int i = 0; i < 25; i++)
        {
            if (ch == operators[i])
            {
                return true;
            }
        }
        return false;
    }

    bool isAlpha(char ch) // Checks if the given character is an alphabet letter
    {
        if ((ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122))
        {
            return true;
        }
        return false;
    }

    bool isDigit(char ch) // Checks if the given character is a digit
    {
        if (ch >= 48 && ch <= 57)
        {
            return true;
        }
        return false;
    }

    bool isBinaryOperator(string op) // Checks if the given string is a binary operator
    {
        for (int i = 0; i < 18; i++)
        {
            if (op == binary_operators[i])
            {
                return true;
            }
        }
        return false;
    }

    bool isNumber(const std::string &s) // Checks if the given string is a number
    {
        std::string::const_iterator it = s.begin();
        while (it != s.end() && std::isdigit(*it))
            ++it;
        return !s.empty() && it == s.end();
    }

    void read(string val, string type) // Read the next token
    {
        if (val != nextToken.getValue() || type != nextToken.getType()) // Check if the next token is the expected token
        {
            cout << "Parse error: Expected " << "\"" << val << "\"" << ", but " << "\"" << nextToken.getValue() << "\"" << " was there" << endl;
            exit(0);
        }

        if (type == "ID" || type == "INT" || type == "STR") // If the token is an identifier, integer or string
            buildTree(val, type, 0);

        nextToken = getToken(readnew); // Get the next token

        while (nextToken.getType() == "DELETE") // Ignore all DELETE tokens
            nextToken = getToken(readnew);
    }

    void buildTree(string val, string type, int child) // Build tree for the given string, type and number of children
    {
        if (child == 0) // Leaf node
        {
            tree *temp = createNode(val, type);
            st.push(temp);
        }
        else if (child > 0) // Non-leaf node
        {
            stack<tree *> temp;
            int no_of_pops = child;
            while (!st.empty() && no_of_pops > 0)
            {
                temp.push(st.top());
                st.pop();
                no_of_pops--;
            }
            tree *tempLeft = temp.top();
            temp.pop();
            child--;
            if (!temp.empty() && child > 0)
            {
                tree *rightNode = temp.top();
                tempLeft->right = rightNode;
                temp.pop();
                child--;
                while (!temp.empty() && child > 0)
                {
                    tree *addRight = temp.top();
                    temp.pop();
                    rightNode->right = addRight;
                    rightNode = rightNode->right;
                    child--;
                }
            }
            tree *toPush = createNode(val, type);
            toPush->left = tempLeft;
            st.push(toPush);
        }
    }

    token getToken(char read[]) // Get the next token
    {
        token t;
        int i = index;         // Index of character
        string id = "";        // Identifier
        string num = "";       // Number
        string isop = "";      // Operator
        string isString = "";  // String
        string isPun = "";     // Punctuation
        string isComment = ""; // Comment
        string isSpace = "";   // Space

        // Check if end of file is reached
        if (read[i] == '\0' || i == sizeOfFile)
        {
            t.setType("EOF");
            t.setValue("EOF");
            return t;
        }

        while (i < sizeOfFile || i < 10000 || read[i] != '\0') // Get the next token
        {
            if (isDigit(read[i])) // Check if character is a digit
            {
                while (isDigit(read[i]))
                {
                    num = num + read[i];
                    i++;
                }
                index = i;
                t.setValue(num);
                t.setType("INT");
                return t;
            }

            else if (isAlpha(read[i])) // Check if character is an alphabet letter
            {
                while (isAlpha(read[i]) || isDigit(read[i]) || read[i] == '_')
                {
                    id = id + read[i];
                    i++;
                }

                if (isReservedKey(id))
                {
                    index = i;
                    t.setValue(id);
                    t.setType("KEYWORD");
                    return t;
                }
                else
                {
                    index = i;
                    t.setValue(id);
                    t.setType("ID");
                    return t;
                }
            }
            else if (read[i] == '/' && read[i + 1] == '/') // Check if character is a comment
            {
                while (read[i] == '\'' || read[i] == '\\' || read[i] == '(' || read[i] == ')' || read[i] == ';' || read[i] == ',' || read[i] == ' ' || read[i] == '\t' || isAlpha(read[i]) || isDigit(read[i]) || isOperator(read[i]))
                {

                    if (read[i] == '\n')
                    {
                        i++;
                        break;
                    }
                    else
                    {
                        isComment = isComment + read[i];
                        i++;
                    }
                }

                index = i;
                t.setValue(isComment);
                t.setType("DELETE");
                return t;
            }

            else if (isOperator(read[i])) // Check if character is an operator
            {
                while (isOperator(read[i]))
                {
                    isop = isop + read[i];
                    i++;
                }

                index = i;
                t.setValue(isop);
                t.setType("OPERATOR");
                return t;
            }

            else if (read[i] == '\'')
            {
                isString = isString + read[i];
                i++;
                while (read[i] == '\'' || read[i] == '\\' || read[i] == '(' || read[i] == ')' || read[i] == ';' || read[i] == ',' || read[i] == ' ' || isAlpha(read[i]) || isDigit(read[i]) || read[i] == '_' || isOperator(read[i]))
                {
                    if (read[i] == '\'')
                    {
                        isString = isString + read[i];
                        i++;
                        break;
                    }
                    else if (read[i] == '\\')
                    {
                        isString = isString + read[i];
                        i++;
                        if (read[i] == 't' || read[i] == 'n' || read[i] == '\\' || read[i] == '"')
                        {
                            isString = isString + read[i];
                            i++;
                        }
                        else
                        {
                            i++;
                        }
                    }
                    else
                    {
                        isString = isString + read[i];
                        i++;
                    }
                }
                index = i;
                t.setValue(isString);
                t.setType("STR");
                return t;
            }
            else if (read[i] == ')' || read[i] == '(' || read[i] == ';' || read[i] == ',')
            {
                isPun = isPun + read[i];
                i++;

                index = i;
                t.setValue(isPun);
                t.setType("PUNCTION");
                return t;
            }
            else if (isspace(read[i]))
            {
                while (isspace(read[i]))
                {
                    isSpace = isSpace + read[i];
                    i++;
                }
                index = i;
                t.setValue(isSpace);
                t.setType("DELETE");
                return t;
            }
            else
            {
                string temp = "";
                temp = temp + read[i];
                t.setValue(temp);
                t.setType("UNKNOWN");
                i++;
                index = i;
                return t;
            }
        }

        return t;
    }

    void parse() // Parse the program
    {
        nextToken = getToken(readnew);          // Get the first token
        while (nextToken.getType() == "DELETE") // Ignore all DELETE tokens
            nextToken = getToken(readnew);

        E(); // Start parsing from E

        while (nextToken.getType() == "DELETE") // Ignore all DELETE tokens
            nextToken = getToken(readnew);

        if (index >= sizeOfFile - 1) // Check if end of file is reached
        {
            tree *t = st.top(); // Get the root of the tree

            // Print the abstact syntax tree if the flag is set
            if (astFlag == 1)
                t->print_tree(0);

            // Make the ST
            makeST(t);

            // Print the standardized tree if the flag is set
            if (astFlag == 2)
                t->print_tree(0);

            // Create
            tree *controlStructureArray[200][200];
            createControlStructures(t, controlStructureArray);

            int size = 0;
            int inner = 0;
            while (controlStructureArray[size][0] != NULL)
                size++;

            vector<vector<tree *>> setOfControlStruct;
            for (int i = 0; i < size; i++)
            {
                vector<tree *> temp;
                for (int j = 0; j < 200; j++)
                {
                    if (controlStructureArray[i][j] != NULL)
                        temp.push_back(controlStructureArray[i][j]);
                }
                setOfControlStruct.push_back(temp);
            }

            cse_machine(setOfControlStruct);
        }
    }

    // Helper function for makeStandardTree
    void makeST(tree *t)
    {
        makeStandardTree(t);
    }

    // Makes the tree standard
    tree *makeStandardTree(tree *t)
    {
        if (t == NULL)
            return NULL;
        makeStandardTree(t->left);
        makeStandardTree(t->right);

        if (t->getValue() == "let")
        {
            if (t->left->getValue() == "=")
            {
                t->setValue("gamma");
                t->setType("KEYWORD");
                tree *P = createNode(t->left->right);
                tree *X = createNode(t->left->left);
                tree *E = createNode(t->left->left->right);
                t->left = createNode("lambda", "KEYWORD");
                t->left->right = E;
                tree *lambda = t->left;
                lambda->left = X;
                lambda->left->right = P;
            }
        }
        else if (t->getValue() == "and" && t->left->getValue() == "=")
        {
            tree *equal = t->left;
            t->setValue("=");
            t->setType("KEYWORD");
            t->left = createNode(",", "PUNCTION");
            tree *comma = t->left;
            comma->left = createNode(equal->left);
            t->left->right = createNode("tau", "KEYWORD");
            tree *tau = t->left->right;

            tau->left = createNode(equal->left->right);
            tau = tau->left;
            comma = comma->left;
            equal = equal->right;

            while (equal != NULL)
            {
                comma->right = createNode(equal->left);
                comma = comma->right;
                tau->right = createNode(equal->left->right);
                tau = tau->right;

                equal = equal->right;
            }
        }
        else if (t->getValue() == "where")
        {
            t->setValue("gamma");
            t->setType("KEYWORD");
            if (t->left->right->getValue() == "=")
            {
                tree *P = createNode(t->left);
                tree *X = createNode(t->left->right->left);
                tree *E = createNode(t->left->right->left->right);
                t->left = createNode("lambda", "KEYWORD");
                t->left->right = E;
                t->left->left = X;
                t->left->left->right = P;
            }
        }
        else if (t->getValue() == "within")
        {
            if (t->left->getValue() == "=" && t->left->right->getValue() == "=")
            {
                tree *X1 = createNode(t->left->left);
                tree *E1 = createNode(t->left->left->right);
                tree *X2 = createNode(t->left->right->left);
                tree *E2 = createNode(t->left->right->left->right);
                t->setValue("=");
                t->setType("KEYWORD");
                t->left = X2;
                t->left->right = createNode("gamma", "KEYWORD");
                tree *temp = t->left->right;
                temp->left = createNode("lambda", "KEYWORD");
                temp->left->right = E1;
                temp = temp->left;
                temp->left = X1;
                temp->left->right = E2;
            }
        }
        else if (t->getValue() == "rec" && t->left->getValue() == "=")
        {
            tree *X = createNode(t->left->left);
            tree *E = createNode(t->left->left->right);

            t->setValue("=");
            t->setType("KEYWORD");
            t->left = X;
            t->left->right = createNode("gamma", "KEYWORD");
            t->left->right->left = createNode("YSTAR", "KEYWORD");
            tree *ystar = t->left->right->left;

            ystar->right = createNode("lambda", "KEYWORD");

            ystar->right->left = createNode(X);
            ystar->right->left->right = createNode(E);
        }
        else if (t->getValue() == "function_form")
        {
            tree *P = createNode(t->left);
            tree *V = t->left->right;

            t->setValue("=");
            t->setType("KEYWORD");
            t->left = P;

            tree *temp = t;
            while (V->right->right != NULL)
            {
                temp->left->right = createNode("lambda", "KEYWORD");
                temp = temp->left->right;
                temp->left = createNode(V);
                V = V->right;
            }

            temp->left->right = createNode("lambda", "KEYWORD");
            temp = temp->left->right;

            temp->left = createNode(V);
            temp->left->right = V->right;
        }
        else if (t->getValue() == "lambda")
        {
            if (t->left != NULL)
            {
                tree *V = t->left;
                tree *temp = t;
                if (V->right != NULL && V->right->right != NULL)
                {
                    while (V->right->right != NULL)
                    {
                        temp->left->right = createNode("lambda", "KEYWORD");
                        temp = temp->left->right;
                        temp->left = createNode(V);
                        V = V->right;
                    }

                    temp->left->right = createNode("lambda", "KEYWORD");
                    temp = temp->left->right;
                    temp->left = createNode(V);
                    temp->left->right = V->right;
                }
            }
        }
        else if (t->getValue() == "@")
        {
            tree *E1 = createNode(t->left);
            tree *N = createNode(t->left->right);
            tree *E2 = createNode(t->left->right->right);
            t->setValue("gamma");
            t->setType("KEYWORD");
            t->left = createNode("gamma", "KEYWORD");
            t->left->right = E2;
            t->left->left = N;
            t->left->left->right = E1;
        }

        return NULL;
    }

    /* -------------------------------- CSE Machine -------------------------------- */

    // Creates the necessary control structures for CSE machine
    void createControlStructures(tree *x, tree *(*setOfControlStruct)[200])
    {
        static int index = 1;
        static int j = 0;
        static int i = 0;
        static int betaCount = 1;
        if (x == NULL)
            return;

        if (x->getValue() == "lambda") // If node is lambda
        {
            std::stringstream ss;

            int t1 = i;
            int k = 0;
            setOfControlStruct[i][j] = createNode("", "");
            i = 0;

            while (setOfControlStruct[i][0] != NULL)
            {
                i++;
                k++;
            }
            i = t1;
            ss << k;
            index++;

            string str = ss.str();
            tree *temp = createNode(str, "deltaNumber");

            setOfControlStruct[i][j++] = temp;

            setOfControlStruct[i][j++] = x->left;

            setOfControlStruct[i][j++] = x;

            int myStoredIndex = i;
            int tempj = j + 3;

            while (setOfControlStruct[i][0] != NULL)
                i++;
            j = 0;

            createControlStructures(x->left->right, setOfControlStruct);

            i = myStoredIndex;
            j = tempj;
        }
        else if (x->getValue() == "->") // If node is conditional node
        {
            int myStoredIndex = i;
            int tempj = j;
            int nextDelta = index;
            int k = i;

            std::stringstream ss2;
            ss2 << nextDelta;
            string str2 = ss2.str();
            tree *temp1 = createNode(str2, "deltaNumber");

            setOfControlStruct[i][j++] = temp1;

            int nextToNextDelta = index;
            std::stringstream ss3;
            ss3 << nextToNextDelta;
            string str3 = ss3.str();
            tree *temp2 = createNode(str3, "deltaNumber");
            setOfControlStruct[i][j++] = temp2;

            tree *beta = createNode("beta", "beta");

            setOfControlStruct[i][j++] = beta;

            while (setOfControlStruct[k][0] != NULL)
            {
                k++;
            }
            int firstIndex = k;
            int lamdaCount = index;

            createControlStructures(x->left, setOfControlStruct);
            int diffLc = index - lamdaCount;

            while (setOfControlStruct[i][0] != NULL)
                i++;
            j = 0;

            createControlStructures(x->left->right, setOfControlStruct);

            while (setOfControlStruct[i][0] != NULL)
                i++;
            j = 0;

            createControlStructures(x->left->right->right, setOfControlStruct);

            stringstream ss23;
            if (diffLc == 0 || i < lamdaCount)
            {
                ss23 << firstIndex;
            }
            else
            {
                ss23 << i - 1;
            }

            string str5 = ss23.str();

            setOfControlStruct[myStoredIndex][tempj]->setValue(str5);
            stringstream ss24;
            ss24 << i;
            string str6 = ss24.str();

            setOfControlStruct[myStoredIndex][tempj + 1]->setValue(str6);

            i = myStoredIndex;
            j = 0;

            while (setOfControlStruct[i][j] != NULL)
            {
                j++;
            }
            betaCount += 2;
        }
        else if (x->getValue() == "tau") // If node is tau node
        {
            tree *tauLeft = x->left;
            int numOfChildren = 0;
            while (tauLeft != NULL)
            {
                numOfChildren++;
                tauLeft = tauLeft->right;
            }
            std::stringstream ss;
            ss << numOfChildren;
            string str = ss.str();
            tree *countNode = createNode(str, "CHILDCOUNT");

            setOfControlStruct[i][j++] = countNode; // putting the number of kids of tua
            tree *tauNode = createNode("tau", "tau");

            setOfControlStruct[i][j++] = tauNode; // putting the tau node and not pushing x

            createControlStructures(x->left, setOfControlStruct);
            x = x->left;
            while (x != NULL)
            {
                createControlStructures(x->right, setOfControlStruct);
                x = x->right;
            }
        }
        else
        {
            setOfControlStruct[i][j++] = createNode(x->getValue(), x->getType());
            createControlStructures(x->left, setOfControlStruct);
            if (x->left != NULL)
                createControlStructures(x->left->right, setOfControlStruct);
        }
    }

    // Control Stack Environment Machine
    void cse_machine(vector<vector<tree *>> &controlStructure)
    {
        stack<tree *> control;                   // Stack for control structure
        stack<tree *> m_stack;                   // Stack for operands
        stack<environment *> stackOfEnvironment; // Stack of environments
        stack<environment *> getCurrEnvironment;

        int currEnvIndex = 0;                     // Initial environment
        environment *currEnv = new environment(); // e0
        currEnv->name = "env0";

        currEnvIndex++;
        m_stack.push(createNode(currEnv->name, "ENV"));
        control.push(createNode(currEnv->name, "ENV"));
        stackOfEnvironment.push(currEnv);
        getCurrEnvironment.push(currEnv);

        vector<tree *> tempDelta;
        tempDelta = controlStructure.at(0); // Get the first control structure
        for (int i = 0; i < tempDelta.size(); i++)
        {
            control.push(tempDelta.at(i)); // Push each element of the control structure to the control stack
        }

        while (!control.empty())
        {
            tree *nextToken;
            nextToken = control.top(); // Get the top of the control stack
            control.pop();             // Pop the top of the control stack

            if (nextToken->getValue() == "nil")
            {
                nextToken->setType("tau");
            }

            if (nextToken->getType() == "INT" || nextToken->getType() == "STR" || nextToken->getValue() == "lambda" || nextToken->getValue() == "YSTAR" || nextToken->getValue() == "Print" || nextToken->getValue() == "Isinteger" || nextToken->getValue() == "Istruthvalue" || nextToken->getValue() == "Isstring" || nextToken->getValue() == "Istuple" || nextToken->getValue() == "Isfunction" || nextToken->getValue() == "Isdummy" || nextToken->getValue() == "Stem" || nextToken->getValue() == "Stern" || nextToken->getValue() == "Conc" || nextToken->getType() == "BOOL" || nextToken->getType() == "NIL" || nextToken->getType() == "DUMMY" || nextToken->getValue() == "Order" || nextToken->getValue() == "nil")
            {
                if (nextToken->getValue() == "lambda")
                {
                    tree *boundVar = control.top(); // Variable bouded to lambda
                    control.pop();

                    tree *nextDeltaIndex = control.top(); // Index of next control structure to access
                    control.pop();

                    tree *env = createNode(currEnv->name, "ENV");

                    m_stack.push(nextDeltaIndex); // Index of next control structure to access
                    m_stack.push(boundVar);       // Variable bouded to lambda
                    m_stack.push(env);            // Environment it was created in
                    m_stack.push(nextToken);      // Lambda Token
                }
                else
                {
                    m_stack.push(nextToken); // Push token to the stack
                }
            }
            else if (nextToken->getValue() == "gamma") // If gamma is on top of control stack
            {
                tree *machineTop = m_stack.top();
                if (machineTop->getValue() == "lambda") // CSE Rule 4 (Apply lambda)
                {
                    m_stack.pop(); // Pop lambda token

                    tree *prevEnv = m_stack.top();
                    m_stack.pop(); // Pop the environment in which it was created

                    tree *boundVar = m_stack.top();
                    m_stack.pop(); // Pop variable bounded to lambda

                    tree *nextDeltaIndex = m_stack.top();
                    m_stack.pop(); // Pop index of next control structure to access

                    environment *newEnv = new environment(); // Create new environment

                    std::stringstream ss;
                    ss << currEnvIndex;
                    string str = ss.str(); // Create string of new environment name

                    newEnv->name = "env" + str;

                    // if (currEnvIndex > 10000) // Avoid stack overflow
                    //     return;

                    stack<environment *> tempEnv = stackOfEnvironment;

                    while (tempEnv.top()->name != prevEnv->getValue()) // Get the previous environment node
                        tempEnv.pop();

                    newEnv->prev = tempEnv.top(); // Set the previous environment node

                    // Bounding variables to the environment
                    if (boundVar->getValue() == "," && m_stack.top()->getValue() == "tau") // If Rand is tau
                    {
                        vector<tree *> boundVariables;     // Vector of bound variables
                        tree *leftOfComa = boundVar->left; // Get the left of the comma
                        while (leftOfComa != NULL)
                        {
                            boundVariables.push_back(createNode(leftOfComa));
                            leftOfComa = leftOfComa->right;
                        }

                        vector<tree *> boundValues; // Vector of bound values
                        tree *tau = m_stack.top();  // Pop the tau token
                        m_stack.pop();

                        tree *tauLeft = tau->left; // Get the left of the tau
                        while (tauLeft != NULL)
                        {
                            boundValues.push_back(tauLeft);
                            tauLeft = tauLeft->right; // Get the right of the tau
                        }

                        for (int i = 0; i < boundValues.size(); i++)
                        {
                            if (boundValues.at(i)->getValue() == "tau")
                            {
                                stack<tree *> res;
                                arrangeTuple(boundValues.at(i), res);
                            }

                            vector<tree *> nodeValVector;
                            nodeValVector.push_back(boundValues.at(i));

                            // Insert the bound variable and its value to the environment
                            newEnv->boundVar.insert(std::pair<tree *, vector<tree *>>(boundVariables.at(i), nodeValVector));
                        }
                    }
                    else if (m_stack.top()->getValue() == "lambda") // If Rand is lambda
                    {
                        vector<tree *> nodeValVector;
                        stack<tree *> temp;
                        int j = 0;
                        while (j < 4)
                        {
                            temp.push(m_stack.top());
                            m_stack.pop();
                            j++;
                        }

                        while (!temp.empty())
                        {
                            tree *fromStack;
                            fromStack = temp.top();
                            temp.pop();
                            nodeValVector.push_back(fromStack);
                        }

                        // Insert the bound variable and its value to the environment
                        newEnv->boundVar.insert(std::pair<tree *, vector<tree *>>(boundVar, nodeValVector));
                    }
                    else if (m_stack.top()->getValue() == "Conc") // If Rand is Conc
                    {
                        vector<tree *> nodeValVector;
                        stack<tree *> temp;
                        int j = 0;
                        while (j < 2)
                        {
                            temp.push(m_stack.top());
                            m_stack.pop();
                            j++;
                        }

                        while (!temp.empty())
                        {
                            tree *fromStack;
                            fromStack = temp.top();
                            temp.pop();
                            nodeValVector.push_back(fromStack);
                        }

                        // Insert the bound variable and its value to the environment
                        newEnv->boundVar.insert(std::pair<tree *, vector<tree *>>(boundVar, nodeValVector));
                    }
                    else if (m_stack.top()->getValue() == "eta") // If Rand is eta
                    {
                        vector<tree *> nodeValVector;
                        stack<tree *> temp;
                        int j = 0;
                        while (j < 4)
                        {
                            temp.push(m_stack.top());
                            m_stack.pop();
                            j++;
                        }

                        while (!temp.empty())
                        {
                            tree *fromStack;
                            fromStack = temp.top();
                            temp.pop();
                            nodeValVector.push_back(fromStack);
                        }

                        // Insert the bound variable and its value to the environment
                        newEnv->boundVar.insert(std::pair<tree *, vector<tree *>>(boundVar, nodeValVector));
                    }
                    else // If Rand is an Int
                    {
                        tree *bindVarVal = m_stack.top();
                        m_stack.pop();

                        vector<tree *> nodeValVector;
                        nodeValVector.push_back(bindVarVal);

                        // Insert the bound variable and its value to the environment
                        newEnv->boundVar.insert(std::pair<tree *, vector<tree *>>(boundVar, nodeValVector));
                    }

                    currEnv = newEnv;
                    control.push(createNode(currEnv->name, "ENV"));
                    m_stack.push(createNode(currEnv->name, "ENV"));
                    stackOfEnvironment.push(currEnv);
                    getCurrEnvironment.push(currEnv);

                    istringstream is3(nextDeltaIndex->getValue());
                    int deltaIndex;
                    is3 >> deltaIndex;

                    vector<tree *> nextDelta = controlStructure.at(deltaIndex); // Get the next control structure
                    for (int i = 0; i < nextDelta.size(); i++)
                    {
                        control.push(nextDelta.at(i)); // Push each element of the next control structure to the control stack
                    }
                    currEnvIndex++;
                }
                else if (machineTop->getValue() == "tau") // CSE Rule 10 (Tuple Selection)
                {

                    tree *tau = m_stack.top(); // Get tau node from top of stack
                    m_stack.pop();

                    tree *selectTupleIndex = m_stack.top(); // Get the index of the child to be selected
                    m_stack.pop();

                    istringstream is4(selectTupleIndex->getValue());
                    int tupleIndex;
                    is4 >> tupleIndex;

                    tree *tauLeft = tau->left;
                    tree *selectedChild;
                    while (tupleIndex > 1) // Get the child to be selected
                    {
                        tupleIndex--;
                        tauLeft = tauLeft->right;
                    }

                    selectedChild = createNode(tauLeft);
                    if (selectedChild->getValue() == "lamdaTuple")
                    {
                        tree *getNode = selectedChild->left;
                        while (getNode != NULL)
                        {
                            m_stack.push(createNode(getNode));
                            getNode = getNode->right;
                        }
                    }
                    else
                    {
                        m_stack.push(selectedChild);
                    }
                }
                else if (machineTop->getValue() == "YSTAR") // CSE Rule 12 (Applying YStar)
                {
                    m_stack.pop(); // Pop YSTAR token
                    if (m_stack.top()->getValue() == "lambda")
                    {
                        tree *etaNode = createNode(m_stack.top()->getValue(), m_stack.top()->getType()); // Create eta node
                        etaNode->setValue("eta");
                        m_stack.pop();

                        tree *boundEnv1 = m_stack.top(); // Pop bounded environment
                        m_stack.pop();

                        tree *boundVar1 = m_stack.top(); // Pop bounded variable
                        m_stack.pop();

                        tree *deltaIndex1 = m_stack.top(); // Pop index of next control structure
                        m_stack.pop();

                        // Push the required nodes to the stack
                        m_stack.push(deltaIndex1);
                        m_stack.push(boundVar1);
                        m_stack.push(boundEnv1);
                        m_stack.push(etaNode);
                    }
                    else
                        return; // Error
                }
                else if (machineTop->getValue() == "eta") // CSE Rule 13 (Applying f.p)
                {
                    tree *eta = m_stack.top(); // Pop eta node
                    m_stack.pop();

                    tree *boundEnv1 = m_stack.top(); // Pop bounded environment
                    m_stack.pop();

                    tree *boundVar1 = m_stack.top(); // Pop bounded variable
                    m_stack.pop();

                    tree *deltaIndex1 = m_stack.top(); // Pop index of next control structure
                    m_stack.pop();

                    // Push the eta node back into the stack
                    m_stack.push(deltaIndex1);
                    m_stack.push(boundVar1);
                    m_stack.push(boundEnv1);
                    m_stack.push(eta);

                    // Push a lambda node with same parameters as the eta node
                    m_stack.push(deltaIndex1);
                    m_stack.push(boundVar1);
                    m_stack.push(boundEnv1);
                    m_stack.push(createNode("lambda", "KEYWORD"));

                    // Push two gamma nodes onto control stack
                    control.push(createNode("gamma", "KEYWORD"));
                    control.push(createNode("gamma", "KEYWORD"));
                }
                else if (machineTop->getValue() == "Print") // Print next item on stack
                {
                    m_stack.pop();
                    tree *nextToPrint = m_stack.top(); // Get item to print

                    if (nextToPrint->getValue() == "tau") // If the next item is a tuple
                    {
                        tree *getTau = m_stack.top();

                        stack<tree *> res;
                        arrangeTuple(getTau, res); // Arrange the tuple into a stack

                        stack<tree *> getRev; // Reverse the stack
                        while (!res.empty())
                        {
                            getRev.push(res.top());
                            res.pop();
                        }

                        cout << "("; // Print the tuple
                        while (getRev.size() > 1)
                        {
                            if (getRev.top()->getType() == "STR")
                                cout << addSpaces(getRev.top()->getValue()) << ", ";
                            else
                            {

                                cout << getRev.top()->getValue() << ", ";
                            }
                            getRev.pop();
                        }
                        if (getRev.top()->getType() == "STR")
                            cout << addSpaces(getRev.top()->getValue()) << ")";
                        else
                            cout << getRev.top()->getValue() << ")";
                        getRev.pop();
                    }
                    else if (nextToPrint->getValue() == "lambda") // If the next item is a lambda token
                    {
                        m_stack.pop(); // Pop lambda token

                        tree *env = m_stack.top(); // Get the environment in which it was created
                        m_stack.pop();

                        tree *boundVar = m_stack.top(); // Get the variable bounded to lambda
                        m_stack.pop();

                        tree *num = m_stack.top(); // Get the index of next control structure to access
                        m_stack.pop();

                        cout << "[lambda closure: " << boundVar->getValue() << ": " << num->getValue() << "]";
                        return;
                    }
                    else // If the next item is a string or integer
                    {
                        if (m_stack.top()->getType() == "STR")
                            cout << addSpaces(m_stack.top()->getValue());
                        else
                            cout << m_stack.top()->getValue();
                    }
                }
                else if (machineTop->getValue() == "Isinteger") // Check if next item in stack is an integer
                {
                    m_stack.pop(); // Pop Isinteger token

                    tree *isNextInt = m_stack.top(); // Get next item in stack
                    m_stack.pop();

                    if (isNextInt->getType() == "INT")
                    {
                        tree *resNode = createNode("true", "boolean");
                        m_stack.push(resNode);
                    }
                    else
                    {
                        tree *resNode = createNode("false", "boolean");
                        m_stack.push(resNode);
                    }
                }
                else if (machineTop->getValue() == "Istruthvalue") // Check if next item in stack is a boolean value
                {
                    m_stack.pop(); // Pop Istruthvalue token

                    tree *isNextBool = m_stack.top(); // Get next item in stack
                    m_stack.pop();

                    if (isNextBool->getValue() == "true" || isNextBool->getValue() == "false")
                    {
                        tree *resNode = createNode("true", "BOOL");
                        m_stack.push(resNode);
                    }
                    else
                    {
                        tree *resNode = createNode("false", "BOOL");
                        m_stack.push(resNode);
                    }
                }
                else if (machineTop->getValue() == "Isstring") // Check if next item in stack is a string
                {
                    m_stack.pop(); // Pop Isstring token

                    tree *isNextString = m_stack.top(); // Get next item in stack
                    m_stack.pop();

                    if (isNextString->getType() == "STR")
                    {
                        tree *resNode = createNode("true", "BOOL");
                        m_stack.push(resNode);
                    }
                    else
                    {
                        tree *resNode = createNode("false", "BOOL");
                        m_stack.push(resNode);
                    }
                }
                else if (machineTop->getValue() == "Istuple") // Check if next item in stack is a tuple
                {
                    m_stack.pop(); // Pop Istuple token

                    tree *isNextTau = m_stack.top(); // Get next item in stack
                    m_stack.pop();

                    if (isNextTau->getType() == "tau")
                    {
                        tree *resNode = createNode("true", "BOOL");
                        m_stack.push(resNode);
                    }
                    else
                    {
                        tree *resNode = createNode("false", "BOOL");
                        m_stack.push(resNode);
                    }
                }
                else if (machineTop->getValue() == "Isfunction") // Check if next item in stack is a function
                {
                    m_stack.pop(); // Pop Isfunction token

                    tree *isNextFn = m_stack.top(); // Get next item in stack

                    if (isNextFn->getValue() == "lambda")
                    {
                        tree *resNode = createNode("true", "BOOL");
                        m_stack.push(resNode);
                    }
                    else
                    {
                        tree *resNode = createNode("false", "BOOL");
                        m_stack.push(resNode);
                    }
                }
                else if (machineTop->getValue() == "Isdummy") // Check if next item in stack is dummy
                {
                    m_stack.pop(); // Pop Isdummy token

                    tree *isNextDmy = m_stack.top(); // Get next item in stack

                    if (isNextDmy->getValue() == "dummy")
                    {
                        tree *resNode = createNode("true", "BOOL");
                        m_stack.push(resNode);
                    }
                    else
                    {
                        tree *resNode = createNode("false", "BOOL");
                        m_stack.push(resNode);
                    }
                }
                else if (machineTop->getValue() == "Stem") // Get first character of string
                {
                    m_stack.pop();                      // Pop Stem token
                    tree *isNextString = m_stack.top(); // Get next item in stack

                    if (isNextString->getValue() == "")
                        return;

                    if (isNextString->getType() == "STR")
                    {
                        string strRes = "'" + isNextString->getValue().substr(1, 1) + "'"; // Get first character
                        m_stack.pop();
                        m_stack.push(createNode(strRes, "STR"));
                    }
                }
                else if (machineTop->getValue() == "Stern") // Get remaining characters other the first character
                {
                    m_stack.pop();                      // Pop Stern token
                    tree *isNextString = m_stack.top(); // Get next item in stack

                    if (isNextString->getValue() == "")
                        return;

                    if (isNextString->getType() == "STR")
                    {
                        string strRes = "'" + isNextString->getValue().substr(2, isNextString->getValue().length() - 3) + "'"; // Get remaining characters
                        m_stack.pop();
                        m_stack.push(createNode(strRes, "STR"));
                    }
                }
                else if (machineTop->getValue() == "Order") // Get number of items in tuple
                {
                    m_stack.pop(); // Pop Order token

                    int numOfItems = 0;
                    tree *getTau = m_stack.top(); // Get next item in stack

                    if (getTau->left != NULL)
                        getTau = getTau->left;

                    while (getTau != NULL)
                    {
                        numOfItems++; // Count number of items
                        getTau = getTau->right;
                    }

                    getTau = m_stack.top();
                    m_stack.pop();

                    if ((getTau->getValue() == "nil"))
                        numOfItems = 0;

                    stringstream ss11;
                    ss11 << numOfItems;
                    string str34 = ss11.str();
                    tree *orderNode = createNode(str34, "INT");
                    m_stack.push(orderNode);
                }
                else if (machineTop->getValue() == "Conc") // Concatenate two strings
                {
                    tree *concNode = m_stack.top(); // Pop Conc token
                    m_stack.pop();

                    tree *firstString = m_stack.top(); // Get first string
                    m_stack.pop();

                    tree *secondString = m_stack.top(); // Get second string

                    if (secondString->getType() == "STR" || (secondString->getType() == "STR" && secondString->left != NULL && secondString->left->getValue() == "true"))
                    {
                        m_stack.pop();
                        string res = "'" + firstString->getValue().substr(1, firstString->getValue().length() - 2) + secondString->getValue().substr(1, secondString->getValue().length() - 2) + "'";
                        tree *resNode = createNode(res, "STR");
                        m_stack.push(resNode);
                        control.pop();
                    }
                    else
                    {
                        concNode->left = firstString;
                        m_stack.push(concNode);
                        firstString->left = createNode("true", "flag");
                    }
                }
            }
            else if (nextToken->getValue().substr(0, 3) == "env") // If env token is on top of control stack (CSE Rule 5)
            {
                stack<tree *> removeFromMachineToPutBack;
                if (m_stack.top()->getValue() == "lambda") // Pop lambda token and its parameters
                {
                    removeFromMachineToPutBack.push(m_stack.top());
                    m_stack.pop();
                    removeFromMachineToPutBack.push(m_stack.top());
                    m_stack.pop();
                    removeFromMachineToPutBack.push(m_stack.top());
                    m_stack.pop();
                    removeFromMachineToPutBack.push(m_stack.top());
                    m_stack.pop();
                }
                else
                {
                    removeFromMachineToPutBack.push(m_stack.top()); // Pop value from stack
                    m_stack.pop();
                }

                tree *remEnv = m_stack.top(); // Get the environment to remove

                if (nextToken->getValue() == remEnv->getValue()) // If the environment to remove is the same as the one on top of the control stack
                {
                    m_stack.pop();

                    stack<tree *> printMachine = m_stack;

                    getCurrEnvironment.pop();
                    if (!getCurrEnvironment.empty())
                    {
                        currEnv = getCurrEnvironment.top();
                    }
                    else
                        currEnv = NULL;
                }
                else
                    return;

                while (!removeFromMachineToPutBack.empty()) // Push the popped values back to the stack
                {
                    m_stack.push(removeFromMachineToPutBack.top());
                    removeFromMachineToPutBack.pop();
                }
            }
            // If any variables are on top of the control stack
            else if (nextToken->getType() == "ID" && nextToken->getValue() != "Print" && nextToken->getValue() != "Isinteger" && nextToken->getValue() != "Istruthvalue" && nextToken->getValue() != "Isstring" && nextToken->getValue() != "Istuple" && nextToken->getValue() != "Isfunction" && nextToken->getValue() != "Isdummy" && nextToken->getValue() != "Stem" && nextToken->getValue() != "Stern" && nextToken->getValue() != "Conc")
            {
                environment *temp = currEnv;
                int flag = 0;
                while (temp != NULL)
                {
                    map<tree *, vector<tree *>>::iterator itr = temp->boundVar.begin();
                    while (itr != temp->boundVar.end())
                    {
                        if (nextToken->getValue() == itr->first->getValue())
                        {
                            vector<tree *> temp = itr->second;
                            if (temp.size() == 1 && temp.at(0)->getValue() == "Conc" && temp.at(0)->left != NULL)
                            {
                                control.push(createNode("gamma", "KEYWORD"));
                                m_stack.push(temp.at(0)->left);
                                m_stack.push(temp.at(0));
                            }
                            else
                            {
                                int i = 0;
                                while (i < temp.size())
                                {
                                    if (temp.at(i)->getValue() == "lamdaTuple")
                                    {
                                        tree *myLambda = temp.at(i)->left;
                                        while (myLambda != NULL)
                                        {
                                            m_stack.push(createNode(myLambda));
                                            myLambda = myLambda->right;
                                        }
                                    }
                                    else
                                    {
                                        if (temp.at(i)->getValue() == "tau")
                                        {
                                            stack<tree *> res;
                                            arrangeTuple(temp.at(i), res);
                                        }
                                        m_stack.push(temp.at(i));
                                    }
                                    i++;
                                }
                            }
                            flag = 1;
                            break;
                        }
                        itr++;
                    }
                    if (flag == 1)
                        break;
                    temp = temp->prev;
                }
                if (flag == 0)
                    return;
            }
            // If a binary or unary operator is on top of the control stack (CSE Rule 6 and CSE Rule 7)
            else if (isBinaryOperator(nextToken->getValue()) || nextToken->getValue() == "neg" || nextToken->getValue() == "not")
            {
                string op = nextToken->getValue();           // Get the operator
                if (isBinaryOperator(nextToken->getValue())) // If token is a binary operator
                {
                    tree *node1 = m_stack.top(); // Get the first operand
                    m_stack.pop();

                    tree *node2 = m_stack.top(); // Get the second operand
                    m_stack.pop();

                    if (node1->getType() == "INT" && node2->getType() == "INT")
                    {
                        int num1;
                        int num2;

                        istringstream is1(node1->getValue());
                        is1 >> num1;
                        istringstream is2(node2->getValue());
                        is2 >> num2;

                        int res = 0;
                        double resPow;

                        // Perform the operation and create a node with the result
                        if (op == "+") // Addition
                        {
                            res = num1 + num2;
                            stringstream ss;
                            ss << res;
                            string str = ss.str();
                            tree *res = createNode(str, "INT");
                            m_stack.push(res);
                        }
                        else if (op == "-") // Subtraction
                        {
                            res = num1 - num2;
                            stringstream ss;
                            ss << res;
                            string str = ss.str();
                            tree *res = createNode(str, "INT");
                            m_stack.push(res);
                        }
                        else if (op == "*") // Multiplication
                        {
                            res = num1 * num2;
                            stringstream ss;
                            ss << res;
                            string str = ss.str();
                            tree *res = createNode(str, "INT");
                            m_stack.push(res);
                        }
                        else if (op == "/") // Division
                        {
                            if (num2 == 0) // If division by zero
                                cout << "Exception: STATUS_INTEGER_DIVIDE_BY_ZERO" << endl;
                            res = num1 / num2;
                            stringstream ss;
                            ss << res;
                            string str = ss.str();
                            tree *res = createNode(str, "INT");
                            m_stack.push(res);
                        }
                        else if (op == "**") // Power
                        {
                            resPow = pow((double)num1, (double)num2);
                            stringstream ss;
                            ss << res;
                            string str = ss.str();
                            tree *res = createNode(str, "INT");
                            m_stack.push(res);
                        }
                        else if (op == "gr" || op == ">") // Greater than
                        {
                            string resStr = num1 > num2 ? "true" : "false";
                            tree *res = createNode(resStr, "bool");
                            m_stack.push(res);
                        }
                        else if (op == "ge" || op == ">=") // Greater than or equal to
                        {
                            string resStr = num1 >= num2 ? "true" : "false";
                            tree *res = createNode(resStr, "bool");
                            m_stack.push(res);
                        }
                        else if (op == "ls" || op == "<") // Less than
                        {
                            string resStr = num1 < num2 ? "true" : "false";
                            tree *res = createNode(resStr, "bool");
                            m_stack.push(res);
                        }
                        else if (op == "le" || op == "<=") // Less than or equal to
                        {
                            string resStr = num1 <= num2 ? "true" : "false";
                            tree *res = createNode(resStr, "bool");
                            m_stack.push(res);
                        }
                        else if (op == "eq" || op == "=") // Equal
                        {
                            string resStr = num1 == num2 ? "true" : "false";
                            tree *res = createNode(resStr, "bool");
                            m_stack.push(res);
                        }
                        else if (op == "ne" || op == "><") // Not equal
                        {
                            string resStr = num1 != num2 ? "true" : "false";
                            tree *res = createNode(resStr, "bool");
                            m_stack.push(res);
                        }
                    }
                    else if (node1->getType() == "STR" && node2->getType() == "STR") // If both operands are strings
                    {
                        if (op == "ne" || op == "<>")
                        {
                            string resStr = node1->getValue() != node2->getValue() ? "true" : "false";
                            tree *res = createNode(resStr, "BOOL");
                            m_stack.push(res);
                        }
                        else if (op == "eq" || op == "==")
                        {
                            string resStr = node1->getValue() == node2->getValue() ? "true" : "false";
                            tree *res = createNode(resStr, "BOOL");
                            m_stack.push(res);
                        }
                    }
                    // If both operands are boolean values
                    else if ((node1->getValue() == "true" || node1->getValue() == "false") && (node2->getValue() == "false" || node2->getValue() == "true"))
                    {
                        if (op == "ne" || op == "<>")
                        {
                            string resStr = node1->getValue() != node2->getValue() ? "true" : "false";
                            tree *res = createNode(resStr, "BOOL");
                            m_stack.push(res);
                        }
                        else if (op == "eq" || op == "==")
                        {
                            string resStr = node1->getValue() == node2->getValue() ? "true" : "false";
                            tree *res = createNode(resStr, "BOOL");
                            m_stack.push(res);
                        }
                        else if (op == "or")
                        {
                            string resStr;
                            if (node1->getValue() == "true" || node2->getValue() == "true")
                            {
                                resStr = "true";
                                tree *res = createNode(resStr, "BOOL");
                                m_stack.push(res);
                            }
                            else
                            {
                                resStr = "false";
                                tree *res = createNode(resStr, "BOOL");
                                m_stack.push(res);
                            }
                        }
                        else if (op == "&")
                        {
                            string resStr;
                            if (node1->getValue() == "true" && node2->getValue() == "true")
                            {
                                resStr = "true";
                                tree *res = createNode(resStr, "BOOL");
                                m_stack.push(res);
                            }
                            else
                            {
                                resStr = "false";
                                tree *res = createNode(resStr, "BOOL");
                                m_stack.push(res);
                            }
                        }
                    }
                }
                // If unary operator
                else if (op == "neg" || op == "not")
                {
                    if (op == "neg") // If negation
                    {
                        tree *node1 = m_stack.top();
                        m_stack.pop();
                        istringstream is1(node1->getValue());
                        int num1;
                        is1 >> num1;
                        int res = -num1;
                        stringstream ss;
                        ss << res;
                        string str = ss.str();
                        tree *resStr = createNode(str, "INT");
                        m_stack.push(resStr);
                    }
                    else if (op == "not" && (m_stack.top()->getValue() == "true" || m_stack.top()->getValue() == "false")) // If not
                    {
                        if (m_stack.top()->getValue() == "true")
                        {
                            m_stack.pop();
                            m_stack.push(createNode("false", "BOOL"));
                        }
                        else
                        {
                            m_stack.pop();
                            m_stack.push(createNode("true", "BOOL"));
                        }
                    }
                }
            }
            // If beta is on top of control stack (CSE Rule 8)
            else if (nextToken->getValue() == "beta")
            {
                tree *boolVal = m_stack.top(); // Get the boolean value from stack
                m_stack.pop();

                tree *elseIndex = control.top(); // Get the index of the else statement
                control.pop();

                tree *thenIndex = control.top(); // Get the index of the then statement
                control.pop();

                int index;
                if (boolVal->getValue() == "true") // If the boolean value is true, then go to the then statement
                {
                    istringstream is1(thenIndex->getValue());
                    is1 >> index;
                }
                else // If the boolean value is false, then go to the else statement
                {
                    istringstream is1(elseIndex->getValue());
                    is1 >> index;
                }

                vector<tree *> nextDelta = controlStructure.at(index); // Get the next control structure from index
                for (int i = 0; i < nextDelta.size(); i++)             // Push each element of the next control structure to the control stack
                    control.push(nextDelta.at(i));
            }
            // If tau is on top of control stack (CSE Rule 9)
            else if (nextToken->getValue() == "tau")
            {
                tree *tupleNode = createNode("tau", "tau");

                tree *noOfItems = control.top(); // Get the number of items in the tuple
                control.pop();

                int numOfItems;

                istringstream is1(noOfItems->getValue());
                is1 >> numOfItems; // Convert the number of items to an integer

                if (m_stack.top()->getValue() == "lambda") // If the first item is a lambda token
                {
                    tree *lamda = createNode(m_stack.top()->getValue(), m_stack.top()->getType()); // Pop lambda
                    m_stack.pop();

                    tree *prevEnv = createNode(m_stack.top()->getValue(), m_stack.top()->getType()); // Pop the environment in which it was created
                    m_stack.pop();                                                                   // popped the evn in which it was created

                    tree *boundVar = createNode(m_stack.top()->getValue(), m_stack.top()->getType()); // Pop the variable bounded to lambda
                    m_stack.pop();                                                                    // bound variable of lambda

                    tree *nextDeltaIndex = createNode(m_stack.top()->getValue(), m_stack.top()->getType()); // Pop the index of next control structure
                    m_stack.pop();                                                                          // next delta to be opened

                    // Create a lambda tuple and set parameters
                    tree *myLambda = createNode("lamdaTuple", "lamdaTuple");
                    myLambda->left = nextDeltaIndex;
                    nextDeltaIndex->right = boundVar;
                    boundVar->right = prevEnv;
                    prevEnv->right = lamda;
                    tupleNode->left = myLambda;
                }
                else
                {
                    tupleNode->left = createNode(m_stack.top());
                    m_stack.pop();
                }

                tree *sibling = tupleNode->left; // Get the first item in the tuple

                for (int i = 1; i < numOfItems; i++) // For each item in the tuple
                {
                    tree *temp = m_stack.top();       // Get the next item in the stack
                    if (temp->getValue() == "lambda") // If the next item is a lambda token
                    {
                        tree *lamda = createNode(m_stack.top()->getValue(), m_stack.top()->getType()); // Pop lambda
                        m_stack.pop();

                        tree *prevEnv = createNode(m_stack.top()->getValue(), m_stack.top()->getType()); // Pop the environment in which it was created
                        m_stack.pop();

                        tree *boundVar = createNode(m_stack.top()->getValue(), m_stack.top()->getType()); // Pop the variable bounded to lambda
                        m_stack.pop();

                        tree *nextDeltaIndex = createNode(m_stack.top()->getValue(), m_stack.top()->getType()); // Pop the index of next control structure
                        m_stack.pop();

                        tree *myLambda = createNode("lamdaTuple", "lamdaTuple");

                        myLambda->left = nextDeltaIndex;
                        nextDeltaIndex->right = boundVar;
                        boundVar->right = prevEnv;
                        prevEnv->right = lamda;
                        sibling->right = myLambda;
                        sibling = sibling->right;
                    }
                    else // If the next item is not a lambda token
                    {
                        m_stack.pop();
                        sibling->right = temp;
                        sibling = sibling->right;
                    }
                }
                m_stack.push(tupleNode); // Push the tuple to the stack
            }
            // If aug is on top of control stack
            else if (nextToken->getValue() == "aug")
            {
                tree *token1 = createNode(m_stack.top()); // Get the first item in the stack
                m_stack.pop();

                tree *token2 = createNode(m_stack.top()); // Get the second item in the stack
                m_stack.pop();

                if (token1->getValue() == "nil" && token2->getValue() == "nil") // If both tokens are nil
                {
                    tree *tupleNode = createNode("tau", "tau");
                    tupleNode->left = token1;
                    m_stack.push(tupleNode);
                }
                else if (token1->getValue() == "nil") // If the first token is nil
                {
                    tree *tupleNode = createNode("tau", "tau");
                    tupleNode->left = token2;
                    m_stack.push(tupleNode);
                }
                else if (token2->getValue() == "nil") // If the second token is nil
                {
                    tree *tupleNode = createNode("tau", "tau");
                    tupleNode->left = token1;
                    m_stack.push(tupleNode);
                }
                else if (token1->getType() != "tau") // If the first token is not a tuple
                {
                    tree *tupleNode = token2->left;
                    while (tupleNode->right != NULL)
                    {
                        tupleNode = tupleNode->right;
                    }
                    tupleNode->right = createNode(token1);
                    m_stack.push(token2);
                }
                else if (token2->getType() != "tau") // If the second token is not a tuple
                {
                    tree *tupleNode = token1->left;
                    while (tupleNode->right != NULL)
                    {
                        tupleNode = tupleNode->right;
                    }
                    tupleNode->right = createNode(token2);
                    m_stack.push(token1);
                }
                else // If both tokens are tuples
                {
                    tree *tupleNode = createNode("tau", "tau");
                    tupleNode->left = token1;
                    tupleNode->left->right = token2;
                    m_stack.push(tupleNode);
                }
            }
        }

        cout << endl;
    }

    /* -------------------------------- Additional Functions to print output-------------------------------- */

    // Arranges nodes in tauNode into a stack
    void arrangeTuple(tree *tauNode, stack<tree *> &res)
    {
        if (tauNode == NULL)
            return;
        if (tauNode->getValue() == "lamdaTuple")
            return;

        if (tauNode->getValue() != "tau" && tauNode->getValue() != "nil")

        {
            res.push(tauNode);
        }

        arrangeTuple(tauNode->left, res);
        arrangeTuple(tauNode->right, res);
    }

    // Adds spaces to the string
    string addSpaces(string temp_str)
    {
        for (int i = 1; i < temp_str.length(); i++)
        {
            if (temp_str[i - 1] == 92 && (temp_str[i] == 'n'))
            {
                temp_str.replace(i - 1, 2, "\\\n");
            }
            if (temp_str[i - 1] == 92 && (temp_str[i] == 't'))
            {
                temp_str.replace(i - 1, 2, "\\\t");
            }
        }

        for (int i = 0; i < temp_str.length(); i++)
        {
            if (temp_str[i] == 92 || temp_str[i] == 39)
            {
                temp_str.replace(i, 1, "");
            }
        }
        return temp_str;
    }

    /* --------------------------------Implementation of the Grammar Rules of RPAL-------------------------------- */

    void E() // E -> ’let’ D ’in’ E | ’fn’ Vb+ ’.’ E | Ew
    {
        if (nextToken.getValue() == "let")
        {
            read("let", "KEYWORD");
            D();

            read("in", "KEYWORD"); // read in
            E();

            buildTree("let", "KEYWORD", 2);
        }
        else if (nextToken.getValue() == "fn")
        {
            int n = 0;
            read("fn", "KEYWORD");
            do
            {
                Vb();
                n++;
            } while (nextToken.getType() == "ID" || nextToken.getValue() == "(");
            read(".", "OPERATOR");
            E();

            buildTree("lambda", "KEYWORD", n + 1);
        }
        else
        {
            Ew();
        }
    }

    void Ew() // Ew ->  T ’where’ Dr | T
    {
        T();

        if (nextToken.getValue() == "where") // If next token is 'where', then rule is, Ew ->  T ’where’ Dr | T
        {
            read("where", "KEYWORD");
            Dr();
            buildTree("where", "KEYWORD", 2);
        }
    }

    void T() // T -> Ta ( ’,’ Ta )+ | Ta
    {
        Ta();

        int n = 1;
        while (nextToken.getValue() == ",") // Iterate and read ","; Rule is T -> Ta ( ’,’ Ta )+
        {
            n++;
            read(",", "PUNCTION");
            Ta();
        }
        if (n > 1)
        {
            buildTree("tau", "KEYWORD", n);
        }
    }

    void Ta() // Ta -> Ta ’aug’ Tc | Tc
    {
        Tc();

        while (nextToken.getValue() == "aug") // If next token is 'aug', then rule is, Ta -> Ta ’aug’ Tc
        {
            read("aug", "KEYWORD");
            Tc();
            buildTree("aug", "KEYWORD", 2);
        }
    }

    void Tc() // Tc -> B ’->’ Tc ’|’ Tc | B
    {
        B();

        if (nextToken.getValue() == "->") // If next token is '->', then rule is, Tc -> B ’->’ Tc ’|’ Tc
        {
            read("->", "OPERATOR");
            Tc();
            read("|", "OPERATOR");
            Tc();
            buildTree("->", "KEYWORD", 3);
        }
    }

    void B() // B -> B ’or’ Bt | Bt
    {
        Bt();
        while (nextToken.getValue() == "or") // If next token is 'or', then rule is, B -> B ’or’ Bt
        {
            read("or", "KEYWORD");
            Bt();
            buildTree("or", "KEYWORD", 2);
        }
    }

    void Bt() // Bt -> Bt ’&’ Bs | Bs
    {
        Bs();
        while (nextToken.getValue() == "&") // If next token is '&', then rule is, Bt -> Bt ’&’ Bs
        {
            read("&", "OPERATOR");
            Bs();
            buildTree("&", "KEYWORD", 2);
        }
    }

    void Bs() // Bs -> ’not’ Bp | Bp
    {
        if (nextToken.getValue() == "not") // If next token is 'not', then rule is, Bs -> ’not’ Bp
        {
            read("not", "KEYWORD");
            Bp();
            buildTree("not", "KEYWORD", 1);
        }
        else // Else the rule is, Bs -> Bp
        {
            Bp();
        }
    }

    void Bp()
    {
        A();
        string temp = nextToken.getValue();
        string temp2 = nextToken.getType();

        if (nextToken.getValue() == "gr" || nextToken.getValue() == ">") // Bp -> A (’gr’ | ’>’ )
        {
            read(temp, temp2);
            A();
            buildTree("gr", "KEYWORD", 2);
        }
        else if (nextToken.getValue() == "ge" || nextToken.getValue() == ">=") // Bp -> A (’ge’ | ’>=’)
        {
            read(temp, temp2);
            A();
            buildTree("ge", "KEYWORD", 2);
        }
        else if (nextToken.getValue() == "ls" || nextToken.getValue() == "<") // Bp -> A (’ls’ | ’<’ )
        {
            read(temp, temp2);
            A();
            buildTree("ls", "KEYWORD", 2);
        }
        else if (nextToken.getValue() == "le" || nextToken.getValue() == "<=") // Bp -> A (’le’ | ’<=’)
        {
            read(temp, temp2);
            A();
            buildTree("le", "KEYWORD", 2);
        }
        else if (nextToken.getValue() == "eq") // Bp -> A ’eq’ A
        {
            read(temp, temp2);
            A();
            buildTree("eq", "KEYWORD", 2);
        }
        else if (nextToken.getValue() == "ne") // Bp -> A ’ne’ A
        {
            read(temp, temp2);
            A();
            buildTree("ne", "KEYWORD", 2);
        }
    }

    void A() // A -> A ’+’ At | A ’-’ At | ’+’ At | ’-’ At | At
    {
        if (nextToken.getValue() == "+") // A -> ’+’ At
        {
            read("+", "OPERATOR");
            At();
        }
        else if (nextToken.getValue() == "-") // A -> ’-’ At
        {
            read("-", "OPERATOR");
            At();
            buildTree("neg", "KEYWORD", 1);
        }
        else
        {
            At();
        }

        // A   -> A ’+’ At | A ’-’ At
        while (nextToken.getValue() == "+" || nextToken.getValue() == "-")
        {
            string temp = nextToken.getValue();
            read(temp, "OPERATOR");
            At();
            buildTree(temp, "OPERATOR", 2);
        }
    }

    void At() // At -> At ’*’ Af | At ’/’  Af | Af
    {
        Af(); // At -> Af

        while (nextToken.getValue() == "*" || nextToken.getValue() == "/") // At -> At ’*’ Af | At ’/’  Af
        {
            string temp = nextToken.getValue();
            read(temp, "OPERATOR");
            Af();
            buildTree(temp, "OPERATOR", 2);
        }
    }

    void Af() // Af -> Ap ’**’ Af | Ap
    {
        Ap();

        if (nextToken.getValue() == "**") // Af -> Ap ’**’ Af
        {
            read("**", "OPERATOR");
            Af();
            buildTree("**", "KEYWORD", 2);
        }
    }

    void Ap() // Ap -> Ap ’@’ ’<IDENTIFIERENTIFIER>’ R | R
    {
        R();
        while (nextToken.getValue() == "@")
        {
            read("@", "OPERATOR"); // read a type ID
            if (nextToken.getType() != "ID")
            {
                cout << "Exception: UNEXPECTED_TOKEN";
            }
            else
            {
                read(nextToken.getValue(), "ID");
                R();
                buildTree("@", "KEYWORD", 3);
            }
        }
    }

    void R() // R -> R Rn | Rn
    {
        Rn();
        while (nextToken.getType() == "ID" || nextToken.getType() == "INT" || nextToken.getType() == "STR" || nextToken.getValue() == "true" || nextToken.getValue() == "false" || nextToken.getValue() == "nil" || nextToken.getValue() == "(" || nextToken.getValue() == "dummy")
        {
            Rn();
            buildTree("gamma", "KEYWORD", 2);
        }
    }

    void Rn() // Rn -> ’<ID>’ | ’<INT>’ | ’<STR>’ | ’true’ | ’false’ | ’nil’ | ’(’ E ’)’ | ’dummy’
    {
        if (nextToken.getType() == "ID" || nextToken.getType() == "INT" || nextToken.getType() == "STR") // Rn -> ’<ID>’ | ’<INT>’ | ’<STR>’
        {
            read(nextToken.getValue(), nextToken.getType());
        }
        else if (nextToken.getValue() == "true") // Rn -> ’true’
        {
            read("true", "KEYWORD");
            buildTree("true", "BOOL", 0);
        }
        else if (nextToken.getValue() == "false") // Rn -> ’false’
        {
            read("false", "KEYWORD");
            buildTree("false", "BOOL", 0);
        }
        else if (nextToken.getValue() == "nil") // Rn -> ’nil’
        {
            read("nil", "KEYWORD");
            buildTree("nil", "NIL", 0);
        }
        else if (nextToken.getValue() == "(") // Rn -> ’(’ E ’)’
        {
            read("(", "PUNCTION");
            E();
            read(")", "PUNCTION");
        }
        else if (nextToken.getValue() == "dummy") // Rn -> ’dummy’
        {
            read("dummy", "KEYWORD");
            buildTree("dummy", "DUMMY", 0);
        }
    }

    void D() // D -> Da ’within’ D | Da
    {
        Da();
        if (nextToken.getValue() == "within")
        {
            read("within", "KEYWORD");
            Da();
            buildTree("within", "KEYWORD", 2);
        }
    }

    void Da() // Da -> Dr ( ’and’ Dr )+ | Dr
    {
        Dr();

        int n = 1;
        while (nextToken.getValue() == "and")
        {
            n++;
            read("and", "KEYWORD");
            Dr();
        }
        if (n > 1)
        {
            buildTree("and", "KEYWORD", n);
        }
    }

    void Dr() // Dr -> ’rec’ Db | Db
    {
        if (nextToken.getValue() == "rec")
        {
            read("rec", "KEYWORD");
            Db();
            buildTree("rec", "KEYWORD", 1);
        }
        else
        {
            Db();
        }
    }

    void Db() // Db -> Vl ’=’ E | ’<ID>’ Vb+ ’=’ E | ’(’ D ’)’
    {
        if (nextToken.getValue() == "(")
        {
            read("(", "PUNCTION");
            D();
            read(")", "PUNCTION");
        }
        else if (nextToken.getType() == "ID")
        {
            read(nextToken.getValue(), "ID");
            int n = 1;
            if (nextToken.getValue() == "=" || nextToken.getValue() == ",")
            {
                while (nextToken.getValue() == ",")
                {
                    read(",", "PUNCTION");
                    read(nextToken.getValue(), "ID");
                    n++;
                }
                if (n > 1)
                {
                    buildTree(",", "KEYWORD", n);
                }
                read("=", "OPERATOR");
                E();
                buildTree("=", "KEYWORD", 2);
            }
            else
            {
                do
                {
                    Vb();
                    n++;
                } while (nextToken.getType() == "ID" || nextToken.getValue() == "(");
                read("=", "OPERATOR");
                E();
                buildTree("function_form", "KEYWORD", n + 1);
            }
        }
    }

    void Vb() // Vb -> ’<ID>’ | ’(’ Vl ’)’ | ’(’ ’)’
    {
        if (nextToken.getType() == "ID")
        {
            read(nextToken.getValue(), "ID");
        }
        else if (nextToken.getValue() == "(")
        {
            read("(", "PUNCTION");
            if (nextToken.getValue() == ")")
            {
                read(")", "PUNCTION");
                buildTree("()", "KEYWORD", 0);
            }
            else
            {
                Vl();
                read(")", "PUNCTION");
            }
        }
    }

    void Vl() // Vl -> ’<ID>’ list ’,’
    {
        int n = 1;
        read(nextToken.getValue(), "ID");

        while (nextToken.getValue() == ",")
        {
            read(",", "PUNCTION");
            read(nextToken.getValue(), "ID");
            n++;
        }
        if (n > 1)
        {
            buildTree(",", "KEYWORD", n);
        }
    }
};
