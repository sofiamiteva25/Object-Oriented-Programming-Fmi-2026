#include "MinExpRequirement.h"
#include <stdexcept>

using namespace std;

MinExpRequirement::MinExpRequirement(int minXp) : minXp(minXp)
{
    if (minXp < 1) {
        throw invalid_argument("Minimum experence required must be a positive integer.");
    }

    minXpTxt = to_string(minXp);
}

optional<string> MinExpRequirement::check(const Resume& resume) const
{
    if (resume.getYearsXp() >= minXp) {
        return nullopt;
    }
    
    return "Not enough experience: ( " + to_string(resume.getYearsXp()) + " < " + minXpTxt + " )";
}

unique_ptr<Requirement> MinExpRequirement::clone() const
{
    return make_unique<MinExpRequirement>(*this);
}
