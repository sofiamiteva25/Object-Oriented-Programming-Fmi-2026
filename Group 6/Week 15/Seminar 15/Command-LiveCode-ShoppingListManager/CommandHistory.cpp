#include "CommandHistory.h"

void CommandHistory::saveState(const ShoppingList& current)
{
	undoStack.push_back(current.clone());
	redoStack.clear();
}

bool CommandHistory::canUndo() const
{
	return !undoStack.empty();
}

bool CommandHistory::canRedo() const
{
	return !redoStack.empty();
}

unique_ptr<ShoppingList> CommandHistory::undo(const ShoppingList& current)
{
	if (!canUndo()) {
		throw std::logic_error("Cannot undo");
	}
	redoStack.push_back(current.clone());
	auto prev = std::move(undoStack.back());
	undoStack.pop_back();
	return prev;
}

unique_ptr<ShoppingList> CommandHistory::redo(const ShoppingList& current)
{
	if (!canRedo()) {
		throw std::logic_error("Cannot redo");
	}
	undoStack.push_back(current.clone());
	auto next = std::move(redoStack.back());
	redoStack.pop_back();
	return next;
}
