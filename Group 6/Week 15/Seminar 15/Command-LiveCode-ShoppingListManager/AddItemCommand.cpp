#include "AddItemCommand.h"

AddItemCommand::AddItemCommand(ShoppingList& list, const string& item) : list(list), item(item)
{
}

void AddItemCommand::execute()
{
    list.addItem(item);
}

bool AddItemCommand::isModifiable() const
{
    return true;
}
