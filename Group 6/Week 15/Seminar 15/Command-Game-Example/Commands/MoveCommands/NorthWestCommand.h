#pragma once
#include "MoveCommand.h"

class Game;

class NorthWestCommand : public MoveCommand {
public:
	NorthWestCommand(Game& game);

	static constexpr char CMD[] = "northwest";
};
