#include "CommandFactory.h"
#include "AddItemCommand.h"
#include "ClearCommand.h"
#include "RemoveItemCommand.h"
#include "PrintCommand.h"
#include "UndoCommand.h"
#include "RedoCommand.h"

CommandFactory::CommandFactory(unique_ptr<ShoppingList>& l, CommandHistory& h) :
	list(l), history(h)
{
}

unique_ptr<ICommand> CommandFactory::create(const string& command, const string& args)
{
	if (command == "add") {
		return make_unique<AddItemCommand>(*list,args);
	}
	if (command == "remove") {
		return make_unique<RemoveItemCommand>(*list, args);
	}
	if (command == "clear") {
		return make_unique<ClearCommand>(*list);
	}
	if (command == "print") {
		return make_unique<PrintCommand>(*list);
	}
	if (command == "undo") {
		return make_unique<UndoCommand>(history ,list);
	}
	if (command == "redo") {
		return make_unique<RedoCommand>(history, list);
	}

	throw std::invalid_argument("invalid type");
}
