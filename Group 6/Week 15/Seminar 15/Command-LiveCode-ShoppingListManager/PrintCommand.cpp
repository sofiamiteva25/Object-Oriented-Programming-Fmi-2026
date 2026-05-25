#include "PrintCommand.h"

PrintCommand::PrintCommand(const ShoppingList& l) : list(l)
{
}

void PrintCommand::execute()
{
    list.print();
}

bool PrintCommand::isModifiable() const
{
    return false;
}
