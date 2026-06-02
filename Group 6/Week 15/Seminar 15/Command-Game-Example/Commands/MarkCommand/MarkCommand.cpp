#include "MarkCommand.h"
#include "../../Game/Game.h"

#include <print>

void MarkCommand::announceMarking() const {
	std::println("Area at ({}, {}) was marked and cannot be visited.",
		game.getCharacter().getX(), game.getCharacter().getY());
}

void MarkCommand::announceUnmarking() const {
	std::println("Area at ({}, {}) was unmarked and can now be visited again.",
		game.getCharacter().getX(), game.getCharacter().getY());
}

MarkCommand::MarkCommand(Game& game) : Command(game) { }

void MarkCommand::exec() {
	game.markGrid(game.getCharacter().getX(), game.getCharacter().getY());
	announceMarking();
}

void MarkCommand::undo() {
	game.unmarkGrid(game.getCharacter().getX(), game.getCharacter().getY());
	announceUnmarking();
}
