#pragma once
#include "MoveCommand.h"

class Game;

class SouthEastCommand : public MoveCommand {
public:
	SouthEastCommand(Game& game);

	static constexpr char CMD[] = "southeast";
};
