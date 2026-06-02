#pragma once
#include "ShoppingList.h"
#include "CommandHistory.h"
#include "ICommand.h"
class CommandFactory
{
	unique_ptr<ShoppingList>& list;
	CommandHistory& history;

public:
	CommandFactory(unique_ptr<ShoppingList>& l, CommandHistory& h);

	unique_ptr<ICommand> create(const string& command, const string& args);
};

