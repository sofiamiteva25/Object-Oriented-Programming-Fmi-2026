#include "MaxSalaryRequirement.h"
#include <stdexcept>

MaxSalaryRequirement::MaxSalaryRequirement(float maxSalary) : maxSalary(maxSalary)
{
	if (maxSalary <= 0) {
		throw invalid_argument("Maximum salary must a positive number.");
	}

	maxSalTxt = to_string(maxSalary);
}

optional<string> MaxSalaryRequirement::check(const Resume& resume) const
{
	if (maxSalary > resume.getDesiredSalary()) {
		return nullopt;
	}

	return "The desired salary is over the maximum of " + maxSalTxt;
}

unique_ptr<Requirement> MaxSalaryRequirement::clone() const
{
	return make_unique<MaxSalaryRequirement>(*this);
}
