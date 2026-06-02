#include "CommandHistory.h"
#include "../Command.h"

#include <stdexcept>

void CommandHistory::add(std::unique_ptr<Command> cmd) {
	commands.push_back(std::move(cmd));
}

void CommandHistory::undoLast() {
	if (!commands.empty()) {
		commands.back()->undo();
		commands.pop_back();
	}
	else {
		throw std::logic_error("No commands to undo.");
	}
}
