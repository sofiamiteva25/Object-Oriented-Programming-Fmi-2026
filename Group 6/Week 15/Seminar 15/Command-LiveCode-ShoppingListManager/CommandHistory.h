#pragma once
#include "ShoppingList.h"
class CommandHistory
{
	vector<unique_ptr<ShoppingList>> undoStack;
	vector<unique_ptr<ShoppingList>> redoStack;

public:
	void saveState(const ShoppingList& current);

	bool canUndo() const;
	bool canRedo() const;

	unique_ptr<ShoppingList> undo(const ShoppingList& current);
	unique_ptr<ShoppingList> redo(const ShoppingList& current);
};

