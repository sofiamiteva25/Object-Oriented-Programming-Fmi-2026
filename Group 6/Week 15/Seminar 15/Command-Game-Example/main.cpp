#include <iostream>
#include <print>

#include "Game/Game.h"
#include "Grid/Grid.h"
#include "Character/Character.h"
#include "Commands/Factory/CommandFactory.h"

int main() {
	Character c("Mitio", '*');
	Grid gr(9);
	Game game(gr, c);

	game.print();

	std::string input;
	while (true) {
		std::cin >> input;

		if (input == "exit") return 0;

		try {
			auto cmd = CommandFactory::create(input, game);

			if (cmd.has_value()) {
				try {
					(*cmd)->exec();
					game.addCommand(std::move(*cmd));
				}
				catch (const std::exception& e) {
					std::print("{}\n", e.what());
				}
				catch (...) {
					std::println("Unexpected exception.");
				}
			}

			game.print();
		}
		catch (const std::exception& e) {
			std::print("{}\n", e.what());
		}
		catch (...) {
			std::println("Unexpected exception.");
		}
	}

	return 0;
}