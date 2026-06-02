#pragma once
#include "ICommand.h"
#include "ShoppingList.h"
class RemoveItemCommand : public ICommand
{
	ShoppingList& list;
	string item;
public:
	RemoveItemCommand(ShoppingList& l, const string& item);

	void execute() override;

	bool isModifiable() const override;

};

