#include "ResumePool.h"

optional<Resume> ResumePool::findByEmail(const string& email) const
{
    return findFirst([&](const Resume& r) {
                return r.getEmail() == email;
               });
}

vector<Resume> ResumePool::filterBySkill(const string& skill) const
{
    return filter([&](const Resume& r) {
        return find(r.getSkills().begin(), r.getSkills().end(), skill) != r.getSkills().end();
        });
}

double ResumePool::averageSalary() const
{
    if (isEmpty()) { return 0; }

    double sum = 0;
    for (const auto& el : data) {
        sum += el.getDesiredSalary();
    }

    return sum / size();
}
