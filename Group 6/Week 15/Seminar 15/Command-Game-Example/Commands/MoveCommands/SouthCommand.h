#pragma once
#include "MoveCommand.h"

class Game;

class SouthCommand : public MoveCommand {
public:
	SouthCommand(Game& game);

	static constexpr char CMD[] = "south";
};
