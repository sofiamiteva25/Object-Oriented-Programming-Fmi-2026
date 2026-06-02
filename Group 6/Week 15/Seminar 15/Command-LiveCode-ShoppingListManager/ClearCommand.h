#pragma once
#include "ICommand.h"
#include "ShoppingList.h"
class ClearCommand : public ICommand
{
	ShoppingList& list;

public:
	explicit ClearCommand(ShoppingList& l);

	void execute() override;
	bool isModifiable() const override;
};

