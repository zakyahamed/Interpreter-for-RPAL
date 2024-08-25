#ifndef TREE_H_
#define TREE_H_
#include <iostream>
#include <stack>

using namespace std;

class tree // Tree class to store the AST nodes
{
private:
	string val;	 // Value of node
	string type; // Type of node

public:
	tree *left;
	tree *right;
	void setType(string typ);
	void setValue(string value);
	string getType();
	string getValue();
	tree *createNode(string value, string typ);
	tree *createNode(tree *x);
	void print_tree(int dots_count);
};

void tree::setType(string typ)
{
	type = typ;
}

void tree::setValue(string value)
{
	val = value;
}

string tree::getType()
{
	return type;
}

string tree::getValue()
{
	return val;
}

tree *createNode(string value, string typ)
{
	tree *t = new tree();
	t->setValue(value);
	t->setType(typ);
	t->left = NULL;
	t->right = NULL;
	return t;
}

tree *createNode(tree *x)
{
	tree *t = new tree();
	t->setValue(x->getValue());
	t->setType(x->getType());
	t->left = x->left;
	t->right = NULL;
	return t;
}

void tree::print_tree(int dots_count) // Function to print the AST/ST
{
	int n = 0;
	while (n < dots_count)
	{
		cout << ".";
		n++;
	}

	if (type == "ID" || type == "STR" || type == "INT")
	{
		cout << "<";
		cout << type;
		cout << ":";
	}

	if (type == "BOOL" || type == "NIL" || type == "DUMMY")
		cout << "<";

	cout << val;

	if (type == "ID" || type == "STR" || type == "INT")
		cout << ">";

	if (type == "BOOL" || type == "NIL" || type == "DUMMY")
		cout << ">";

	cout << endl;
	// recursive call to print the left and right subtrees
	if (left != NULL)
		left->print_tree(dots_count + 1);

	if (right != NULL)
		right->print_tree(dots_count);
}
#endif
