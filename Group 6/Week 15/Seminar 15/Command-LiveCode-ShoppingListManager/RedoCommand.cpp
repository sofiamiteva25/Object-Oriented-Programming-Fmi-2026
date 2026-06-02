#include "RedoCommand.h"

RedoCommand::RedoCommand(CommandHistory& h, unique_ptr<ShoppingList>& curr): history(h), current(curr)
{
}

void RedoCommand::execute()
{
    if (!history.canRedo()) {
        cout << "Nqma operaciq za povtorenie\n";
        return;
    }
    current = history.redo(*current);
}

bool RedoCommand::isModifiable() const
{
    return false;
}
