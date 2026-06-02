#pragma once
#include "MoveCommand.h"

class Game;

class NorthEastCommand : public MoveCommand {
public:
	NorthEastCommand(Game& game);

	static constexpr char CMD[] = "northeast";
};
