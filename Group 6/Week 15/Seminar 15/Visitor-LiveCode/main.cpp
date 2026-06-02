
#include <iostream>
#include <vector>
#include "Person.h"
#include "IBuilding.h"
#include "House.h"
#include "Factory.h"
#include "Bank.h"
#include "Inspector.h"

int main()
{
    std::vector<std::unique_ptr<IBuilding>> buildings;
    buildings.push_back(std::make_unique<House>());
    buildings.push_back(std::make_unique<Factory>());
    buildings.push_back(std::make_unique<Bank>());

    Person person;

    std::cout << "=== Човекът обикаля ===\n";
    for (const auto& b : buildings)
        b->accept(person);

    Inspector inspector;
    for (const auto& b : buildings)
        b->accept(inspector);

    // WE CAN ADD Fireman as well without touching any of the classes!!!
    /*FiremanVisitor   fireman;

    std::cout << "\n=== Пожарникарят обикаля ===\n";
    for (const auto& b : buildings)
        b->accept(&fireman);*/
}

