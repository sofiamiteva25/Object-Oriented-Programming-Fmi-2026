#pragma once
#include "Requirement.h"
#include <string>
#include <optional>
using namespace std;

class Resume;

class SkillRequirement : public Requirement {
private:
	string skill;

public:
	SkillRequirement(const string& skill);

	optional<string> check(const Resume& resume) const override;

	unique_ptr<Requirement> clone() const override;
};
