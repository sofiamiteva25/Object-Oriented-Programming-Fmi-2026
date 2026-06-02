#pragma once
#include "../Command.h"
#include <optional>
#include <memory>
#include <string>

class Game;

class CommandFactory {
	static constexpr char UNDO_CMD[] = "undo";

public:
	static std::optional<std::unique_ptr<Command>> create(const std::string& input, Game& game);
};
