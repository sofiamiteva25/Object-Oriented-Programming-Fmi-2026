#include "MinAgeRequirement.h"
#include <stdexcept>

MinAgeRequirement::MinAgeRequirement(int minAge) : minAge(minAge)
{
	if (minAge < 1) {
		throw invalid_argument("Minimum required age must be a positive integer.");
	}

	minAgeTxt = to_string(minAge);
}

optional<string> MinAgeRequirement::check(const Resume& resume) const
{
	if (minAge <= resume.getAge()) {
		return nullopt;
	}

	return "Candidate is under the required minimum age of " + minAgeTxt;
}

unique_ptr<Requirement> MinAgeRequirement::clone() const
{
	return make_unique<MinAgeRequirement>(*this);
}
