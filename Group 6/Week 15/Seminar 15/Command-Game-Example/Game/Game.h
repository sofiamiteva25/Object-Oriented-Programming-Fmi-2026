#pragma once
#include "../Grid/Grid.h"
#include "../Character/Character.h"
#include "../Commands/CommandHistory/CommandHistory.h"

#include <memory>

class Command;

class Game {
	Grid& grid;
	Character& character;

	CommandHistory cmdHistory;

public:
	Game(Grid& grid, Character& character);

	void moveChar(int x, int y);
	void returnChar(int x, int y);
	const Character& getCharacter() const;

	void markGrid(int x, int y);
	void unmarkGrid(int x, int y);

	void print() const;

	void addCommand(std::unique_ptr<Command> cmd);
	void undo();
};
