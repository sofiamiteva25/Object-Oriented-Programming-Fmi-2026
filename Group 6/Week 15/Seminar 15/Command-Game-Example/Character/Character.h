#pragma once
#include <string>

class Character {
	std::string name;
	char symbol;

	int x;
	int y;

public:
	Character(const std::string& name, char symbol);

	void move(int x, int y);

	int getX() const;
	int getY() const;
	char getSymbol() const;
	const std::string& getName() const;

	bool isAt(int x, int y) const;
};
