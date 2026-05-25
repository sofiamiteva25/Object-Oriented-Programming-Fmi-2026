#pragma once
#include "../Command.h"

class MoveCommand : public Command {
	int oldX, oldY;
	int newX, newY;

	void announceMovement() const;
	void announceUndoMovement() const;

protected:
	MoveCommand(Game& game, int dx, int dy);

public:
	void exec() override;
	void undo() override;
};
