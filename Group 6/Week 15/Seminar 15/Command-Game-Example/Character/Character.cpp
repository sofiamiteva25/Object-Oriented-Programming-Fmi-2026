#include "Character.h"
#include "../Grid/Grid.h"

#include <stdexcept>

Character::Character(const std::string& name, char symbol) : name(name), symbol(symbol), x(0), y(0) {
	if (name.empty()) {
		throw std::invalid_argument("Name cannot be empty.");
	}
	else if (symbol == Grid::UNMARKED_SYMBOL) {
		throw std::invalid_argument("\'-\' is not a valid character symbol.");
	}
	else if (symbol == Grid::MARKED_SYMBOL) {
		throw std::invalid_argument("\'X\' is not a valid character symbol.");
	}
}

void Character::move(int x, int y) {
	this->x = x;
	this->y = y;
}

int Character::getX() const {
	return x;
}

int Character::getY() const {
	return y;
}

char Character::getSymbol() const {
	return symbol;
}

const std::string& Character::getName() const {
	return name;
}

bool Character::isAt(int x, int y) const {
	return this-> x == x && this->y == y;
}
