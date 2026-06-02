#include "RemoveItemCommand.h"

RemoveItemCommand::RemoveItemCommand(ShoppingList& l, const string& item) : list(l), item(item)
{
}

void RemoveItemCommand::execute()
{
    list.removeItem(item);
}

bool RemoveItemCommand::isModifiable() const
{
    return true;
}
