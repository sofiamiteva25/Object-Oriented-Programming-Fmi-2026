#include "ClearCommand.h"

ClearCommand::ClearCommand(ShoppingList& l) : list(l)
{
}

void ClearCommand::execute()
{
	list.clear();
}

bool ClearCommand::isModifiable() const
{
	return true;
}
