#pragma once
#include "Requirement.h"
#include <string>
#include <optional>
#include <memory>
#include "Resume.h"
using namespace std;


class MinAgeRequirement : public Requirement {
private:
	int minAge;
	string minAgeTxt;

public:
	MinAgeRequirement(int minAge);

	optional<string> check(const Resume& resume) const override;

	unique_ptr<Requirement> clone() const override;
};
