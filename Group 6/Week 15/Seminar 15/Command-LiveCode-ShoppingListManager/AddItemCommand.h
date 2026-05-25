#pragma once
#include "ICommand.h"
#include "ShoppingList.h"
class AddItemCommand : public ICommand
{
	ShoppingList& list;
	string item;

public:

	AddItemCommand(ShoppingList& list, const string& item);

	void execute() override;

	bool isModifiable() const override;

};

