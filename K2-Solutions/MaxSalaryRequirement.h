#pragma once
#include "Requirement.h"
#include <string>
#include <optional>
#include <memory>
#include "Resume.h"
using namespace std;


class MaxSalaryRequirement : public Requirement {
private:
	float maxSalary;
	string maxSalTxt;

public:
	MaxSalaryRequirement(float maxSalary);

	optional<string> check(const Resume& resume) const override;

	unique_ptr<Requirement> clone() const override;
};
