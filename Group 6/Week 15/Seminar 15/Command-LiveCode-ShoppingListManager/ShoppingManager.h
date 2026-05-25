#pragma once
#include "ShoppingList.h"
#include "CommandHistory.h"
#include "CommandFactory.h"
class ShoppingManager
{
	unique_ptr<ShoppingList> list;
	CommandHistory  history;
	CommandFactory factory;

public:
	ShoppingManager(const string& name);

	void run();
};

