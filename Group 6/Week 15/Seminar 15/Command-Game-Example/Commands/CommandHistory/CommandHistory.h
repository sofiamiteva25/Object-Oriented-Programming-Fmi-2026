#pragma once
#include <memory>
#include <vector>
#include "../Command.h"

class CommandHistory {
	std::vector<std::unique_ptr<Command>> commands;

public:
	void add(std::unique_ptr<Command> cmd);
	void undoLast();
};
