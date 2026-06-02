#pragma once

class Game;

class Command {
protected:
	Game& game;

public:
	Command(Game& game);

	virtual void exec() = 0;
	virtual void undo() = 0;

	virtual ~Command() = default;
};
