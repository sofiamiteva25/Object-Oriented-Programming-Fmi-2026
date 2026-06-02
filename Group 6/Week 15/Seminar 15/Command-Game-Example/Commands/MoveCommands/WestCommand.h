#pragma once
#include "MoveCommand.h"

class Game;

class WestCommand : public MoveCommand {
public:
	WestCommand(Game& game);

	static constexpr char CMD[] = "west";
};
