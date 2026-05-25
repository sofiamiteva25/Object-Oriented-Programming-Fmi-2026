#pragma once
#include "MoveCommand.h"

class Game;

class NorthCommand : public MoveCommand {
public:
	NorthCommand(Game& game);

	static constexpr char CMD[] = "north";
};
