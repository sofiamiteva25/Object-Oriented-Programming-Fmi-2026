#pragma once
#include "../Command.h"

class MarkCommand : public Command {
	void announceMarking() const;
	void announceUnmarking() const;

public:
	MarkCommand(Game& game);

	void exec() override;
	void undo() override;

	static constexpr char CMD[] = "mark";
};
