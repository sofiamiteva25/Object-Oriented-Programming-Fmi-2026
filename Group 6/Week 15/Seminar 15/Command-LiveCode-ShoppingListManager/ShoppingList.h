#pragma once
#include <iostream>
#include <vector>
#include <string>
using namespace std;
class ShoppingList 
{
	string name;
	vector<string> products;
public:
	ShoppingList(const string& name);
	void addItem(const string& name);
	void removeItem(const string& name);
	void clear();
	void print() const;

	unique_ptr<ShoppingList> clone() const;

};

