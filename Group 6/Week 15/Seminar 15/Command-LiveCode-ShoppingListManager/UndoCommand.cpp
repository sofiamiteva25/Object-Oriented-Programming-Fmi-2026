#include "UndoCommand.h"

UndoCommand::UndoCommand(CommandHistory& h, unique_ptr<ShoppingList>& curr) :
    history(h),current(curr)
{
}

void UndoCommand::execute()
{
    if (!history.canUndo()) {
        cout << "Cannot undo\n";
        return;
    }
    current = history.undo(*current);
}

bool UndoCommand::isModifiable() const
{
    return false;
}
