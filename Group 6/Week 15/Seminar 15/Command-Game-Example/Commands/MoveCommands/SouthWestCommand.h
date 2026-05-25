#pragma once
#include "MoveCommand.h"

class Game;

class SouthWestCommand : public MoveCommand {
public:
	SouthWestCommand(Game& game);

	static constexpr char CMD[] = "southwest";
};
