#pragma once
#include "MoveCommand.h"

class Game;

class EastCommand : public MoveCommand {
public:
	EastCommand(Game& game);

	static constexpr char CMD[] = "east";
};
