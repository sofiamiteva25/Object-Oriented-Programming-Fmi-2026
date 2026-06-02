#pragma once
#include "ICommand.h"
#include "ShoppingList.h"
class PrintCommand : public ICommand
{

	const ShoppingList& list;
public:
	PrintCommand(const ShoppingList& l);



	void execute() override;

	bool isModifiable() const override;

};

