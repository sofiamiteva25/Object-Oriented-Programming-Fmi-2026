#include "Game.h"

#include <stdexcept>
#include <print>

Game::Game(Grid& grid, Character& character) : grid(grid), character(character) { }

void Game::moveChar(int x, int y) {
	if (!grid.isInside(x, y)) {
		Grid::gridThrowOutside();
	}
	if (grid.isMarked(x, y)) {
		throw std::logic_error("Cannot step on marked area.");
	}

	character.move(x, y);
}

void Game::returnChar(int x, int y) {
	if (!grid.isInside(x, y)) {
		Grid::gridThrowOutside();
	}

	character.move(x, y);
}

const Character& Game::getCharacter() const {
	return character;
}

void Game::markGrid(int x, int y) {
	grid.mark(x, y);
}

void Game::unmarkGrid(int x, int y) {
	grid.unmark(x, y);
}

void Game::addCommand(std::unique_ptr<Command> cmd) {
	cmdHistory.add(std::move(cmd));
}

void Game::undo() {
	cmdHistory.undoLast();
}

void Game::print() const {
	std::print("{}  ", '\n');
	for (size_t i = 0; i < grid.getSize(); i++)
	{
		std::print("{} ", i);
	}
	std::println();

	for (size_t i = 0; i < grid.getSize(); i++)
	{
		std::print("{} ", i);

		for (size_t j = 0; j < grid.getSize(); j++)
		{
			char symbol = Grid::UNMARKED_SYMBOL;
			if (character.isAt(i, j)) {
				symbol = character.getSymbol();
			}
			else if (grid.isMarked(i, j)) {
				symbol = Grid::MARKED_SYMBOL;
			}

			std::print("{} ", symbol);
		}

		std::println();
	}

	std::println();
	std::println();
}
