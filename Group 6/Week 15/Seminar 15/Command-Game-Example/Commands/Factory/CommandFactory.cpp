#include "CommandFactory.h"
#include "../../Game/Game.h"
#include "../MoveCommands/NorthCommand.h"
#include "../MoveCommands/SouthCommand.h"
#include "../MoveCommands/WestCommand.h"
#include "../MoveCommands/EastCommand.h"
#include "../MoveCommands/NorthEastCommand.h"
#include "../MoveCommands/SouthEastCommand.h"
#include "../MoveCommands/NorthWestCommand.h"
#include "../MoveCommands/SouthWestCommand.h"
#include "../MarkCommand/MarkCommand.h"

#include <stdexcept>

std::optional<std::unique_ptr<Command>> CommandFactory::create(const std::string& input, Game& game)
{
    if (input == NorthCommand::CMD) {
        return std::make_unique<NorthCommand>(game);
    }
    else if (input == SouthCommand::CMD) {
        return std::make_unique<SouthCommand>(game);
    }
    else if (input == WestCommand::CMD) {
        return std::make_unique<WestCommand>(game);
    }
    else if (input == EastCommand::CMD) {
        return std::make_unique<EastCommand>(game);
    }
    else if (input == NorthEastCommand::CMD) {
        return std::make_unique<NorthEastCommand>(game);
    }
    else if (input == SouthEastCommand::CMD) {
        return std::make_unique<SouthEastCommand>(game);
    }
    else if (input == NorthWestCommand::CMD) {
        return std::make_unique<NorthWestCommand>(game);
    }
    else if (input == SouthWestCommand::CMD) {
        return std::make_unique<SouthWestCommand>(game);
    }

    if (input == MarkCommand::CMD) {
        return std::make_unique<MarkCommand>(game);
    }

    if (input == UNDO_CMD) {
        game.undo();
        return std::nullopt;
    }

    throw std::invalid_argument("Invalid command.");
}
