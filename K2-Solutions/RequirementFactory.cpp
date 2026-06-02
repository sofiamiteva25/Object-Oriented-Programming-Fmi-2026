#include "RequirementFactory.h"
#include "MinExpRequirement.h"
#include "MaxSalaryRequirement.h"
#include "MinAgeRequirement.h"
#include "SkillRequirement.h"
#include <stdexcept>

unique_ptr<Requirement> RequirementFactory::create(const pair<string, string>& input)
{
    if (input.first == "min_experience") {
        return make_unique<MinExpRequirement>(stoi(input.second));
    }
    if (input.first == "skill") {
        return make_unique<SkillRequirement>(input.second);
    }
    if (input.first == "min_age") {
        return make_unique<MinAgeRequirement>(stoi(input.second));
    }
    if (input.first == "max_salary") {
        return make_unique<MaxSalaryRequirement>(stod(input.second));
    }

    throw invalid_argument("No such requirement.");
}
