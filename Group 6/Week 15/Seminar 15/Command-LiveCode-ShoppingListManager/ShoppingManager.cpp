#include "ShoppingManager.h"
using namespace std;

ShoppingManager::ShoppingManager(const string& name): list(make_unique<ShoppingList>(name)),
factory(list,history)
{

}

void ShoppingManager::run()
{
	try {
		std::string command, arg;
		while (std::cin >> command) {
			if (command == "exit") break;

			if (command == "add" || command == "remove")
				std::cin >> arg;
			else
				arg = "";

			auto cmd = factory.create(command, arg);
			if (cmd->isModifiable()) {
				history.saveState(*list);
			}
			cmd->execute();
		}
		
	}
	catch (const invalid_argument& ex) {
		cout << ex.what();
	}
	
	
}
