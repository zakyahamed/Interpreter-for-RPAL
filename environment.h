#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_
#include <iostream>
#include <map>

using namespace std;

class environment // Environment class to store the environment objects and their bindings
{
public:
	environment *prev;
	string name;
	map<tree *, vector<tree *>> boundVar; // Map to store the bindings of variables to environment objects

	environment()
	{ // Constructor to initialize the environment object
		prev = NULL;
		name = "env0";
	}

	environment(const environment &); // Copy constructor to copy the environment object

	environment &operator=(const environment &env); // Assignment operator to assign the environment object
};

#endif
