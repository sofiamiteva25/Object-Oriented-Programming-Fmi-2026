#include "MoveCommand.h"
#include "../../Game/Game.h"
#include <print>

MoveCommand::MoveCommand(Game& game, int dx, int dy) : Command(game) {
	oldX = game.getCharacter().getX();
	oldY = game.getCharacter().getY();
	newX = oldX + dx;
	newY = oldY + dy;
}

void MoveCommand::announceMovement() const {
	std::println("{} moved from ({}, {}) to ({}, {}).",
		game.getCharacter().getName(), oldX, oldY, newX, newY);
}

void MoveCommand::announceUndoMovement() const {
	std::println("{} cancelled movement from ({}, {}) to ({}, {}).",
		game.getCharacter().getName(), oldX, oldY, newX, newY);
}

void MoveCommand::exec() {
	game.moveChar(newX, newY);
	announceMovement();
}

void MoveCommand::undo() {
	game.returnChar(oldX, oldY);
	announceUndoMovement();
}
