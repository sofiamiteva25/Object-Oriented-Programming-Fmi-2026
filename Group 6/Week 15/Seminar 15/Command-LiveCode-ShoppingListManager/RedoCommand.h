#pragma once
#include "ICommand.h"
#include "ShoppingList.h"
#include "CommandHistory.h"
class RedoCommand : public ICommand
{
	CommandHistory& history;
	unique_ptr<ShoppingList>& current;
public:

	RedoCommand(CommandHistory& h, unique_ptr<ShoppingList>& curr);



	// Inherited via ICommand
	void execute() override;

	bool isModifiable() const override;

};
