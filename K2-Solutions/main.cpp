#include <iostream>
#include <memory>
#include <vector>
#include "ResumeSelectionSystem.h"
using namespace std;

int main()
{
    std::vector<std::pair<std::string, std::string>> config = {
{"min_experience", "3"},
{"skill", "C++"},
{"skill", "OOP"},
{"max_salary", "8000"},
{"min_age", "21"},
    };
    ResumeSelectionSystem system;
    for (const auto& req : config) {
        system.addRequirement(req);
    }
    // Резюме което покрива всички изисквания
    Resume r1("Ivan", "Ivanov", "ivan@mail.com", 25, 5, { "C++", "OOP", "SQL" }, 7000.0);
    auto result1 = system.addResume(r1);
    if (result1.empty()) {
        std::cout << "Accepted! \n";
    }

    // Резюме което НЕ покрива изискванията
    Resume r2("Mariya", "Petrova", "maria@mail.com", 19, 1, { "Java" }, 9000.0);
    auto result2 = system.addResume(r2);
    if (!result2.empty()) {
        for (const auto& err : result2)
            std::cout << "Rejected: " << err << "\n";
    }
    // Отказано: Недостатъчен опит (1 < 3 години)
    // Отказано: Липсва умение: C++
    // Отказано: Липсва умение: OOP
    // Отказано: Желана заплата надвишава максимума
    // Отказано: Кандидатът е под минималната възраст
    // Дублиран имейл
    auto result3 = system.addResume(r1);
    if (!result3.empty()) {
        std::cout << result3[0] << "\n"; // Резюме с този имейл вече съществува
    }
    auto found = system.findResume("ivan@mail.com");
    if (found) std::cout << (*found).getFirstName() << " " << (*found).getLastName() << "\n";

    // Филтриране чрез пула
    auto cppDevs = system.pool().filterBySkill("C++");
    std::cout << "C++ candidates: " << cppDevs.size() << "\n";
    std::cout << "Average salary: " << system.pool().averageSalary() << "\n";
    // Pool с произволен тип
    Pool<std::string> tags;
    tags.add("senior"); tags.add("remote"); tags.add("junior");
    auto senior = tags.filter([](const auto& t) { return t == "senior"; });
    for (const auto& el : senior) {
        cout << el << " ";
    }
    cout << endl;

    // Копиране - независими изисквания, промени по изискванията в system2 не засягат system
    ResumeSelectionSystem system2 = system;
}
